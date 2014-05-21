////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFAnimatedObject.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "GAFAnimatedObject.h"
#import "GAFSprite.h"
#import "GAFTextureAtlas.h"
#import "GAFTextureAtlasElement.h"
#import "GAFNode.h"
#import "GAFSprite.h"
#import "GAFAsset.h"
#import "GAFSpriteWithAlpha.h"
#import "GAFAnimationFrame.h"
#import "GAFSubobjectState.h"
#import "GAFStencilMaskSprite.h"
#import "GAFAnimationSequence.h"
#import "GAFFilterData.h"
#import "CCSpriteFrame.h"
#import "NSString+GAFExtensions.h"
#import "CCDirector.h"
#import "GAFHelperMethods.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface GAFAnimatedObject ()

@property (nonatomic, assign) BOOL isInitialized;
@property (nonatomic, assign) BOOL isRunning;

@property (nonatomic, retain) NSMutableDictionary *capturedObjects;    // [Key]:inner ids of captured objects [Value]:controlFlags

- (GAFSprite *)subObjectForInnerObjectId:(NSString *)anInnerObjectId;
- (NSString *)objectIdByObjectName:(NSString *)aName;

/// Creates subobjects from [ObjectID]->[AtlasSegmentID] pairs
- (void)addSubObjectsUsingAnimationObjectsDictionary:(NSDictionary *)anAnimationObjects
                            animationMasksDictionary:(NSDictionary *)anAnimationMasks;
- (void)removeAllSubObjects;

/// Processes animation frame that was already processed, doesn't move currentFrame pointer
/// (may be needed in case when subobject parameters were changed and require verification)
- (void)processCurrentAnimationFrameOnceMore;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation GAFAnimatedObject

#pragma mark -
#pragma mark Properties

@synthesize asset;
@synthesize subObjects;
@synthesize masks;
@synthesize framePlayedDelegate;
@synthesize currentFrameIndex;
@synthesize totalFrameCount;
@synthesize isRunning;
@synthesize FPSType;
@synthesize extraFramesCounter;

#pragma mark -
#pragma mark Initialization & Release

+ (GAFAnimatedObject *)animatedObjectWithAsset:(GAFAsset *)anAsset
{
    return [[[self alloc] initWithAsset:anAsset] ah_autorelease];
}

+ (GAFAnimatedObject *)animatedObjectWithPath:(NSString *)aJSONPath
{
    return [[[self alloc] initWithPath:aJSONPath] ah_autorelease];
}

+ (GAFAnimatedObject *)animatedObjectWithPath:(NSString *)aJSONPath runLooped:(BOOL)aLooped
{
    return [[[self alloc] initWithPath:aJSONPath runLooped:aLooped] ah_autorelease];
}

- (id)initWithAsset:(GAFAsset *)anAsset
{
    if(anAsset == nil)
    {
        CCLOGWARN(@"ERROR: initializing GAFAnimatedObject.  anAsset not present");
        [self ah_autorelease];
        return nil;
    }
    
	if ((self = [super init]))
	{
		if (anAsset == nil)
		{
            [self ah_release];
			return nil;
		}
		self.asset = anAsset;
		self.subObjects = [NSMutableDictionary dictionary];
		self.masks = [NSMutableDictionary dictionary];
		_animationsSelectorScheduled = NO;
		self.FPSType = kGAFAnimationFPSType_30;
		self.extraFramesCounter = 0;
		self.currentSequenceStart = self.currentFrameIndex = GAF_FIRST_FRAME_INDEX;
        self.totalFrameCount = [self.asset.animationFrames count];
		self.currentSequenceEnd = self.totalFrameCount - 1;
		self.isRunning = NO;
        
        self.isInitialized = NO;
        self.capturedObjects = [[NSMutableDictionary new] ah_autorelease];
        
        self.externalNamedPartsTextureAtlases = [NSMutableDictionary dictionaryWithCapacity:0];
        self.hiddenSubobjectIds = [NSMutableArray array];
	}
	return self;
}

- (id)initWithPath:(NSString *)aJSONPath
{
    if(aJSONPath == nil)
    {
        CCLOGWARN(@"ERROR: initializing GAFAnimatedObject.  aJSONPath not present");
        [self ah_release];
        return nil;
    }
    
	GAFAsset *assetData = [[GAFAsset alloc] initWithJSONAtPath:aJSONPath keepImagesInAtlas:NO];
    if (assetData != nil)
    {
        return [self initWithAsset:[assetData ah_autorelease]];
    }
    else
    {
        CCLOGWARN(@"ERROR: initializing GAFAnimatedObject.  assetData is nil");
        [self ah_release];
        return nil;
    }
}

- (id)initWithPath:(NSString *)aJSONPath runLooped:(BOOL)aLooped
{
    if ( (self = [self initWithPath:aJSONPath]) )
    {
        self.isLooped = aLooped;
        [self start];
        
        return self;
    }
	
    return nil;
}

- (void)dealloc
{
    [_externalNamedPartsTextureAtlases ah_release];
    [_hiddenSubobjectIds ah_release];
    [asset ah_release];
    [subObjects ah_release];
    [masks ah_release];
    [_capturedObjects ah_release];
    
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Public methods

- (CGRect)realBoundingBoxForCurrentFrame
{
    CGRect result = CGRectNull;
    for (GAFSprite *subObject in [self.subObjects allValues])
    {
        if (subObject.visible)
        {
            CGRect bb = [subObject boundingBox];
            result = CGRectUnion(result, bb);
        }
    }
    
    return CGRectApplyAffineTransform(result, [self nodeToParentTransform]);
}

- (void)addSubObjectsUsingAnimationObjectsDictionary:(NSDictionary *)anAnimationObjects
                            animationMasksDictionary:(NSDictionary *)anAnimationMasks
{   
    // Adding sub objects
    for (NSString *objectId in anAnimationObjects)
    {
        NSString *atlasElementId = anAnimationObjects[objectId];
        CCSpriteFrame *spriteFrame = nil;
        
        GAFTextureAtlasElement *element = self.asset.textureAtlas.elements[atlasElementId];
        if (nil != element)
        {
            if ([self.asset.textureAtlas.textures count] > element.atlasIdx)
            {
                CCTexture2D *texture = self.asset.textureAtlas.textures[element.atlasIdx];
                spriteFrame = [CCSpriteFrame frameWithTexture:texture rect:element.bounds];
            }
            else
            {
                CCLOGWARN(@"Cannot add sub object with Id: %@, atlas with idx: %u not found.",
                           atlasElementId, element.atlasIdx);
            }
        }
        else if (self.externalNamedPartsTextureAtlases.count > 0) // Try using named objects' atlas
        {
            NSString *namedPartId = self.asset.namedParts[objectId];
            if (namedPartId != nil)
            {
                // Search external atlases for such object
                GAFTextureAtlas *externalAtlas = nil;
                for (externalAtlas in [self.externalNamedPartsTextureAtlases allValues])
                {
                    element = externalAtlas.elements[namedPartId];
                    if (element != nil)
                        break;
                }
                
                if (nil != element)
                {
                    if ([externalAtlas.textures count] > element.atlasIdx)
                    {
                        CCTexture2D *texture = externalAtlas.textures[element.atlasIdx];
                        spriteFrame = [CCSpriteFrame frameWithTexture:texture rect:element.bounds];
                    }
                    else
                    {
                        CCLOGWARN(@"Cannot add sub object with Id: %@, atlas with idx: %u not found.",
                                  atlasElementId, element.atlasIdx);
                    }
                }
            }
        }
        
        if (spriteFrame != nil)
        {
            GAFSpriteWithAlpha *sprite = [[GAFSpriteWithAlpha alloc] initWithSpriteFrame:spriteFrame];
            
            sprite.objectId = objectId;
            sprite.atlasElementId = atlasElementId;
            
            sprite.visible = NO;
            sprite.anchorPoint = CGPointMake(element.pivotPoint.x / sprite.contentSize.width,
                                             1 - (element.pivotPoint.y / sprite.contentSize.height));
            sprite.useExternalTransform = YES;
            if (element.scale != 1.0f)
			{
				sprite.atlasScale = 1.0f / element.scale;
			}
            
            sprite.blendFunc = (ccBlendFunc){ GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
            
            // Add to subobjects
            (self.subObjects)[objectId] = sprite;
            
            [sprite ah_release];
        }
        else
        {
            CCLOGWARN(@"Cannot add subnode with AtlasElementName: %@, not found in atlas(es). Ignoring.",
                            atlasElementId);
        }
    }
    
    // Adding masks
    for (NSString *maskId in [anAnimationMasks allKeys])
    {
        NSString *atlasElementId = anAnimationMasks[maskId];
        CCSpriteFrame *spriteFrame = nil;
        
        GAFTextureAtlasElement *element = (self.asset.textureAtlas.elements)[atlasElementId];
        if (nil != element)
        {
            if ([self.asset.textureAtlas.textures count] > element.atlasIdx)
            {
                CCTexture2D *texture = self.asset.textureAtlas.textures[element.atlasIdx];
                spriteFrame = [CCSpriteFrame frameWithTexture:texture rect:element.bounds];
            }
            else
            {
                CCLOGWARN(@"Cannot add sub object with Id: %@, atlas with idx: %u not found.",
                          atlasElementId, element.atlasIdx);
            }
        }
        else if (self.externalNamedPartsTextureAtlases.count > 0) // Try using named objects' atlas
        {
            NSString *maskPartId = self.asset.namedParts[maskId];
            if (maskPartId != nil)
            {
                // Search external atlases for such object
                GAFTextureAtlas *externalAtlas = nil;
                for (externalAtlas in [self.externalNamedPartsTextureAtlases allValues])
                {
                    element = externalAtlas.elements[maskPartId];
                    if (element != nil)
                        break;
                }
                
                if (nil != element)
                {
                    if ([externalAtlas.textures count] > element.atlasIdx)
                    {
                        CCTexture2D *texture = externalAtlas.textures[element.atlasIdx];
                        spriteFrame = [CCSpriteFrame frameWithTexture:texture rect:element.bounds];
                    }
                    else
                    {
                        CCLOGWARN(@"Cannot add sub object with Id: %@, atlas with idx: %u not found.",
                                  atlasElementId, element.atlasIdx);
                    }
                }
            }
        }
		else
		{
			CCLOGWARN(@"Can not get atlasElementId for key %@. Animation can not work as expected.", maskId);
		}
        
        if (spriteFrame != nil)
        {
            GAFStencilMaskSprite *mask = [[GAFStencilMaskSprite alloc] initWithSpriteFrame:spriteFrame];
            
            mask.objectId = maskId;
            mask.atlasElementId = atlasElementId;
            mask.anchorPoint = CGPointMake(element.pivotPoint.x / mask.contentSize.width,
                                           1 - (element.pivotPoint.y / mask.contentSize.height));
            mask.useExternalTransform = YES;
            
            // Add to masks
            (self.masks)[maskId] = mask;
            [self addChild:mask];
            
            [mask ah_release];
        }
    }
}

- (void)removeAllSubObjects
{
    [self.subObjects enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop)
    {
        ((GAFSprite *)obj).visible = NO;
        [obj removeFromParentAndCleanup:YES];
    }];
    
    [self.subObjects removeAllObjects];
}

- (GAFSprite *)subobjectByName:(NSString *)aName
{
	if (aName == nil)
	{
		return nil;
	}
	
	NSString *rawName = [self objectIdByObjectName:aName];
	return (rawName != nil) ? [self subobjectByRawName:rawName] : nil;
}

- (GAFSprite *)subobjectByRawName:(NSString *)aRawName
{
	return (GAFSprite *)(self.subObjects)[aRawName];
}

- (BOOL)isSubobjectHiddenWithName:(NSString *)aName
{
	NSString *rawName = [self objectIdByObjectName:aName];
    return [self.hiddenSubobjectIds containsObject:rawName];
}

- (void)setSubobjectHidden:(BOOL)isHidden withName:(NSString *)aName
{
	NSString *rawName = [self objectIdByObjectName:aName];
    BOOL alreadyHidden = [self.hiddenSubobjectIds containsObject:rawName];
    if (isHidden != alreadyHidden)
    {
        if (isHidden)
        {
            [self.hiddenSubobjectIds addObject:rawName];
            
            // Hide subobject if exists
            CCNode *subObject = self.subObjects[rawName];
            if (subObject != nil)
            {
                subObject.visible = NO;
            }
        }
        else
        {
            [self.hiddenSubobjectIds removeObject:rawName];
            [self processCurrentAnimationFrameOnceMore];
        }
    }
}

#pragma mark -

- (void)linkExternalNamedPartsAtlas:(GAFTextureAtlas *)aTextureAtlas forName:(NSString *)anAtlasName
{
    NSAssert(anAtlasName != nil, @"");
    if (anAtlasName == nil)
        return;
    
    [self removeLinkedAtlasForName:anAtlasName];
    
    // Add new atlas to animation
    if (aTextureAtlas != nil && self.externalNamedPartsTextureAtlases[anAtlasName] == nil)
    {
        // To save memory
        [aTextureAtlas releaseImages];
        
        self.externalNamedPartsTextureAtlases[anAtlasName] = aTextureAtlas;
        
        if (self.isInitialized)
        {
            // Get object (and mask) ids to recreate
            NSMutableDictionary *newObjects = [NSMutableDictionary dictionary];
            NSMutableDictionary *newMasks = [NSMutableDictionary dictionary];
            
            for (NSString *namedObjectId in aTextureAtlas.elements)
            {
                NSString *objectOrMaskID = [self objectIdByObjectName:namedObjectId];
                
                id object = self.asset.objects[objectOrMaskID];
                if (object != nil)
                {
                    newObjects[objectOrMaskID] = object;
                }
                else
                {
                    id mask = self.asset.masks[objectOrMaskID];
                    if (mask != nil)
                    {
                        newMasks[objectOrMaskID] = mask;
                    }
                }
            }
            
            // Adding new subobjects
            [self addSubObjectsUsingAnimationObjectsDictionary:newObjects
                                      animationMasksDictionary:newMasks];
            
            [self processCurrentAnimationFrameOnceMore];
        }
    }
}


- (void)removeLinkedAtlasForName:(NSString *)anAtlasName
{
    NSAssert(anAtlasName != nil, @"");
    if (anAtlasName == nil)
        return;
    
    // Remove old atlas from animation
    GAFTextureAtlas *oldAtlas = self.externalNamedPartsTextureAtlases[anAtlasName];
    if (oldAtlas != nil)
    {
        if (self.isInitialized)
        {
            // Remove all subobjects that were using old atlas
            NSMutableArray *spriteIdsToRemove = [NSMutableArray new];
            for (GAFSpriteWithAlpha *subObject in [self.subObjects allValues])
            {
                GAFTextureAtlasElement *element = oldAtlas.elements[subObject.atlasElementId];
                if (element != nil)
                {
                    [spriteIdsToRemove addObject:subObject.objectId];
                    
                    // Check if subobject isn't part of mask
                    if (subObject.parent != nil)
                    {
                        [self removeChild:subObject cleanup:YES];
                    }
                    else
                    {
                        GAFStencilMaskSprite *mask = [subObject maskContainer];
                        if (mask != nil)
                        {
                            [mask removeMaskedObject:subObject];
                        }
                    }
                }
            }
            for (NSString *subObjectId in spriteIdsToRemove)
            {
                [self.subObjects removeObjectForKey:subObjectId];
            }
            [spriteIdsToRemove ah_release];
            
            // Remove all masks that were using old atlas
            NSMutableArray *maskIdsToRemove = [NSMutableArray new];
            for (GAFStencilMaskSprite *mask in [self.masks allValues])
            {
                GAFTextureAtlasElement *element = oldAtlas.elements[mask.atlasElementId];
                if (element != nil)
                {
                    [maskIdsToRemove addObject:mask.objectId];
                    [self removeChild:mask cleanup:YES];
                    
                    [mask removeAllMaskedObjects];
                }
            }
            for (NSString *maskId in maskIdsToRemove)
            {
                [self.masks removeObjectForKey:maskId];
            }
            [maskIdsToRemove ah_release];
        }
        
        [self.externalNamedPartsTextureAtlases removeObjectForKey:anAtlasName];
    }
}

- (BOOL)captureControlOverSubobjectNamed:(NSString *)aName controlFlags:(GAFAnimatedObjectControlFlags)aControlFlags
{
    NSString *objectId = [self objectIdByObjectName:aName];
    if (objectId == nil)
        return NO;
    
    if ((self.capturedObjects)[objectId] != nil)
        return NO;
    
    self.capturedObjects[objectId] = @(aControlFlags);
    
    return YES;
}

- (void)releaseControlOverSubobjectNamed:(NSString *)aName
{
    NSString *objectId = [self objectIdByObjectName:aName];
    if (objectId != nil)
    {
        [self.capturedObjects removeObjectForKey:objectId];
    }
}

#pragma mark -

- (BOOL)isDone
{
    if (self.isLooped)
    {
        return NO;
    }
    else
    {
        return (self.currentFrameIndex >= self.totalFrameCount);
    }
}

- (void)start
{
    if (!self.isInitialized)
    {
        [self initialize];
    }
    
    if (!_animationsSelectorScheduled)
    {
        [self schedule:@selector(processAnimations:)];
        _animationsSelectorScheduled = YES;
    }
    
    self.isRunning = YES;
    
    [self step];
}

- (void)pause
{
    if (self.isRunning)
    {
        self.isRunning = NO;
    }
}

- (void)resume
{
    if (!self.isRunning)
    {
        self.isRunning = YES;
    }
}

- (void)stop
{
    if (self.isRunning)
    {
        self.currentFrameIndex = GAF_FIRST_FRAME_INDEX;
        self.isRunning = NO;
    }
	[self unschedule:@selector(processAnimations:)];
	_animationsSelectorScheduled = NO;
}

- (BOOL)setFrame:(NSUInteger)aFrameIndex
{
    if (aFrameIndex < self.totalFrameCount)
    {
        self.currentFrameIndex = aFrameIndex;
        [self processAnimation];
		return YES;
    }
	return NO;
}

- (BOOL)gotoSequenceWithNameAndStop:(NSString *)aSequenceName
{
	NSUInteger f = [self startFrameForSequenceWithName:aSequenceName];
	return (f != NSNotFound) ? [self gotoFrameAndStop:f] : NO;
}

- (BOOL)gotoSequenceWithNameAndPlay:(NSString *)aSequenceName looped:(BOOL)isLooped
{
	NSUInteger f = [self startFrameForSequenceWithName:aSequenceName];
	if (f != NSNotFound)
    {
        self.isLooped = isLooped;
        [self gotoFrameAndPlay:f];
    }
    return NO;
}

- (BOOL)gotoFrameAndStop:(NSUInteger)aFrameNumber
{
	if ([self setFrame:aFrameNumber])
	{
		self.isRunning = NO;
		return YES;
	}
	return NO;
}

- (BOOL)gotoFrameAndPlay:(NSUInteger)aFrameNumber
{
	if ([self setFrame:aFrameNumber])
	{
		self.isRunning = YES;
		return YES;
	}
	return NO;
}

- (NSUInteger)startFrameForSequenceWithName:(NSString *)aSequenceName
{
	if (self.asset == nil)
	{
		return NSNotFound;
	}
	GAFAnimationSequence *seq = [self.asset animationSequenceForName:aSequenceName];
	return (seq != nil) ? seq.framesRange.location : NSNotFound;
}

- (NSUInteger)endFrameForSequenceWithName:(NSString *)aSequenceName
{
	if (self.asset == nil)
	{
		return NSNotFound;
	}
	GAFAnimationSequence *seq = [self.asset animationSequenceForName:aSequenceName];
	return (seq != nil) ? (seq.framesRange.location + seq.framesRange.length) : NSNotFound;
}

- (BOOL)playSequenceWithName:(NSString *)aSeqName
                      looped:(BOOL)isLooped
                      resume:(BOOL)aResume
                        hint:(AnimSetSequenceHint)aHint;
{
	if (self.asset == nil)
	{
		return NO;
	}
	
	if (aSeqName == nil || [aSeqName length] == 0)
	{
		return NO;
	}
	NSUInteger s = [self startFrameForSequenceWithName:aSeqName];
	NSUInteger e = [self endFrameForSequenceWithName:aSeqName];
	
	if (s == NSNotFound || e == NSNotFound)
	{
		return NO;
	}
	self.currentSequenceStart = s;
	self.currentSequenceEnd = e;
	
	if (self.currentFrameIndex < self.currentSequenceStart ||
        self.currentFrameIndex > self.currentSequenceEnd)
	{
		self.currentFrameIndex = self.currentSequenceStart;
	}
	else
	{
		if (aHint == ASSH_RESTART)
		{
			self.currentFrameIndex = self.currentSequenceStart;
		}
		else
		{
			// new hints may appear
		}
	}
	self.isLooped = isLooped;
	if (aResume)
	{
		[self resume];
	}
	else
	{
		[self stop];
	}
	return YES;
}

- (BOOL)playSequenceWithName:(NSString *)aSeqName looped:(BOOL)isLooped resume:(BOOL)aResume
{
	return [self playSequenceWithName:aSeqName looped:isLooped resume:aResume hint:ASSH_RESTART];
}

- (BOOL)playSequenceWithName:(NSString *)aSeqName looped:(BOOL)isLooped
{
	return [self playSequenceWithName:aSeqName looped:isLooped resume:YES hint:ASSH_RESTART];
}

- (BOOL)playSequenceWithName:(NSString *)aSeqName
{
	return [self playSequenceWithName:aSeqName looped:NO];
}

- (void)clearSequence
{
	self.currentSequenceStart = GAF_FIRST_FRAME_INDEX;
	self.currentSequenceEnd = self.totalFrameCount - 1;
}

#pragma mark -
#pragma mark Private methods

- (void)initialize
{
    if (!self.isInitialized)
    {
        [self addSubObjectsUsingAnimationObjectsDictionary:self.asset.objects
                                  animationMasksDictionary:self.asset.masks];
        
        self.isInitialized = YES;
        
        self.currentFrameIndex = GAF_FIRST_FRAME_INDEX;
    }
}

- (GAFSprite *)subObjectForInnerObjectId:(NSString *)anInnerObjectId
{
    NSSet *keys = [self.subObjects keysOfEntriesPassingTest:^BOOL (id key, id obj, BOOL *stop)
                   {
                       return (*stop = [((GAFSprite *) obj).objectId isEqual:anInnerObjectId]);
                   }];
    
    if (keys != nil && [keys count] > 0)
    {
        return (self.subObjects)[[keys anyObject]];
    }
    return nil;
}

- (NSString *)objectIdByObjectName:(NSString *)aName
{
    if (aName == nil)
	{
		return nil;
	}
    
    __block NSString *result = nil;
    [self.asset.namedParts enumerateKeysAndObjectsUsingBlock:
     ^(id key, id obj, BOOL *stop)
    {
         if ([(NSString *)obj isEqual:aName])
         {
             result = (NSString *)key;
             *stop = YES;
         }
    }];
	return result;
}

- (int)numberOfGlobalFramesForOneAnimationFrame
{
    float globalFPS = roundf(1.0 / [[CCDirector sharedDirector] animationInterval]);
    
    if (globalFPS > (float)self.FPSType - FLT_EPSILON)
    {
        return (int)roundf(globalFPS / (float)self.FPSType);
    }
    else
    {
        return 1;
    }
}

- (void)processAnimations:(ccTime)dt
{
    if (++self.extraFramesCounter >= [self numberOfGlobalFramesForOneAnimationFrame])
    {
        self.extraFramesCounter = 0;
		if (!self.isDone && self.isRunning)
		{
			[self step];
			
			if (framePlayedDelegate != nil)
			{
				[framePlayedDelegate onFramePlayed:self didPlayFrameNo:[self currentFrameIndex]];
			}
		}
    }
}

- (void)step
{
    if (self.isLooped)
    {
        if (self.currentFrameIndex > self.currentSequenceEnd)
        {
            self.currentFrameIndex = self.currentSequenceStart;
        }
    }
    else
    {
        if (self.currentFrameIndex > self.currentSequenceEnd)
        {
            self.isRunning = NO;
            return;
        }
    }
    
    [self processAnimation];
    
	if (self.sequenceDelegate != nil && self.asset != nil)
	{
		GAFAnimationSequence *seq = [asset animationSequenceByLastFrame:self.currentFrameIndex];
		if (seq != nil)
		{
			[self.sequenceDelegate onFinishSequence:self sequenceName:seq.name];
		}
	}
    
    ++self.currentFrameIndex;
}

- (void)processCurrentAnimationFrameOnceMore
{
    // Make one processAnimation to restore old objects state (frameIndex is increased in the end
    // of [self step] so we decrease it and restore)
    NSUInteger oldFrameIndex = self.currentFrameIndex;
    if (self.currentFrameIndex != 0)
        self.currentFrameIndex --;
    
    [self processAnimation];
    
    self.currentFrameIndex = oldFrameIndex;
}

- (void)processAnimation
{
    GAFAnimationFrame *currentFrame = (self.asset.animationFrames)[self.currentFrameIndex];
    
    // Make all object invisible
    [self.subObjects enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
        ((GAFSprite *)obj).visible = NO;
    }];
    
    // Set states for objects and make mentioned objects visible
    BOOL zOrderChanged = NO;
    for (NSUInteger i = 0; i < currentFrame.objectsStates.count; ++i)
    {
        GAFSubobjectState *state = (currentFrame.objectsStates)[i];
        GAFSpriteWithAlpha *subObject = (self.subObjects)[state.objectId];
        if (subObject != nil)
        {
            // Validate sprite type (w/ or w/o filter)
            GAFBlurFilterData *blurFilter = (state.filters)[kGAFBlurFilterName];
            
            CGPoint prevAP = subObject.anchorPoint;
            CGSize prevCS = subObject.contentSize;
            
            if (blurFilter != nil)
            {
                // TODO: don't use 9 sample kernel constant, make it variable and computable (4 = floor(9 / 2))
                subObject.blurRadius = CGSizeMake(blurFilter.blurSize.width / 4, blurFilter.blurSize.height / 4);
            }
            else
            {
                subObject.blurRadius = CGSizeMake(0, 0);
            }
            
            // Handle initial object in one position
            CGSize newCS = subObject.contentSize;
            CGPoint newAP = CGPointMake( ((prevAP.x - 0.5) * prevCS.width) / newCS.width + 0.5,
                                        ((prevAP.y - 0.5) * prevCS.height) / newCS.height + 0.5);
            subObject.anchorPoint = newAP;
            
            // Determine if object is masked or not
            // Depending on that: add to hierarchy OR add to mask
            if (state.maskObjectId == nil)
            {
                if (subObject.parent == nil)
                {
                    [self addChild:subObject];
                }
            }
            else
            {
                if (subObject.parent != nil)
                {
                    [self removeChild:subObject cleanup:NO];
                }
                GAFStencilMaskSprite *mask = self.masks[state.maskObjectId];
                if (mask != nil)
                {
                    [mask addMaskedObject:subObject];
                }
            }
            
            // Determine is subobject is captured
            BOOL subobjectCaptured = NO;
            GAFAnimatedObjectControlFlags controlFlags = kGAFAnimatedObjectControl_None;
            NSNumber *flagsNum = self.capturedObjects[state.objectId];
            if (flagsNum)
            {
                subobjectCaptured = YES;
                controlFlags = [flagsNum unsignedIntegerValue];
            }
            
            if (!subobjectCaptured ||
                (subobjectCaptured && (controlFlags & kGAFAnimatedObjectControl_ApplyState)))
            {
                // Update object position and alpha
                // Applying csf adjustments
                CGAffineTransform stateTransform = state.affineTransform;
                stateTransform.tx *= self.asset.usedAtlasContentScaleFactor;
                stateTransform.ty *= self.asset.usedAtlasContentScaleFactor;
                
                subObject.externalTransform = GAF_CGAffineTransformCocosFormatFromFlashFormat(state.affineTransform);
                if (subObject.zOrder != state.zIndex)
                {
                    zOrderChanged |= YES;
                    subObject.zOrder = state.zIndex;
                    
                    if (subObject.maskContainer != nil)
                    {
                        [subObject.maskContainer invalidateMaskedObjectsOrder];
                    }
                }
                
                BOOL forceHide = [self.hiddenSubobjectIds containsObject:subObject.objectId];
                
                subObject.visible = !forceHide && [state isVisible];
                [subObject setColorTransformMult:[state colorMults] offsets:[state colorOffsets]];
            }
        }
        else
        {
            GAFSprite *mask = (self.masks)[state.objectId];
            if (mask != nil)
            {
                mask.externalTransform = GAF_CGAffineTransformCocosFormatFromFlashFormat(state.affineTransform);
                
                if (mask.zOrder != state.zIndex)
                {
                    zOrderChanged |= YES;
                    mask.zOrder = state.zIndex;
                    
                    if (subObject.maskContainer != nil)
                    {
                        [subObject.maskContainer invalidateMaskedObjectsOrder];
                    }
                }
            }
        }
    }
    
    // Notify control delegate about captured subobjects
    for (GAFSubobjectState *state in currentFrame.objectsStates)
    {
        GAFSpriteWithAlpha *subObject = (self.subObjects)[state.objectId];
        if (subObject != nil)
        {
            // Determine is subobject is captured
            BOOL subobjectCaptured = (self.capturedObjects[state.objectId] != nil);
            
            // If captured, notify delegate about new frame rendering
            if (subobjectCaptured && self.controlDelegate != nil)
            {
                [self.controlDelegate animatedObject:self didDisplayFrameWithSubobject:subObject];
            }
        }
        else
        {
            // Masks cannot be captured right now
        }
    }
}

@end

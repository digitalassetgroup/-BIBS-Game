#import "GAFAnimatedObject.h"
#import "GAFSprite.h"
#import "GAFTextureAtlas.h"
#import "GAFTextureAtlasElement.h"
#import "GAFNode.h"
#import "GAFSprite.h"
#import "GAFAsset.h"
#import "GAFSpriteWithAlpha.h"
#import "GAFBlurredSprite.h"
#import "GAFAnimationFrame.h"
#import "GAFSubobjectState.h"
#import "GAFStencilMaskSprite.h"
#import "GAFAnimationSequence.h"
#import "GAFFilterData.h"
#import "CCSpriteFrame.h"
#import "NSString+Extensions.h"
#import "ccMacros.h"
#import "CCDirector.h"


static CGAffineTransform CGAffineTransformCocosFormatFromFlashFormat (CGAffineTransform aTransform)
{
    CGAffineTransform transform = aTransform;
    transform.b = - transform.b;
    transform.c = - transform.c;
    transform.ty = - transform.ty;
    return transform;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

static float const kAnimationFPS = 30.0;   // we keep this number 'almost' synchronized with web. The reason it's
// not 31 fps is that we will have some animation artifacts when running
// on 30/60 fps device.


@interface GAFAnimatedObject ()


- (GAFSprite *)subObjectForInnerObjectId:(NSString *)anInnerObjectId;

@end

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

+ (GAFAnimatedObject *) createWithAsset:(GAFAsset*) anAsset
{
	GAFAnimatedObject * res = [self alloc];
	if (res != nil && [res initWithAsset:anAsset])
	{
		return [res autorelease];
	}
	[res release];
	return nil;
}

+ (GAFAnimatedObject *) createWithPath:(NSString *)jsonPath
{
	GAFAsset * asset = [GAFAsset create:jsonPath];
	return [asset createObject];
}

+ (GAFAnimatedObject *) createAndRun:(NSString *)jsonPath looped:(BOOL) looped
{
	GAFAsset * asset = [GAFAsset create:jsonPath];
	return [asset createObjectAndRun:looped];
}

- (void)dealloc
{
    [asset release];
    [subObjects release];
    [masks release];
    [super dealloc];
}

#pragma mark -
#pragma mark Overriden methods

#pragma mark -
#pragma mark Public methods

/// Analyzes animation frames and generates structure such as:
/// { MaskId -> [ ObjID1, ObjID2, ObjID3 ] }
/// Optionally returns all masked objects
- (NSDictionary *)masksAssociationStructureFromAnimationObjectIds:(NSArray *)anAnimationObjectIds
                                                           frames:(NSArray *)anAnimationFrames
                                                  maskedObjectIds:(NSArray **)aMaskedObjectIds;
{
    NSMutableDictionary *maskedObjectsStructure = [NSMutableDictionary dictionary];
    
    if (aMaskedObjectIds != nil)
    {
        *aMaskedObjectIds = [[NSMutableArray alloc] init];
    }
    
    NSMutableArray *animationObjectIds = [anAnimationObjectIds mutableCopy];
    for (GAFAnimationFrame *frame in anAnimationFrames)
    {
        for (GAFSubobjectState *state in frame.objectsStates)
        {
            if (state.maskObjectId != nil && ![state.maskObjectId isEmpty])
            {
                NSMutableArray *maskSubObjects = [maskedObjectsStructure objectForKey:state.maskObjectId];
                if (maskSubObjects == nil)
                {
                    maskSubObjects = [NSMutableArray array];
                    [maskedObjectsStructure setObject:maskSubObjects forKey:state.maskObjectId];
                }
                
                if (![maskSubObjects containsObject:state.objectId])
                {
                    [maskSubObjects addObject:state.objectId];
                }
                
                if (aMaskedObjectIds != nil)
                {
                    if (![(NSMutableArray *)*aMaskedObjectIds containsObject:state.objectId])
                    {
                        [(NSMutableArray *)*aMaskedObjectIds addObject:state.objectId];
                    }
                }
                [animationObjectIds removeObjectIdenticalTo:state.objectId];
            }
        }
        
        if ([animationObjectIds count] == 0)
            break;
    }
    [animationObjectIds release];
    
    return maskedObjectsStructure;
}

- (void)addSubObjectsUsingAnimationObjectsDictionary:(NSDictionary *)anAnimationObjects
                            animationMasksDictionary:(NSDictionary *)anAnimationMasks
                                     animationFrames:(NSArray *)anAnimationFrames

{
    NSArray *maskedObjectIds = nil;
    NSDictionary *maskedObjectsStructure = [self masksAssociationStructureFromAnimationObjectIds:[anAnimationObjects allKeys]
                                                                                          frames:anAnimationFrames
                                                                                 maskedObjectIds:&maskedObjectIds];
    
    // Adding sub objects
    for (NSString *objectId in anAnimationObjects)
    {
        NSString *atlasElementId = [anAnimationObjects objectForKey:objectId];
        CCSpriteFrame *spriteFrame = nil;
        
        GAFTextureAtlasElement *element = [self.asset.textureAtlas.elements objectForKey:atlasElementId];
        if (nil != element)
        {
            if ([self.asset.textureAtlas.textures count] >= element.atlasIdx + 1)
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
        
        if (spriteFrame != nil)
        {
            GAFSpriteWithAlpha *sprite = [[GAFSpriteWithAlpha alloc] initWithSpriteFrame:spriteFrame];
            
            sprite.objectId = atlasElementId;
            
            sprite.visible = NO;
            sprite.anchorPoint = CGPointMake(element.pivotPoint.x / sprite.contentSize.width,
                                             1 - (element.pivotPoint.y / sprite.contentSize.height));
            sprite.useExternalTransform = YES;
            
            sprite.blendFunc = (ccBlendFunc){ GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
            
            // Add to hierarchy
            [self.subObjects setObject:sprite forKey:objectId];
            // Check if the object is masked (don't add it to visual hierarchy then)
            if (![maskedObjectIds containsObject:objectId])
            {
                [self addChild:sprite];
            }
            
            [sprite release];
        }
        else
        {
            CCLOGWARN(@"Cannot add subnode with AtlasElementName: %@, not found in atlas(es). Ignoring.",
                            atlasElementId);
        }
    }
    
    // Adding masks
    for (NSString *maskId in maskedObjectsStructure)
    {
        NSString *atlasElementId = [anAnimationMasks objectForKey:maskId];
        CCSpriteFrame *spriteFrame = nil;
        
        GAFTextureAtlasElement *element = [self.asset.textureAtlas.elements objectForKey:atlasElementId];
        if (nil != element)
        {
            spriteFrame = [CCSpriteFrame frameWithTexture:self.asset.textureAtlas.texture rect:element.bounds];
        }
		else
		{
			CCLOGWARN(@"Can not get atlasElementId for key %@. Animation can not work as expected.", maskId);
		}
        
        if (spriteFrame != nil)
        {
            GAFStencilMaskSprite *mask = [[GAFStencilMaskSprite alloc] initWithSpriteFrame:spriteFrame];
            
            mask.anchorPoint = CGPointMake(element.pivotPoint.x / mask.contentSize.width,
                                           1 - (element.pivotPoint.y / mask.contentSize.height));
            mask.useExternalTransform = YES;
            
            // Add to masks
            [self.masks setObject:mask forKey:maskId];
            [self addChild:mask];
            
            // Adding masked objects to mask
            for (NSString *objectId in [maskedObjectsStructure objectForKey:maskId])
            {
                id object = [self.subObjects objectForKey:objectId];
                if (object != nil)
                {
                    [mask addMaskedObject:object];
                }
                else
                {
                    CCLOGWARN(@"Object with id:%@ is not in 'animationObjects' list in config json.", objectId);
                }
            }

            [mask release];
        }
    }	
	[maskedObjectIds release];
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

#pragma mark -
#pragma mark <PCAnimatable>

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

- (GAFSprite *) getNamedSubobject:(NSString *)name
{
	if (name == nil)
	{
		return nil;
	}
	
	NSString * rawName = nil;
	
	for (NSString * str in self.asset.namedParts)
	{
		if ([name isEqualToString:[self.asset.namedParts objectForKey:str]])
		{
			rawName = str;
			break;
		}
	}
	if (rawName == nil)
	{
		return nil;
	}
	return [self getSubobjectByRawName:rawName];
}

- (GAFSprite *) getSubobjectByRawName:(NSString *) name
{
	return (GAFSprite*) [self.subObjects objectForKey:name];
}

#pragma mark -
#pragma mark Private methods

- (GAFSprite *)subObjectForInnerObjectId:(NSString *)anInnerObjectId
{
    NSSet *keys = [self.subObjects keysOfEntriesPassingTest:^BOOL (id key, id obj, BOOL *stop)
    {
        return (*stop = [((GAFSprite *) obj).objectId isEqualToString:anInnerObjectId]);
    }];

    if (keys != nil && [keys count] > 0)
    {
        return [self.subObjects objectForKey:[keys anyObject]];
    }
    return nil;
}

#pragma mark -
#pragma mark <PCAnimatorDelegate>


- (BOOL)initWithAsset:(GAFAsset *)assetData
{
	if (self = [super init])
	{
		if (assetData == nil)
		{
			return NO;
		}
		self.asset = assetData;
		self.subObjects = [NSMutableDictionary dictionary];
		self.masks = [NSMutableDictionary dictionary];
		_animationsSelectorScheduled = NO;
		self.FPSType = PCAnimationFPSType_30;
		self.extraFramesCounter = 0;
		self.currentSequenceStart = self.currentFrameIndex = FIRST_FRAME_INDEX;
		self.currentSequenceEnd   = self.totalFrameCount = [self.asset.animationFrames count];
		self.isRunning = NO;
		return YES;
	}
	else
	{
		return NO;
	}
}

#pragma mark -
#pragma mark Public methods

- (void)start
{
    if (!self.isRunning)
    {
        self.currentFrameIndex = FIRST_FRAME_INDEX;
        self.isRunning = YES;
    }
	[self removeAllSubObjects];
    
    [self addSubObjectsUsingAnimationObjectsDictionary:self.asset.objects
                                      animationMasksDictionary:self.asset.masks
                                               animationFrames:self.asset.animationFrames];
	
	[self schedule:@selector(processAnimations:)];
	_animationsSelectorScheduled = YES;
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

/// Stops animation and sets the currentFrame pointer to the first frame
- (void)stop
{
    if (self.isRunning)
    {
        self.currentFrameIndex = FIRST_FRAME_INDEX;
        self.isRunning = NO;
    }
	[self unschedule:@selector(processAnimations:)];
	_animationsSelectorScheduled = NO;
}

#pragma mark -
#pragma mark Protected methods

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

- (void)step
{
	if (self.sequenceDelegate != nil && self.asset != nil)
	{
		GAFAnimationSequence * seq = [asset getSequenceByEndFrame:self.currentFrameIndex];
		if (seq != nil)
		{
			[self.sequenceDelegate onFinishSequence:self sequenceName:seq.name];
		}
	}
    if (self.isLooped)
    {
        if (self.currentFrameIndex >= self.currentSequenceEnd)
        {
            self.currentFrameIndex = self.currentSequenceStart;
        }
    }
    else
    {
        if (self.currentFrameIndex >= self.currentSequenceEnd)
        {
            self.isRunning = NO;
            return;
        }
    }
    
    [self processAnimation];
    
    ++self.currentFrameIndex;
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

- (BOOL)gotoAndStop:(NSString *) frameLabel
{
	int f = [self getStartFrame:frameLabel];
	if (-1 == f)
	{
		return YES;
	}
	return [self gotoFrameAndStop:f];
	
}

- (BOOL)gotoFrameAndStop:(NSUInteger) frameNumber
{
	if ([self setFrame:frameNumber])
	{
		self.isRunning = NO;
		return YES;
	}
	return NO;
	
}

- (BOOL)gotoAndPlay:(NSString *) frameLabel
{
	int f = [self getStartFrame:frameLabel];
	if (-1 == f)
	{
		return NO;
	}
	return [self gotoAndPlay:f];
}

- (BOOL)gotoFrameAndPlay:(NSUInteger) frameNumber
{
	if ([self setFrame:frameNumber])
	{
		self.isRunning = YES;
		return YES;
	}
	return NO;
}

- (NSUInteger) getStartFrame:(NSString *) frameLabel
{
	if (self.asset == nil)
	{
		return -1;
	}
	GAFAnimationSequence * seq = [self.asset getSequence:frameLabel];
	if (seq != nil)
	{
		return seq.framesRange.location;
	}
	return -1;
}

- (NSUInteger) getEndFrame:(NSString *) frameLabel
{
	if (self.asset == nil)
	{
		return -1;
	}
	GAFAnimationSequence * seq = [self.asset getSequence:frameLabel];
	if (seq != nil)
	{
		return seq.framesRange.location + seq.framesRange.length;
	}
	return -1;
}

- (BOOL) setSequenceWithName:(NSString *)name looped:(BOOL)looped resume:(BOOL)resume hint:(AnimSetSequenceHint)hint;
{
	if (self.asset == nil)
	{
		return NO;
	}
	
	if (name == nil || [name length] == 0)
	{
		return NO;
	}
	int s = [self getStartFrame:name];
	int e = [self getEndFrame:name];
	
	if (-1 == s || -1 == e)
	{
		return false;
	}
	self.currentSequenceStart = s;
	self.currentSequenceEnd = e;
	
	if (self.currentFrameIndex < self.currentSequenceStart || self.currentFrameIndex > self.currentSequenceEnd)
	{
		self.currentFrameIndex = self.currentSequenceStart;
	}
	else
	{
		if (hint == ASSH_RESTART)
		{
			self.currentFrameIndex = self.currentSequenceStart;
		}
		else
		{
			// new hints may appear
		}
	}
	self.isLooped = looped;
	if (resume)
	{
		[self resume];
	}
	else
	{
		[self stop];
	}
	return YES;
}

- (BOOL) setSequenceWithName:(NSString *)name
{
	return [self setSequenceWithName:name looped:NO];
}

- (BOOL) setSequenceWithName:(NSString *)name looped:(BOOL)looped
{
	return [self setSequenceWithName:name looped:looped resume:YES hint:ASSH_RESTART];
}

- (BOOL) setSequenceWithName:(NSString *)name looped:(BOOL)looped resume:(BOOL)resume
{
	return [self setSequenceWithName:name looped:looped resume:resume hint:ASSH_RESTART];
}

- (void) clearSequence
{
	self.currentSequenceStart = FIRST_FRAME_INDEX;
	self.currentSequenceEnd = self.totalFrameCount;
}

- (void)processAnimation
{
    GAFAnimationFrame *currentFrame = [self.asset.animationFrames objectAtIndex:self.currentFrameIndex];
    
    // Make all object invisible
    [self.subObjects enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
        ((GAFSprite *)obj).visible = NO;
    }];
    
    // Set states for objects and make mentioned objects visible
    BOOL zOrderChanged = NO;
    for (NSUInteger i = 0; i < currentFrame.objectsStates.count; ++i)
    {
        GAFSubobjectState *state = [currentFrame.objectsStates objectAtIndex:i];
        GAFSpriteWithAlpha *subObject = [self.subObjects objectForKey:state.objectId];
        if (subObject != nil)
        {
            subObject.visible = YES;
            
            // Validate sprite type (w/ or w/o filter)
            GAFBlurFilterData *blurFilter = [state.filters objectForKey:kBlurFilterName];
            if (blurFilter == nil && [subObject isKindOfClass:[GAFBlurredSprite class]])
            {
                CCLOGWARN(@"Converted filtered sprite with ID: %@ to simple.", state.objectId);
                // revert sprite back to simple one
                GAFBlurredSprite *filteredSprite = (GAFBlurredSprite *)subObject;
                
                [self removeChild:filteredSprite cleanup:NO];
                subObject = filteredSprite.sourceSprite;
                [self addChild:subObject];
                
                [self.subObjects setObject:filteredSprite.sourceSprite forKey:state.objectId];
            }
            else if (blurFilter != nil)
            {
                if (![subObject isKindOfClass:[GAFBlurredSprite class]])
                {
                    CCLOGWARN(@"Converted simple sprite with ID: %@ to filtered.", state.objectId);
                    // convert sprite to filtered one
                    GAFBlurredSprite *filteredSprite = [[GAFBlurredSprite alloc] initWithGAFSprite:subObject
																						  blurSize:blurFilter.blurSize];
                    [filteredSprite updateDynamicTexture];
                    
                    [self.subObjects setObject:filteredSprite forKey:state.objectId];
                    [filteredSprite release];
                    
                    [self removeChild:subObject cleanup:NO];
                    subObject = filteredSprite;
                    [self addChild:filteredSprite];
                }
                else
                {
                    // Check if filter parameters match
                    GAFBlurredSprite *filteredSprite = (GAFBlurredSprite *)subObject;
                    CGSize roundedBlurSize = CGSizeMake(ceilf(blurFilter.blurSize.width),
                                                        ceilf(blurFilter.blurSize.height));
                    CGSize normalizedBlurSize = [filteredSprite normalizedBlurSizeFromNeededBlurSize:roundedBlurSize];
                    if (!CGSizeEqualToSize(filteredSprite.blurSize, normalizedBlurSize))
                    {
                        CCLOGWARN(@"Modifying filtered sprite ID: %@ parameters.", state.objectId);
                        
                        filteredSprite.blurSize = roundedBlurSize;
                        [filteredSprite updateDynamicTexture];
                    }
                }
            }
            
            // Update object position and alpha
            subObject.externalTransform = CGAffineTransformCocosFormatFromFlashFormat(state.affineTransform);
            if (subObject.zOrder != state.zIndex)
            {
                zOrderChanged |= YES;
                subObject.zOrder = state.zIndex;
                
                if (subObject.maskContainer != nil)
                {
                    [subObject.maskContainer invalidateMaskedObjectsOrder];
                }
            }
            
            // Set alpha
            subObject.visible = [state isVisible];
            [subObject setColorTransformMult:[state colorMults] offsets:[state colorOffsets]];
        }
        else
        {
            GAFSprite *mask = [self.masks objectForKey:state.objectId];
            if (mask != nil)
            {
                mask.externalTransform = CGAffineTransformCocosFormatFromFlashFormat(state.affineTransform);
            }
            
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


@end

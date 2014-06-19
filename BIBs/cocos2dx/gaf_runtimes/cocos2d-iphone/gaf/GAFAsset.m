#import "GAFAsset.h"
#import "GAFTextureAtlas.h"
#import "GAFActionObject.h"
#import "GAFSubobjectState.h"
#import "GAFAnimationFrame.h"
#import "GAFAnimationSequence.h"
#import "GAFAnimatedObject.h"
#import "NSString+Extensions.h"

#import "GAFSprite.h"
#import "ccMacros.h"
#import "CCFileUtils.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

static NSString * const kAnimationFrameCountKey = @"animationFrameCount";
static NSString * const kAnimationConfigFramesKey = @"animationConfigFrames";
static NSString * const kAnimationObjectsKey = @"animationObjects";
static NSString * const kAnimationMasksKey = @"animationMasks";
static NSString * const kAnimationNamedPartsKey = @"namedParts";
static NSString * const kInteractionObjectsKey = @"interactionObject";
static NSString * const kStandObjectsKey = @"standObjects";
static NSString * const kTextureAtlasKey = @"textureAtlas";
static NSString * const kAnimationSequencesKey = @"animationSequences";
static NSString * const kVersionKey = @"version";

static NSString * const kFrameNumberKey = @"frameNumber";
static NSString * const kFrameStateKey = @"state";

static NSString * const kPCAnimationSequenceIdKey = @"id";
static NSString * const kPCAnimationSequenceStartFrameNoKey = @"startFrameNo";
static NSString * const kPCAnimationSequenceEndFrameNoKey = @"endFrameNo";

NSString * const  kAtlasScaleKey = @"scale";
NSString * const  kAtlasInfoKey  = @"atlases";

NSString * const kAssetRootObjectName = @"ICO";
NSString * const kAssetWindowMaskName = @"windowMask";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFAsset ()

@property (nonatomic, assign) NSUInteger majorVersion;
@property (nonatomic, assign) NSUInteger minorVersion;

@property (nonatomic, retain) GAFTextureAtlas *textureAtlas;

@property (nonatomic, retain) NSDictionary *objects;
@property (nonatomic, retain) NSDictionary *masks;

@property (nonatomic, retain) NSDictionary *namedParts;


@property (nonatomic, retain) NSArray *interactionObjects;
@property (nonatomic, retain) NSArray *standObjects;

@property (nonatomic, retain) NSArray *animationFrames;
@property (nonatomic, retain) NSDictionary *animationSequences;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

static float _currentAtlasScale       = 0.0f;
static float _currentDeviceScale      = 1.0f;
static bool  _makeExactScaleForObject = false;
static float _objectExactScaleRatio   = 0.0f;

@implementation GAFAsset

#pragma mark -
#pragma mark Properties

+ (BOOL)isAssetVersionPlayable:(NSString *)version
{
	// at this moment we do not have legacy animations, in future we may need to do something
	return YES;
}

+ (void) setScalingParameters:(float)currentDeviceScale  makeExactScaleForObject:(bool)makeExactScaleForObject
{
	_currentDeviceScale      = currentDeviceScale;
	_makeExactScaleForObject = makeExactScaleForObject;
}

#pragma mark -
#pragma mark Initialization & Release

+ (GAFAsset*) create:(NSString*) jsonPath
{
	GAFAsset * res = [[GAFAsset alloc] init];
	if (res != nil && [res initWithConfigData:jsonPath])
	{		
		return [res autorelease];
	}
	[res release];

	return nil;
}

- (GAFAnimatedObject*) createObject
{
	if (self.textureAtlas == nil)
	{
		return nil;
	}
	return [GAFAnimatedObject createWithAsset:self];
}

- (GAFAnimatedObject*) createObjectAndRun:(BOOL)looped
{
	GAFAnimatedObject * res = [self createObject];
	if (res != nil)
	{
		res.isLooped = looped;
		[res start];
	}
	return res;
}

- (GAFAnimationSequence*) getSequence:(NSString*)name
{
	if (self.animationSequences == nil || name == nil || ![name length])
	{
		return nil;
	}
	return (GAFAnimationSequence*)[_animationSequences objectForKey:name];
}

- (GAFAnimationSequence*) getSequenceByEndFrame:(NSUInteger)frame
{
	if (self.animationSequences == nil)
	{
		return nil;
	}
	for (NSString* key in self.animationSequences)
	{
		GAFAnimationSequence * seq = [self.animationSequences objectForKey:key];
		if (frame == (seq.framesRange.location + seq.framesRange.length))
		{
			return seq;
		}
	}
	return nil;
}

- (float) getAtlasSclae:(NSDictionary*)atlas
{
	if (!atlas)
	{
		return 0;
	}
	NSNumber * scale = (NSNumber * )[atlas objectForKey:kAtlasScaleKey];
	return [scale floatValue];
}

- (BOOL) initWithConfigData:(NSString *)jsonPath
{
	NSString * fp = [[CCFileUtils sharedFileUtils] fullPathForFilename:jsonPath];	
	NSData * aConfigData = [NSData dataWithContentsOfFile:fp];
	
	if (aConfigData == nil || aConfigData.length == 0)
	{
		CCLOGWARN(@"Can not get data from json file : %@.", jsonPath);
		return nil;
	}
	
    NSAssert(aConfigData != nil, @"aConfig data should not be nil");
    
	NSError *error = nil;
	id configDataObject = [NSJSONSerialization JSONObjectWithData:aConfigData options:(NSJSONReadingOptions)0 error:&error];
	if (configDataObject == nil && error != nil && [configDataObject isKindOfClass:[NSDictionary class]])
	{
		CCLOGWARN(@"Error while creating PCAnimationData. ConfigData cannot be parsed.");
		return NO;
	}
	
	NSDictionary *configDictionary = (NSDictionary *)configDataObject;

	
	NSArray *animationConfigFrames      = configDictionary[kAnimationConfigFramesKey];
	NSArray *interactionObjectNodes     = configDictionary[kInteractionObjectsKey];
	NSArray *standObjectsNodes          = configDictionary[kStandObjectsKey];
	NSArray *textureAtlasNode           = configDictionary[kTextureAtlasKey];
	NSArray *animationSequences         = configDictionary[kAnimationSequencesKey];
	
	NSDictionary *objectNodes           = configDictionary[kAnimationObjectsKey];
	NSDictionary *masksNodes            = configDictionary[kAnimationMasksKey];
	NSDictionary *namedPartsNodes       = configDictionary[kAnimationNamedPartsKey];

	NSString *versionNode               = configDictionary[kVersionKey];
	
	if ([[self class] isAssetVersionPlayable:versionNode])
	{
		if (animationConfigFrames == nil || textureAtlasNode == nil || objectNodes == nil)
		{
			CCLOGWARN(@"Error while creating GAFAsset. Required subnodes in dictionary are missing.");
		    return NO;
		}
		
		NSDictionary * atlasDictionary = (NSDictionary *)[textureAtlasNode objectAtIndex:0];
		float atlasScale = [self getAtlasSclae:atlasDictionary];
		for (int i = 1; i < [textureAtlasNode count]; ++i)
		{
			NSDictionary * a = (NSDictionary *) [textureAtlasNode objectAtIndex:i];
			float as = [self getAtlasSclae:a];
			if ( fabs(atlasScale - _currentDeviceScale) > fabs(as - _currentDeviceScale))
			{
				atlasDictionary = a;
				atlasScale = as;
			}
		}
		_currentAtlasScale = atlasScale;
		NSArray * atlasesInfo = (NSArray *)[atlasDictionary objectForKey:kAtlasInfoKey];
		if (!atlasesInfo)
		{
			CCLOGWARN(@"Error while creating GAFAsset.atlasesInfo subnode is missing in atlasDictionary.");
			return false;
		}
			
		GAFTextureAtlas * atlasData = [[GAFTextureAtlas alloc] initWithJsonPath:fp configDictionary:atlasDictionary imagesData:atlasesInfo];
		self.textureAtlas = atlasData;
		[atlasData release];
		if (self.textureAtlas == nil)
		{
			CCLOGWARN(@"Failed to initialize PCAnimationData. GAFTextureAtlas could not be created.");
			return NO;
		}

		// Animation objects & masks
		// Nodes are accurately the same as we need [ObjectId] -> [AtlasTextureId]
		self.objects = objectNodes;
		self.masks = masksNodes;
		self.namedParts = namedPartsNodes;
		
		// Interaction objects
		if (interactionObjectNodes != nil)
		{
			@autoreleasepool
			{
				NSMutableArray *mInteractionObjects = [NSMutableArray arrayWithCapacity:[interactionObjectNodes count]];
				for (NSDictionary *interObjectNode in interactionObjectNodes)
				{
					GAFActionObject *interObject = [[GAFActionObject alloc] initWithDictionary:interObjectNode];
					[mInteractionObjects addObject:interObject];
					[interObject release];
				}
				self.interactionObjects = mInteractionObjects;
			}
		}
		
		// Stand objects
		if (standObjectsNodes != nil)
		{
			@autoreleasepool
			{
				NSMutableArray *mStandObjects = [NSMutableArray arrayWithCapacity:[standObjectsNodes count]];
				for (NSDictionary *standObjectNode in standObjectsNodes)
				{
					GAFActionObject *standObject = [[GAFActionObject alloc] initWithDictionary:standObjectNode];
					[mStandObjects addObject:standObject];
					[standObject release];
				}
				self.standObjects = mStandObjects;
			}
		}
		
		// Frames
		[self loadFramesFromConfigDictionary:configDictionary];
		
		// Sequences
		if (animationSequences != nil)
		{
			[self loadAnimationSequences:animationSequences];
		}
		
		// Version
		if (versionNode == nil && [versionNode isEmpty])
		{
			self.majorVersion = self.minorVersion = NSNotFound;
		}
		else
		{
			NSArray *comps = [versionNode componentsSeparatedByString:@"."];
			if ([comps count] == 1)
			{
				self.majorVersion = [[comps objectAtIndex:0] integerValue];
				self.minorVersion = 0;
			}
			else if ([comps count] == 2)
			{
				self.majorVersion = [[comps objectAtIndex:0] integerValue];
				self.minorVersion = [[comps objectAtIndex:1] integerValue];
			}
			else
			{
				self.majorVersion = self.minorVersion = NSNotFound;
			}
		}
	}
	else
	{
		return NO;
	}
	return YES;
}

- (void)dealloc
{
    [_standObjects release];
    [_interactionObjects release];
    [_objects release];
    [_masks release];
    [_animationFrames release];
    [_animationSequences release];
    [_textureAtlas release];
	[_namedParts release];
    [super dealloc];
}

#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

- (void)loadFramesFromConfigDictionary:(NSDictionary *)aConfigDictionary
{
    NSAssert(_objects != nil, @"self.objects != nil");
    
    @autoreleasepool
    {
        NSMutableDictionary *currentStates = [[[NSMutableDictionary alloc] initWithCapacity:[_objects count]] autorelease];
        NSMutableArray *mAnimationFrames = [NSMutableArray array];
        
        NSArray *animationConfigFrames = [aConfigDictionary objectForKey:@"animationConfigFrames"];
        
        // Create default states for all objects in self.animationObjects
        for (NSString *key in _objects)
        {
            GAFSubobjectState *state = [[GAFSubobjectState alloc] initWithObjectId:key];
            [currentStates setObject:state forKey:key];
            [state release];
        }        
        // Processing frames
        NSUInteger configFrameCount = (NSUInteger)[[aConfigDictionary objectForKey:kAnimationFrameCountKey] integerValue];
        NSUInteger configFrameIndex = 0;
        for (NSUInteger index = 0; index < configFrameCount; ++index)
        {
            if (configFrameIndex < [animationConfigFrames count])
            {
                NSDictionary *configFrame = [animationConfigFrames objectAtIndex:configFrameIndex];
                NSInteger configFrameNo = [[configFrame objectForKey:kFrameNumberKey] integerValue];
                
                if (configFrameNo == index)
                {
                    // Update current object states
                    NSArray *newStates = [self objectStatesFromConfigFrame:configFrame];
                    
                    for (GAFSubobjectState *state in newStates)
                    {
                        [currentStates setObject:state forKey:state.objectId];
                    }
                    
                    configFrameNo ++;
                    configFrameIndex ++;					
                }
            }
            
            // Add frame
            GAFAnimationFrame *frame = [[GAFAnimationFrame alloc] init];
            
            frame.objectsStates = [[[currentStates allValues] copy] autorelease];
            
            [mAnimationFrames addObject:frame];
            [frame release];
        }        
        self.animationFrames = mAnimationFrames;
    }
}

- (void)loadAnimationSequences:(NSArray *)aSequencesNodes
{
    static NSString * const kPCAnimationSequenceIdKey = @"id";
    static NSString * const kPCAnimationSequenceStartFrameNoKey = @"startFrameNo";
    static NSString * const kPCAnimationSequenceEndFrameNoKey = @"endFrameNo";
    
    @autoreleasepool
    {
        NSMutableDictionary *mutSequences = [[[NSMutableDictionary alloc] initWithCapacity:aSequencesNodes.count] autorelease];
        for (NSDictionary *sequenceNode in aSequencesNodes)
        {
            NSString *sequenceId = sequenceNode[kPCAnimationSequenceIdKey];
            NSNumber *startFrameNo = sequenceNode[kPCAnimationSequenceStartFrameNoKey];
            NSNumber *endFrameNo = sequenceNode[kPCAnimationSequenceEndFrameNoKey];
            
            if (sequenceId != nil && startFrameNo != nil && endFrameNo != nil)
            {
                GAFAnimationSequence *sequence =
                [[GAFAnimationSequence alloc] initWithName:sequenceId
                                                   framesRange:NSMakeRange([startFrameNo integerValue],
                                                                           [endFrameNo integerValue] - [startFrameNo integerValue])];
                
                mutSequences[sequenceId] = sequence;
				[sequence release];
            }
            else
            {
                CCLOGWARN(@"Error while creating PCAnimationData. ConfigData cannot be parsed.");
            }
        }
        
        self.animationSequences = mutSequences;
    }
}

- (NSArray *)objectStatesFromConfigFrame:(NSDictionary *)configFrame
{
	if (configFrame == nil)
	{
		return nil;
	}
    NSDictionary *stateNodes = (NSDictionary*)[configFrame objectForKey:kFrameStateKey];
    NSMutableArray *states = [NSMutableArray arrayWithCapacity:[stateNodes count]];
    
    for (NSString *key in stateNodes)
    {
        GAFSubobjectState *state = [[GAFSubobjectState alloc] initWithStateDictionary:[stateNodes objectForKey:key] objectId:key];
        if (state != nil)
        {
            [states addObject:state];
            [state release];
        }
        else
        {
            CCLOGWARN(@"GAFSubobjectState cannot be created. Ignoring.");
        }
    }
    return states;
}

@end
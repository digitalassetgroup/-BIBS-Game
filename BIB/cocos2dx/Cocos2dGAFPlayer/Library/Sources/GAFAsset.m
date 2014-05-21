////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFAsset.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "GAFAsset.h"
#import "GAFTextureAtlas.h"
#import "GAFSubobjectState.h"
#import "GAFAnimationFrame.h"
#import "GAFAnimationSequence.h"
#import "GAFAnimatedObject.h"
#import "NSString+GAFExtensions.h"
#import "GAFAssetExtendedDataObject.h"

#import "GAFSprite.h"
#import "Support/CCFileUtils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

static NSString * const kAnimationFrameCountKey = @"animationFrameCount";
static NSString * const kAnimationConfigFramesKey = @"animationConfigFrames";
static NSString * const kAnimationObjectsKey = @"animationObjects";
static NSString * const kAnimationMasksKey = @"animationMasks";
static NSString * const kAnimationNamedPartsKey = @"namedParts";
static NSString * const kTextureAtlasKey = @"textureAtlas";
static NSString * const kAnimationSequencesKey = @"animationSequences";
static NSString * const kVersionKey = @"version";

static NSString * const kFrameNumberKey = @"frameNumber";
static NSString * const kFrameStateKey = @"state";

static NSString * const kPCAnimationSequenceIdKey = @"id";
static NSString * const kPCAnimationSequenceStartFrameNoKey = @"startFrameNo";
static NSString * const kPCAnimationSequenceEndFrameNoKey = @"endFrameNo";

static NSString * const kAtlasScaleKey = @"scale";
static NSString * const kAtlasInfoKey  = @"atlases";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFAsset ()

@property (nonatomic, assign) NSUInteger majorVersion;
@property (nonatomic, assign) NSUInteger minorVersion;

@property (nonatomic, retain) GAFTextureAtlas *textureAtlas;
@property (nonatomic, assign) CGFloat usedAtlasContentScaleFactor;

@property (nonatomic, retain) NSDictionary *objects;
@property (nonatomic, retain) NSDictionary *masks;

@property (nonatomic, retain) NSDictionary *namedParts;

@property (nonatomic, retain) NSMutableDictionary *extendedDataObjectGroups;

@property (nonatomic, retain) NSArray *animationFrames;
@property (nonatomic, retain) NSDictionary *animationSequences;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFAsset

#pragma mark -
#pragma mark Properties

+ (BOOL)isAssetVersionPlayable:(NSString *)version
{
	// at this moment we do not have legacy animations, in future we may need to do something
	return YES;
}

#pragma mark -
#pragma mark Initialization & Release

- (id)initWithJSONAtPath:(NSString *)aJSONPath keepImagesInAtlas:(BOOL)aKeepImagesInAtlas
{
    return [self initWithJSONAtPath:aJSONPath extendedDataObjectClasses:nil keepImagesInAtlas:aKeepImagesInAtlas];
}

- (id)initWithJSONAtPath:(NSString *)aJSONPath
extendedDataObjectClasses:(NSDictionary *)anExtendedDataObjectClasses
       keepImagesInAtlas:(BOOL)aKeepImagesInAtlas
{
    NSAssert(aJSONPath != nil || [aJSONPath length] == 0, @"ERROR: initializing JSON. aJSONPath data should not be nil");
	if (aJSONPath == nil || [aJSONPath length] == 0)
	{
		CCLOGWARN(@"ERROR: initializing JSON. Empty json path");
        [self ah_release];
		return nil;
	}
    
    // Retrieve JSON data from filePath
	NSString *filePath = [[CCFileUtils sharedFileUtils] fullPathForFilenameIgnoringResolutions:aJSONPath];
    NSData *JSONData = [NSData dataWithContentsOfFile:filePath];
    if (JSONData == nil)
    {
        CCLOGWARN(@"ERROR: initializing JSON. Cannot load JSON file at path: %@", aJSONPath);
        [self ah_release];
        return nil;
    }
    
    return [self initWithJSONData:JSONData
            atlasesDataDictionary:nil
            orAtlasTexturesFolder:[filePath stringByDeletingLastPathComponent]
        extendedDataObjectClasses:anExtendedDataObjectClasses
                keepImagesInAtlas:aKeepImagesInAtlas];
}

- (id)initWithJSONData:(NSData *)aJSONData
 atlasesDataDictionary:(NSDictionary *)anAtlasesDataDictionary
 orAtlasTexturesFolder:(NSString *)anAtlasTexturesFolder
     keepImagesInAtlas:(BOOL)aKeepImagesInAtlas
{
    return [self initWithJSONData:aJSONData
            atlasesDataDictionary:anAtlasesDataDictionary
            orAtlasTexturesFolder:anAtlasTexturesFolder
        extendedDataObjectClasses:nil
                keepImagesInAtlas:aKeepImagesInAtlas];
}

- (id)initWithJSONData:(NSData *)aJSONData
 atlasesDataDictionary:(NSDictionary *)anAtlasesDataDictionary
 orAtlasTexturesFolder:(NSString *)anAtlasTexturesFolder
extendedDataObjectClasses:(NSDictionary *)anExtendedDataObjectClasses
     keepImagesInAtlas:(BOOL)aKeepImagesInAtlas
{
	self = [super init];
	
    NSAssert(aJSONData != nil, @"parameters should not be nil");
    if (aJSONData == nil || [aJSONData length] == 0)
    {
		CCLOGWARN(@"ERROR: initializing JSON. JSONData and/or atlasDictionary are empty");
        [self ah_release];
		return nil;
	}
    
	NSError *error = nil;
	id configDataObject = [NSJSONSerialization JSONObjectWithData:aJSONData options:(NSJSONReadingOptions)0 error:&error];
	if (configDataObject == nil && error != nil && [configDataObject isKindOfClass:[NSDictionary class]])
	{
		CCLOGWARN(@"ERROR: initializing JSON. ConfigData cannot be parsed.");
        [self ah_release];
		return nil;
	}
	
	NSDictionary *configDictionary = (NSDictionary *)configDataObject;
    
	NSArray *animationConfigFrames  = configDictionary[kAnimationConfigFramesKey];
	NSArray *textureAtlasNode       = configDictionary[kTextureAtlasKey];
	NSArray *animationSequences     = configDictionary[kAnimationSequencesKey];
	
	NSDictionary *objectNodes       = configDictionary[kAnimationObjectsKey];
	NSDictionary *masksNodes        = configDictionary[kAnimationMasksKey];
	NSDictionary *namedPartsNodes   = configDictionary[kAnimationNamedPartsKey];
    
	NSString *versionNode           = configDictionary[kVersionKey];

    if (animationConfigFrames == nil)
    {
        CCLOGWARN(@"ERROR: initializing GAFAsset. 'animationFrameCount' not present");
        [self ah_release];
        return nil;
    }
    
    if (textureAtlasNode == nil || ![textureAtlasNode count])
    {
        CCLOGWARN(@"ERROR: initializing GAFAsset. 'textureAtlas' not present");
        [self ah_release];
        return nil;
    }
    /*
    if (animationSequences == nil)
    {
        CCLOGWARN(@"ERROR: initializing GAFAsset. 'animationSequences' not present");
        [self release];
        return nil;
    }
     */
    
    if (objectNodes == nil)
    {
        CCLOGWARN(@"ERROR: initializing GAFAsset. 'animationObjects' not present");
        [self ah_release];
        return nil;
    }
	
	if (![[self class] isAssetVersionPlayable:versionNode])
	{
        CCLOGWARN(@"ERROR: initializing GAFAsset. Source config version is not supported.");
        [self ah_release];
        return nil;
    }
    
    // Determining best available atlas content scale factor compatible with current device
    float currentDeviceScale = CC_CONTENT_SCALE_FACTOR();
    
    NSDictionary *atlasDictionary = textureAtlasNode[0];
    CGFloat atlasScale = 1.0;
    for (int i = 0; i < [textureAtlasNode count]; ++i)
    {
        NSDictionary *newAtlasDictionary = textureAtlasNode[i];
        float newAtlasScale = [self atlasScaleFromAtlasConfig:atlasDictionary];
        
        if (newAtlasScale > 0)
        {
            if (fabs(atlasScale - currentDeviceScale) > fabs(newAtlasScale - currentDeviceScale))
            {
                atlasDictionary = newAtlasDictionary;
                atlasScale = newAtlasScale;
            }
        }
    }
    self.usedAtlasContentScaleFactor = atlasScale;
    
    NSArray *atlasesInfo = (NSArray *)atlasDictionary[kAtlasInfoKey];
    if (!atlasesInfo)
    {
        CCLOGWARN(@"Error while creating GAFAsset. 'atlases' subnode is missing.");
        [self ah_release];
        return nil;
    }
    
    GAFTextureAtlas *atlasData = nil;
    if (anAtlasesDataDictionary != nil)
    {
        atlasData = [[GAFTextureAtlas alloc] initWithTextureAtlasesDictionary:anAtlasesDataDictionary
                                                 textureAtlasConfigDictionary:atlasDictionary
                                                            keepImagesInAtlas:aKeepImagesInAtlas];
    }
    else if (anAtlasTexturesFolder != nil && anAtlasTexturesFolder.length > 0)
    {
        atlasData = [[GAFTextureAtlas alloc] initWithTexturesDirectory:anAtlasTexturesFolder
                                          textureAtlasConfigDictionary:atlasDictionary
                                                     keepImagesInAtlas:aKeepImagesInAtlas];
    }
    
    if (atlasData == nil)
    {
        CCLOGWARN(@"Failed to initialize PCAnimationData. GAFTextureAtlas could not be created.");
        [self ah_release];
        return nil;
    }
    
    self.textureAtlas = atlasData;
    [atlasData ah_release];
    
    // Animation objects & masks
    // Nodes are accurately the same as we need [ObjectId] -> [AtlasTextureId]
    self.objects = objectNodes;
    self.masks = masksNodes;
    self.namedParts = namedPartsNodes;
    
    // Extended objects
    if (anExtendedDataObjectClasses != nil)
    {
        @autoreleasepool
        {
            for (NSString *groupName in anExtendedDataObjectClasses)
            {
                Class objectClass = anExtendedDataObjectClasses[groupName];
                NSArray *jsonObjects = configDictionary[groupName];
                
                if (jsonObjects != nil && jsonObjects.count > 0 &&
                    [objectClass isSubclassOfClass:[GAFAssetExtendedDataObject class]])
                {
                    NSMutableArray *parsedObjects = [[NSMutableArray alloc] initWithCapacity:jsonObjects.count];
                    for (NSDictionary *jsonObject in jsonObjects)
                    {
                        GAFAssetExtendedDataObject *newObject = [[objectClass alloc] initWithDictionary:jsonObject];
                        if (newObject != nil)
                        {
                            [parsedObjects addObject:newObject];
                            [newObject ah_release];
                        }
                    }
                    
                    // Save objects
                    if (self.extendedDataObjectGroups == nil)
                    {
                        self.extendedDataObjectGroups = [[[NSMutableDictionary alloc] init] ah_autorelease];
                    }
                    self.extendedDataObjectGroups[groupName] = parsedObjects;
                    [parsedObjects ah_release];
                }
            }
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
            self.majorVersion = [comps[0] integerValue];
            self.minorVersion = 0;
        }
        else if ([comps count] == 2)
        {
            self.majorVersion = [comps[0] integerValue];
            self.minorVersion = [comps[1] integerValue];
        }
        else
        {
            self.majorVersion = self.minorVersion = NSNotFound;
        }
    }
	return self;
}


- (void)dealloc
{
    [_extendedDataObjectGroups ah_release];
    [_objects ah_release];
    [_masks ah_release];
    [_animationFrames ah_release];
    [_animationSequences ah_release];
    [_textureAtlas ah_release];
	[_namedParts ah_release];

#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Overriden methods

- (NSString *)description
{
    return [NSString stringWithFormat:@"< %@ - 0x%x | version - %lu.%lu | objects - %lu | masks - %lu | frames - %lu >",
            NSStringFromClass(self.class), (unsigned int)self,
            (unsigned long)self.majorVersion, (unsigned long)self.minorVersion,
            (unsigned long)self.objects.count, (unsigned long)self.masks.count, (unsigned long)self.animationFrames.count];
}

#pragma mark -
#pragma mark Public methods

- (NSArray *)animationSequenceNames
{
    return [self.animationSequences allKeys];
}

- (GAFAnimationSequence *)animationSequenceForName:(NSString *)aName
{
	if (self.animationSequences == nil || aName == nil || ![aName length])
	{
		return nil;
	}
	return (GAFAnimationSequence *)_animationSequences[aName];
}

- (GAFAnimationSequence *)animationSequenceByLastFrame:(NSUInteger)aLastFrame
{
	if (self.animationSequences == nil)
	{
		return nil;
	}
	for (NSString *key in self.animationSequences)
	{
		GAFAnimationSequence *seq = (self.animationSequences)[key];
		if (aLastFrame == (seq.framesRange.location + seq.framesRange.length))
		{
			return seq;
		}
	}
	return nil;
}

#pragma mark -
#pragma mark Private methods

- (void)loadFramesFromConfigDictionary:(NSDictionary *)aConfigDictionary
{
    NSAssert(_objects != nil, @"self.objects != nil");
    if (_objects == nil)
    {
        CCLOGWARN(@"ERROR: in loadFramesFromConfigDictionary. self.objects property should be initialized.");
        return;
    }
    
    @autoreleasepool
    {
        NSMutableDictionary *currentStates = [[[NSMutableDictionary alloc] initWithCapacity:[_objects count]] ah_autorelease];
        NSMutableArray *mAnimationFrames = [NSMutableArray array];
        
        NSArray *animationConfigFrames = aConfigDictionary[@"animationConfigFrames"];
        
        // Create default states for all objects in self.animationObjects
        for (NSString *key in _objects)
        {
            GAFSubobjectState *state = [[GAFSubobjectState alloc] initEmptyStateWithObjectId:key];
            currentStates[key] = state;
            [state ah_release];
        }
        // Processing frames
        NSUInteger configFrameCount = (NSUInteger)[aConfigDictionary[kAnimationFrameCountKey] integerValue];
        if (configFrameCount == 0)
        {
            CCLOGWARN(@"ERROR: initializing GAFAsset.  configFrameCount is 0 or pmpty");
            return;
        }
        NSUInteger configFrameIndex = 0;
        for (NSUInteger index = 0; index < configFrameCount; ++index)
        {
            if (configFrameIndex < [animationConfigFrames count])
            {
                NSDictionary *configFrame = animationConfigFrames[configFrameIndex];
                NSInteger configFrameNo = [configFrame[kFrameNumberKey] integerValue];
                
                if (configFrameNo - 1 == index) // -1 because frame numbers start with 1
                {
                    // Update current object states
                    NSArray *newStates = [self objectStatesFromConfigFrame:configFrame];
                    
                    for (GAFSubobjectState *state in newStates)
                    {
                        currentStates[state.objectId] = state;
                    }
                    
                    configFrameNo ++;
                    configFrameIndex ++;
                }
            }
            
            // Add frame
            GAFAnimationFrame *frame = [[GAFAnimationFrame alloc] init];
            
            frame.objectsStates = [[[currentStates allValues] copy] ah_autorelease];
            
            [mAnimationFrames addObject:frame];
            [frame ah_release];
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
        NSMutableDictionary *mutSequences = [[[NSMutableDictionary alloc] initWithCapacity:aSequencesNodes.count] ah_autorelease];
        for (NSDictionary *sequenceNode in aSequencesNodes)
        {
            NSString *sequenceId = sequenceNode[kPCAnimationSequenceIdKey];
            NSNumber *startFrameNo = sequenceNode[kPCAnimationSequenceStartFrameNoKey];
            NSNumber *endFrameNo = sequenceNode[kPCAnimationSequenceEndFrameNoKey];
            
            if (sequenceId != nil && startFrameNo != nil && endFrameNo != nil)
            {
                GAFAnimationSequence *sequence =
                [[GAFAnimationSequence alloc] initWithName:sequenceId
                                                   framesRange:NSMakeRange([startFrameNo integerValue] - 1,
                                                                           [endFrameNo integerValue] - [startFrameNo integerValue])];
                
                mutSequences[sequenceId] = sequence;
				[sequence ah_release];
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
    NSDictionary *stateNodes = (NSDictionary*)configFrame[kFrameStateKey];
    NSMutableArray *states = [NSMutableArray arrayWithCapacity:[stateNodes count]];
    
    for (NSString *key in stateNodes)
    {
        GAFSubobjectState *state = [[GAFSubobjectState alloc] initWithStateDictionary:stateNodes[key] objectId:key];
        if (state != nil)
        {
            [states addObject:state];
            [state ah_release];
        }
        else
        {
            CCLOGWARN(@"GAFSubobjectState cannot be created. Ignoring.");
        }
    }
    return states;
}

- (float)atlasScaleFromAtlasConfig:(NSDictionary *)anAtlasConfigDictionary
{
	if (anAtlasConfigDictionary == nil)
	{
		return 0;
	}
	NSNumber *scale = (NSNumber *)anAtlasConfigDictionary[kAtlasScaleKey];
    return (scale != nil) ? [scale floatValue] : 0;
}

@end
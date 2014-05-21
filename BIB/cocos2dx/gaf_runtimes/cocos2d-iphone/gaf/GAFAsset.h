
@class GAFTextureAtlas;
@class GAFAnimatedObject;
@class GAFAnimationSequence;


extern NSString * const kAssetRootObjectName;
extern NSString * const kAssetWindowMaskName;


@interface GAFAsset : NSObject

#pragma mark Parsed data

@property (nonatomic, assign, readonly) NSUInteger majorVersion;
@property (nonatomic, assign, readonly) NSUInteger minorVersion;

@property (nonatomic, retain, readonly) GAFTextureAtlas *textureAtlas;

@property (nonatomic, retain, readonly) NSDictionary *objects;  // dictionary of objects [ObjectId -> AtlasElementName]
@property (nonatomic, retain, readonly) NSDictionary *masks;    // dictionary of masks [MaskId -> AtlasElementName]

@property (nonatomic, retain, readonly) NSDictionary *namedParts;    // dictionary of masks [MaskId -> AtlasElementName]


/// List of GAFActionObject-s representing zones to interact with
@property (nonatomic, retain, readonly) NSArray *interactionObjects;
/// List of GAFActionObject-s representing zones on which objects can be put
@property (nonatomic, retain, readonly) NSArray *standObjects;

#pragma mark Animation related data

@property (nonatomic, retain, readonly) NSArray *animationFrames;   ///< List of GAFAnimationFrame objects
@property (nonatomic, retain, readonly) NSDictionary *animationSequences;   ///< List of GAFAnimationSequences objects


#pragma mark Methods
+ (void) setScalingParameters:(float)currentDeviceScale  makeExactScaleForObject:(bool)makeExactScaleForObject;
+ (BOOL)isAssetVersionPlayable:(NSString *)version;

// this function return object that is already autoreleased
+ (GAFAsset*) create:(NSString*) jsonPath;

- (GAFAnimatedObject*) createObject;
- (GAFAnimatedObject*) createObjectAndRun:(BOOL)looped;
- (GAFAnimatedObject*) createObjectAndRunWithAnimationName:(BOOL)looped animation_name:(NSString*) animation_name;
- (GAFAnimationSequence*) getSequence:(NSString*)name;
- (GAFAnimationSequence*) getSequenceByEndFrame:(NSUInteger)frame;


@end

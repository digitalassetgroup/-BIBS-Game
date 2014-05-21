#import "CCLayer.h"

@class GAFAsset;
@class GAFObjectAnimation;
@class GAFSprite;
@class GAFAnimatedObject;

typedef enum
{
    PCAnimationFPSType_15 = 15,
    PCAnimationFPSType_30 = 30,
    PCAnimationFPSType_60 = 60
} PCAnimationFPSType;

typedef enum
{
	ASSH_CONTINUE,
	ASSH_RESTART
} AnimSetSequenceHint;

#define FIRST_FRAME_INDEX 1

@protocol GAFFramePlayedDelegate <NSObject>
@required
- (void)onFramePlayed:(GAFAnimatedObject *)anObject didPlayFrameNo:(NSUInteger)aFrameNo;
@end

@protocol GAFSequenceDelegate <NSObject>
@required
- (void)onFinishSequence:(GAFAnimatedObject *)anObject sequenceName:(NSString*)sequenceName;
@end

/// Graphic object which consists of set of different graphic objects and single texture atlas.
@interface GAFAnimatedObject : CCLayer
{
@private
    BOOL _animationsSelectorScheduled;
}
@property (nonatomic, retain) GAFAsset *asset;
@property (nonatomic, retain) NSMutableDictionary *subObjects;
@property (nonatomic, retain) NSMutableDictionary *masks;           ///< stores all masks (not displayed)
@property (nonatomic, assign) NSUInteger currentFrameIndex;
@property (nonatomic, assign) NSUInteger totalFrameCount;
@property (nonatomic, assign) NSUInteger currentSequenceStart;
@property (nonatomic, assign) NSUInteger currentSequenceEnd;
@property (nonatomic, assign) PCAnimationFPSType FPSType;
@property (nonatomic, assign) NSUInteger extraFramesCounter;


@property (nonatomic, assign) BOOL isRunning;
@property (nonatomic, assign) BOOL isLooped;

// do not forgt clear delegates if you use them
@property (nonatomic, assign) id <GAFFramePlayedDelegate> framePlayedDelegate;
@property (nonatomic, assign) id <GAFSequenceDelegate>       sequenceDelegate;


- (BOOL)initWithAsset:(GAFAsset *)asset;

/// Creates subobjects from [ObjectID]->[AtlasSegmentID] pairs
- (void)addSubObjectsUsingAnimationObjectsDictionary:(NSDictionary *)anAnimationObjects
                            animationMasksDictionary:(NSDictionary *)anAnimationMasks
                                     animationFrames:(NSArray *)anAnimationFrames;

- (void)removeAllSubObjects;


+ (GAFAnimatedObject *) createWithAsset:(GAFAsset*) anAsset;
+ (GAFAnimatedObject *) createWithPath:(NSString *)jsonPath;
+ (GAFAnimatedObject *) createAndRun:(NSString *)jsonPath looped:(BOOL) looped;

- (GAFSprite *) getNamedSubobject:(NSString *)name;
- (GAFSprite *) getSubobjectByRawName:(NSString *) name;

/// @returns YES if the animation is finished, otherwise NO
- (BOOL)isDone;

- (void)start;
- (void)pause;
- (void)resume;
/// Stops animation and sets the currentFrame pointer to the first frame
- (void)stop;
- (BOOL)setFrame:(NSUInteger)aFrameIndex;

- (BOOL)gotoAndStop:(NSString *) frameLabel;
- (BOOL)gotoFrameAndStop:(NSUInteger) frameNumber;
- (BOOL)gotoAndPlay:(NSString *) frameLabel;
- (BOOL)gotoFrameAndPlay:(NSUInteger) frameNumber;
- (NSUInteger) getStartFrame:(NSString *)frameLabel;
- (NSUInteger) getEndFrame:(NSString *) frameLabel;
- (BOOL) setSequenceWithName:(NSString *)name looped:(BOOL)looped resume:(BOOL)resume hint:(AnimSetSequenceHint)hint;
- (BOOL) setSequenceWithName:(NSString *)name looped:(BOOL)looped resume:(BOOL)resume;
- (BOOL) setSequenceWithName:(NSString *)name looped:(BOOL)looped;
- (BOOL) setSequenceWithName:(NSString *)name;
- (void) clearSequence;

@end
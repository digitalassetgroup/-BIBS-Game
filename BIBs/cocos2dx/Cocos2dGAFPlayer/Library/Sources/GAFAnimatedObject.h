////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFAnimatedObject.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "CCLayer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@class GAFAsset;
@class GAFObjectAnimation;
@class GAFSprite;
@class GAFAnimatedObject;
@class GAFTextureAtlas;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum : NSUInteger
{
    kGAFAnimationFPSType_15 = 15,
    kGAFAnimationFPSType_30 = 30,
    kGAFAnimationFPSType_60 = 60
} GAFAnimationFPSType;

typedef enum
{
	ASSH_CONTINUE,
	ASSH_RESTART
} AnimSetSequenceHint;

enum
{
    kGAFAnimatedObjectControl_None = 0,
    // If specified, state of controlled object will be changed every frame (like it is by default) and then
    // animatedObject:didDisplayFrameWithSubobject: will be called
    kGAFAnimatedObjectControl_ApplyState = 1 << 0
};
typedef NSUInteger GAFAnimatedObjectControlFlags;

#define GAF_FIRST_FRAME_INDEX 0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@protocol GAFFramePlayedDelegate <NSObject>

@required
- (void)onFramePlayed:(GAFAnimatedObject *)anObject didPlayFrameNo:(NSUInteger)aFrameNo;

@end

@protocol GAFSequenceDelegate <NSObject>

@required
- (void)onFinishSequence:(GAFAnimatedObject *)anObject sequenceName:(NSString *)aSequenceName;

@end

@protocol GAFAnimatedObjectControlDelegate <NSObject>

@required
- (void)animatedObject:(GAFAnimatedObject *)anObject didDisplayFrameWithSubobject:(GAFSprite *)aSubobject;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
@property (nonatomic, assign) GAFAnimationFPSType FPSType;
@property (nonatomic, assign) NSUInteger extraFramesCounter;
@property (nonatomic, retain) NSMutableDictionary *externalNamedPartsTextureAtlases;  ///< Value is of GAFTextureAtlas class
@property (nonatomic, retain) NSMutableArray *hiddenSubobjectIds;

@property (nonatomic, assign, readonly) BOOL isInitialized;
@property (nonatomic, assign, readonly) BOOL isRunning;
@property (nonatomic, assign) BOOL isLooped;

// do not forget clear delegates if you use them
@property (nonatomic, assign) id <GAFFramePlayedDelegate> framePlayedDelegate;
@property (nonatomic, assign) id <GAFSequenceDelegate>    sequenceDelegate;
@property (nonatomic, assign) id <GAFAnimatedObjectControlDelegate> controlDelegate;


+ (id)animatedObjectWithAsset:(GAFAsset *)anAsset;
+ (id)animatedObjectWithPath:(NSString *)aJSONPath;
+ (id)animatedObjectWithPath:(NSString *)aJSONPath runLooped:(BOOL)aLooped;

/// Designated initializer
- (id)initWithAsset:(GAFAsset *)anAsset;

- (id)initWithPath:(NSString *)aJSONPath;
- (id)initWithPath:(NSString *)aJSONPath runLooped:(BOOL)aLooped;

#pragma mark Dimensions

/// Merges all visible subobjects rects and returns result
- (CGRect)realBoundingBoxForCurrentFrame;

#pragma mark Working with subobjects and external objects

/// Returns subobject which has specified name assinged to it ("namedParts" section in config)
- (GAFSprite *)subobjectByName:(NSString *)aName;
/// Returns subobject by raw name (usually "Zn", where n is natural number (1,2,3,...))
- (GAFSprite *)subobjectByRawName:(NSString *)aRawName;

/// Hides or show subobject which has specified name assinged to it ("namedParts" section in config)
- (BOOL)isSubobjectHiddenWithName:(NSString *)aName;
- (void)setSubobjectHidden:(BOOL)isHidden withName:(NSString *)aName;

/// Uses objects from atlas by linking their IDs to IDs specified in 'namedParts' config section
/// @note after execution of this method, aTextureAtlas will have only textures, images will be released
/// @param aTextureAtlas new texture atlas
/// @param anOldTextureAtlas old external texture atlas that should be replaced by a new one (can be nil)
- (void)linkExternalNamedPartsAtlas:(GAFTextureAtlas *)aTextureAtlas forName:(NSString *)anAtlasName;
- (void)removeLinkedAtlasForName:(NSString *)anAtlasName;

/// Takes control over subobject, which means that every frame control delegate will be notified to decide
/// what to do with captured external object
/// @note it supports only objects for now, DOES NOT SUPPORT MASKS
/// @param subobject name taken from "namedParts" section in config
/// @param controlFlags flags specifying what played will do with subobjects controlled externally
/// @returns YES if control was successfully taken and all future 
- (BOOL)captureControlOverSubobjectNamed:(NSString *)aName controlFlags:(GAFAnimatedObjectControlFlags)aControlFlags;
/// Releases control over subobject captured earlier
- (void)releaseControlOverSubobjectNamed:(NSString *)aName;

#pragma mark Managing animation playback

/// @returns YES if the animation is finished, otherwise NO
- (BOOL)isDone;

/// Initializes animation for first use, won't run animation
- (void)initialize;

- (void)start;
- (void)pause;
- (void)resume;
/// Stops animation and sets the currentFrame pointer to the first frame
- (void)stop;
/// Sets currentFrame and applies specified frame state to animation objects
- (BOOL)setFrame:(NSUInteger)aFrameIndex;

/// Plays only first frame of specified animation and then stops
- (BOOL)gotoSequenceWithNameAndStop:(NSString *)aSequenceName;
/// Plays animation with specified name
- (BOOL)gotoSequenceWithNameAndPlay:(NSString *)aSequenceName looped:(BOOL)isLooped;

/// Plays specified frame and then stops
- (BOOL)gotoFrameAndStop:(NSUInteger)aFrameNumber;
/// Plays animation from specified frame
- (BOOL)gotoFrameAndPlay:(NSUInteger)aFrameNumber;

#pragma mark Animation sequences

- (NSUInteger)startFrameForSequenceWithName:(NSString *)aSequenceName;
- (NSUInteger)endFrameForSequenceWithName:(NSString *)aSequenceName;

/// Plays animation sequence with specified name
/// @param aSeqName a sequence name
/// @param isLooped if YES - sequence should play in cycle
/// @param aResume if YES - animation will be played immediately, if NO - playback will be paused after the first frame is shown
/// @param aHint specific animation playback parameters
- (BOOL)playSequenceWithName:(NSString *)aSeqName looped:(BOOL)isLooped resume:(BOOL)aResume hint:(AnimSetSequenceHint)aHint;
- (BOOL)playSequenceWithName:(NSString *)aSeqName looped:(BOOL)isLooped resume:(BOOL)aResume;
- (BOOL)playSequenceWithName:(NSString *)aSeqName looped:(BOOL)isLooped;
- (BOOL)playSequenceWithName:(NSString *)aSeqName;
- (void)clearSequence;

@end
@interface GAFAnimationSequence : NSObject

@property (nonatomic, copy  , readonly) NSString *name;
@property (nonatomic, assign, readonly) NSRange framesRange;

- (id)initWithName:(NSString *)aName framesRange:(NSRange)aFramesRange;

@end

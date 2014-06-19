#import "CCNode.h"

@interface GAFNode : CCNode

// By setting this parameter all other state values except 'anchorPoint' will be ignored
@property (nonatomic, assign) BOOL useExternalTransform;
@property (nonatomic, assign) CGAffineTransform externalTransform;

@end

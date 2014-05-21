extern NSString * const kBlurFilterName;

@interface GAFFilterData : NSObject

@end

@interface GAFBlurFilterData : GAFFilterData

@property (nonatomic, assign) CGSize blurSize;

@end
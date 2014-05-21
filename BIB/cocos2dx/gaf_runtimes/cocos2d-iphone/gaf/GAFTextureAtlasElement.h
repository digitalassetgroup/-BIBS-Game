@interface GAFTextureAtlasElement : NSObject

@property (nonatomic,   copy) NSString * name;
@property (nonatomic, assign) CGPoint    pivotPoint;
@property (nonatomic, assign) CGRect     bounds;
@property (nonatomic, assign) NSUInteger atlasIdx;

- (id)initWithDictionary:(NSDictionary *)aDictionary;

@end

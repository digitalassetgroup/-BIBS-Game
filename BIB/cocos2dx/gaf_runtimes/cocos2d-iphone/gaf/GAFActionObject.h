@class GAFTextureAtlas;
@class GAFTextureAtlasElement;

@interface GAFActionObject : NSObject

@property (nonatomic,   copy) NSString *name;
@property (nonatomic, assign) CGPoint pivotPoint;
@property (nonatomic, assign) CGRect bounds;

- (id)initWithDictionary:(NSDictionary *)aDictionary;

@end

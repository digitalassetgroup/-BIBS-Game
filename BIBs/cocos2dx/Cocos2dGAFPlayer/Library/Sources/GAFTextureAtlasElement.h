////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFTextureAtlasElement.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface GAFTextureAtlasElement : NSObject

@property (nonatomic,   copy) NSString * name;
@property (nonatomic, assign) CGPoint    pivotPoint;
@property (nonatomic, assign) CGRect     bounds;
@property (nonatomic, assign) NSUInteger atlasIdx;
@property (nonatomic, assign) float      scale;

// DI
- (id)init;
- (id)initWithDictionary:(NSDictionary *)aDictionary;

@end

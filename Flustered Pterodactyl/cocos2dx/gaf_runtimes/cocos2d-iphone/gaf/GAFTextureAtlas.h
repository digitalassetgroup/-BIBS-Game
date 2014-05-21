@class CCTexture2D;

@interface GAFTextureAtlas : NSObject

@property (nonatomic, assign) BOOL loaded;
//@property (nonatomic, assign) CGSize actualSize;

@property (nonatomic, retain) UIImage *image;   // returns or sets the first image in .images array
@property (nonatomic, retain, readonly) CCTexture2D *texture;   // returns the first image in .textures array

@property (nonatomic, retain, readonly) NSMutableArray *images;
@property (nonatomic, retain, readonly) NSMutableArray *textures;  ///< CCTexture2D textures

@property (nonatomic, retain) NSDictionary *elements;

- (id)initWithJsonPath:(NSString*)jsonPath configDictionary:(NSDictionary *)aConfigDictionary imagesData:(NSArray *)anImageDatas;

@end
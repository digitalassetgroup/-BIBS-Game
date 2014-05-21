////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFTextureAtlas.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@class CCTexture2D;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface GAFTextureAtlas : NSObject

@property (nonatomic, assign) BOOL loaded;

//@property (nonatomic, retain) UIImage *image;   // returns or sets the first image in .images array
//@property (nonatomic, retain, readonly) CCTexture2D *texture;   // returns the first image in .textures array

@property (nonatomic, retain, readonly) NSMutableArray *images;     ///< UIImage images
@property (nonatomic, retain, readonly) NSMutableArray *textures;   ///< CCTexture2D textures

@property (nonatomic, retain) NSDictionary *elements;

/// Loads texture atlas from config dictionary and textures NSData
/// @param aTextureAtlasesDictionary dictionary with texture data with Key->Value pairs [AtlasName -> AtlasData]
/// @param aTextureAtlasConfigDictionary part of JSON asset config representing texture atlases metadata
- (id)initWithTextureAtlasesDictionary:(NSDictionary *)aTextureAtlasesDictionary
          textureAtlasConfigDictionary:(NSDictionary *)aTextureAtlasConfigDictionary
                     keepImagesInAtlas:(BOOL)aKeepImagesInAtlas;

/// Loads texture atlas from config dictionary. Loads textures from file system knowing texture directory location.
/// @param aTexturesDirectory directory where all texture atlases should be located
/// @param aTextureAtlasConfigDictionary part of JSON asset config representing texture atlases metadata
- (id)initWithTexturesDirectory:(NSString *)aTexturesDirectory
   textureAtlasConfigDictionary:(NSDictionary *)aTextureAtlasConfigDictionary
              keepImagesInAtlas:(BOOL)aKeepImagesInAtlas;

/// Initializes atlas with raw data
/// @note when atlas is initialized with this method, 'images' and 'textures' arrays will be initialized and kept in memory
/// by default. If you do not need 'images' to be kept in memory, call releaseImages.
/// @param anImagesArray array that contains UIImage-s
/// @param anElements list of atlas elements
/// @param aGenerateTextures if set to YES testures will be immediately generated from images
- (id)initWithImages:(NSArray *)anImagesArray
       atlasElements:(NSDictionary *)anAtlasElements
    generateTextures:(BOOL)aGenerateTextures;

/// Releases images that can be kept as temporary data source when creating atlas textures
- (void)releaseImages;

@end
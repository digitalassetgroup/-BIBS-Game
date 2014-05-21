////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFStaticObject.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFAnimatedObject.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Predeclarations

@class GAFAsset;
@class GAFSprite;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface

@interface GAFStaticObject : GAFAnimatedObject

+ (GAFStaticObject *)staticObjectWithAsset:(GAFAsset *)anAsset;
+ (GAFStaticObject *)staticObjectWithPath:(NSString *)aJSONPath;
+ (GAFStaticObject *)staticObjectWithAsset:(GAFAsset *)anAsset
                   externalNamedPartsAtlas:(GAFTextureAtlas *)anAtlas
                                 atlasName:(NSString *)anExternalAtlasName;

- (id)initWithAsset:(GAFAsset *)anAsset
externalNamedPartsAtlas:(GAFTextureAtlas *)anAtlas
          atlasName:(NSString *)anExternalAtlasName;

@end
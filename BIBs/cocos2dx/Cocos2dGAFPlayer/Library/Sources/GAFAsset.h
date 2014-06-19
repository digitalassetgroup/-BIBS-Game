////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFAsset.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@class GAFTextureAtlas;
@class GAFAnimatedObject;
@class GAFAnimationSequence;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//extern NSString * const kGAFAssetRootObjectName;
//extern NSString * const kGAFAssetWindowMaskName;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface GAFAsset : NSObject

#pragma mark Parsed data

@property (nonatomic, assign, readonly) NSUInteger majorVersion;
@property (nonatomic, assign, readonly) NSUInteger minorVersion;

@property (nonatomic, retain, readonly) GAFTextureAtlas *textureAtlas;
@property (nonatomic, assign, readonly) CGFloat usedAtlasContentScaleFactor;    // csf of used atlas

@property (nonatomic, retain, readonly) NSDictionary *objects;  // dictionary of objects [ObjectId -> AtlasElementName]
@property (nonatomic, retain, readonly) NSDictionary *masks;    // dictionary of masks [MaskId -> AtlasElementName]

@property (nonatomic, retain, readonly) NSDictionary *namedParts;    // dictionary of masks [MaskId -> AtlasElementName]

/// List of extended data object groups. Key - groupJSONName, Value - group, which is an array of parsed objects.
@property (nonatomic, retain, readonly) NSMutableDictionary *extendedDataObjectGroups;

#pragma mark Animation related data

@property (nonatomic, retain, readonly) NSArray *animationFrames;   ///< List of GAFAnimationFrame objects
@property (nonatomic, retain, readonly) NSDictionary *animationSequences;   ///< List of GAFAnimationSequences objects

#pragma mark Methods

+ (BOOL)isAssetVersionPlayable:(NSString *)version;

/// Initializes asset with path to JSON, atlas images should be located at the same level as JSON file (in the same folder)
/// @param aKeepImagesInAtlas specifies whether GAFTextureAtlas should leave of release images (texture are always present)
- (id)initWithJSONAtPath:(NSString *)aJSONPath keepImagesInAtlas:(BOOL)aKeepImagesInAtlas;
/// Same initialization as initWithJSONAtPath: but gives an ability to specify parsing classes for extendedData objects
- (id)initWithJSONAtPath:(NSString *)aJSONPath
extendedDataObjectClasses:(NSDictionary *)anExtendedDataObjectClasses
       keepImagesInAtlas:(BOOL)aKeepImagesInAtlas;

/// Initializes asset with JSON data and list of available atlases
/// @param aJSONData data loaded from json config
/// @param anAtlasesDataDictionary dictionary with pairs [AtlasName -> AtlasImageData].
///  If not nil, atlas texture data will be taken from it and anAtlasTexturesFolder will be ignored.
/// @param anAtlasTexturesFolder folder where atlas texture files can be found.
- (id)initWithJSONData:(NSData *)aJSONData
 atlasesDataDictionary:(NSDictionary *)anAtlasesDataDictionary
 orAtlasTexturesFolder:(NSString *)anAtlasTexturesFolder
     keepImagesInAtlas:(BOOL)aKeepImagesInAtlas;

/// Same initialization as initWithJSONData:atlasesDataDictionary:orAtlasTexturesFolder: but gives an ability
/// to specify parsing classes for extendedData objects
- (id)initWithJSONData:(NSData *)aJSONData
 atlasesDataDictionary:(NSDictionary *)anAtlasesDataDictionary
 orAtlasTexturesFolder:(NSString *)anAtlasTexturesFolder
extendedDataObjectClasses:(NSDictionary *)anExtendedDataObjectClasses
     keepImagesInAtlas:(BOOL)aKeepImagesInAtlas;

/// Returns list of all available sequence names
- (NSArray *)animationSequenceNames;
- (GAFAnimationSequence *)animationSequenceForName:(NSString *)aName;
- (GAFAnimationSequence *)animationSequenceByLastFrame:(NSUInteger)aLastFrame;

@end

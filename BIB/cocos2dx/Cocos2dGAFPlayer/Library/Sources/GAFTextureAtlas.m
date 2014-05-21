////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFTextureAtlas.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "GAFTextureAtlas.h"
#import "GAFTextureAtlasElement.h"
#import "CCTexture2D.h"
#import "Support/CCFileUtils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static NSString * const kAtlasesKey     = @"atlases";
static NSString * const kElementsKey     = @"elements";
static NSString * const kSourcesKey      = @"sources";
static NSString * const kSourceKey       = @"source";
static NSString * const kCSFKey          = @"csf";
static NSString * const kTextureAtlasKey = @"textureAtlas";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface GAFTextureAtlas ()

@property (nonatomic, retain) NSMutableArray *images;
@property (nonatomic, retain) NSMutableArray *textures;

- (void)loadElementsFromAnimationConfigDictionary:(NSDictionary *)aConfigDictionary;

- (void)generateTexturesFromImages;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation GAFTextureAtlas

#pragma mark -
#pragma mark Properties

#pragma mark -
#pragma mark Initialization & Release

- (id)init
{
    self = [super init];
    if (nil != self)
    {
        self.images = [[[NSMutableArray alloc] initWithCapacity:1] ah_autorelease];
        self.textures = [[[NSMutableArray alloc] initWithCapacity:1] ah_autorelease];
        
        self.loaded = NO;
    }
    return self;
}

- (id)initWithTextureAtlasesDictionary:(NSDictionary *)aTextureAtlasesDictionary
          textureAtlasConfigDictionary:(NSDictionary *)aTextureAtlasConfigDictionary
                     keepImagesInAtlas:(BOOL)aKeepImagesInAtlas
{
    NSAssert(aTextureAtlasesDictionary != nil && aTextureAtlasConfigDictionary != nil, @"parameters should not equal to nil");
    if (aTextureAtlasesDictionary == nil)
    {
        CCLOGWARN(@"ERROR: initializing TextureAtlas. aTextureAtlasesDictionary not present");
        [self ah_release];
        return nil;
    }
    if (aTextureAtlasConfigDictionary == nil)
    {
        CCLOGWARN(@"ERROR: initializing TextureAtlas. aTextureAtlasConfigDictionary not present");
        [self ah_release];
        return nil;
    }
    
    if ((self = [self init]))
    {
        NSArray *atlasesInfo = aTextureAtlasConfigDictionary[kAtlasesKey];
        // Order by atlas id
        atlasesInfo = [atlasesInfo sortedArrayUsingComparator:
                       ^NSComparisonResult(id obj1, id obj2)
                       {
                           NSDictionary *info1 = (NSDictionary *)obj1;
                           NSDictionary *info2 = (NSDictionary *)obj2;
                           NSInteger id1 = [info1[@"id"] integerValue];
                           NSInteger id2 = [info2[@"id"] integerValue];
                           if (id2 > id1)
                               return NSOrderedAscending;
                           else if (id2 < id1)
                               return NSOrderedDescending;
                           else
                               return NSOrderedSame;
                       }];
        
        // Load textures from Data dictionary for each mentioned atlas
        if (atlasesInfo != nil && atlasesInfo.count > 0)
        {
            for (NSDictionary *atlasInfo in atlasesInfo)
            {
                NSArray *sources = atlasInfo[kSourcesKey];
                if (sources == nil)
                {
                    CCLOGWARN(@"ERROR: initializing sources. sources not present");
                    [self ah_release];
                    return nil;
                }
                
                int desiredCsf = 1;
                
                if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] &&
                    ([UIScreen mainScreen].scale == 2.0))
                {
                    desiredCsf = 2;
                }
                
                NSString * source = nil;
                int csf = 0;
                for (NSDictionary * csfdict in sources)
                {
                    csf = [csfdict[kCSFKey] intValue];
                    NSString * s = (NSString *)csfdict[kSourceKey];
                    if (1 == csf)
                    {
                        source = s;
                    }
                    if (csf == desiredCsf)
                    {
                        source = s;
                        break;
                    }
                }
                
                if (csf == 0)
                {
                    CCLOGWARN(@"ERROR: initializing sources.  csf not present");
                    [self ah_release];
                    return nil;
                }
                if (source == nil)
                {
                    CCLOGWARN(@"ERROR: initializing sources.  source not present");
                    [self ah_release];
                    return nil;
                }
                
                NSData *atlasTextureData = aTextureAtlasesDictionary[source];
                if (atlasTextureData != nil)
                {
                    UIImage *image = [[UIImage alloc] initWithData:atlasTextureData scale:csf];
                    if (image == nil)
                    {
                        CCLOGWARN(@"Cannot create UIImage for texture name(key) - %@", source);
                        [self ah_release];
                        return nil;
                    }
                    [self.images addObject:image];
                    [image ah_release];
                }
                else
                {
                    CCLOGWARN(@"Cannot find texture for name(key) - %@", source);
                    [self ah_release];
                    return nil;
                }
            }
        }
        
        [self loadElementsFromAnimationConfigDictionary:aTextureAtlasConfigDictionary];
        [self generateTexturesFromImages];
        if (!aKeepImagesInAtlas)
        {
            [self releaseImages];
        }
    }
    return self;
}

- (id)initWithTexturesDirectory:(NSString *)aTexturesDirectory
   textureAtlasConfigDictionary:(NSDictionary *)aTextureAtlasConfigDictionary
              keepImagesInAtlas:(BOOL)aKeepImagesInAtlas
{
    NSAssert(aTexturesDirectory != nil && aTextureAtlasConfigDictionary != nil, @"parameters should not equal to nil");
    if (aTexturesDirectory == nil)
    {
        CCLOGWARN(@"ERROR: initializing TextureAtlas. aTexturesDirectory not present");
        [self ah_release];
        return nil;
    }
    if (aTextureAtlasConfigDictionary == nil)
    {
        CCLOGWARN(@"ERROR: initializing TextureAtlas. aTextureAtlasConfigDictionary not present");
        [self ah_release];
        return nil;
    }
    
    if ((self = [self init]))
    {
        NSArray *atlasesInfo = aTextureAtlasConfigDictionary[kAtlasesKey];
        // Order by atlas id
        atlasesInfo = [atlasesInfo sortedArrayUsingComparator:
                       ^NSComparisonResult(id obj1, id obj2)
                       {
                           NSDictionary *info1 = (NSDictionary *)obj1;
                           NSDictionary *info2 = (NSDictionary *)obj2;
                           NSInteger id1 = [info1[@"id"] integerValue];
                           NSInteger id2 = [info2[@"id"] integerValue];
                           if (id2 > id1)
                               return NSOrderedAscending;
                           else if (id2 < id1)
                               return NSOrderedDescending;
                           else
                               return NSOrderedSame;
                       }];
        
        // Load textures for each mentioned atlas
        if (atlasesInfo != nil && atlasesInfo.count > 0)
        {
            for (NSDictionary *atlasInfo in atlasesInfo)
            {
                NSArray *sources = atlasInfo[kSourcesKey];
                if(sources == nil)
                {
                    CCLOGWARN(@"ERROR: initializing sources. 'sources' not present");
                    [self ah_release];
                    return nil;
                }
                int desiredCsf = 1;
                int realCsf = 1;
                
                if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] &&
                    ([UIScreen mainScreen].scale == 2.0))
                {
                    desiredCsf = 2;
                }
                
                int csf = 0;
                NSString * source = nil;
                for (NSDictionary * csfdict in sources)
                {
                    csf = [csfdict[kCSFKey] intValue];
                    NSString * s = (NSString *)csfdict[kSourceKey];
                    if (1 == csf)
                    {
                        source = s;
                    }
                    if (csf == desiredCsf)
                    {
                        source = s;
                        realCsf = csf;
                        break;
                    }
                }
                
                if (csf == 0)
                {
                    CCLOGWARN(@"ERROR: initializing sources. 'csf' not present");
                    [self ah_release];
                    return nil;
                }
                if (source == nil)
                {
                    CCLOGWARN(@"ERROR: initializing sources. 'source' not present");
                    [self ah_release];
                    return nil;
                }
                
                NSData *imageData = [NSData dataWithContentsOfFile:[aTexturesDirectory stringByAppendingPathComponent:source]];
                if (imageData == nil)
                {
                    CCLOGWARN(@"Cannot load imageData for name(key) - %@", source);
                    [self ah_release];
                    return nil;
                }
                UIImage *image = [[UIImage alloc] initWithData:imageData scale:realCsf];
                if (image == nil)
                {
                    CCLOGWARN(@"Cannot create UIImage for texture for name(key) - %@", source);
                    [self ah_release];
                    return nil;
                }
                [self.images addObject:image];
                [image ah_release];
            }
        }
        
        [self loadElementsFromAnimationConfigDictionary:aTextureAtlasConfigDictionary];
        [self generateTexturesFromImages];
        if (!aKeepImagesInAtlas)
        {
            [self releaseImages];
        }
    }
    return self;
}

- (id)initWithImages:(NSArray *)anImagesArray
       atlasElements:(NSDictionary *)anAtlasElements
    generateTextures:(BOOL)aGenerateTextures
{
    if (anImagesArray == nil)
    {
        CCLOGWARN(@"ERROR: initializing TextureAtlas. anImagesArray not present");
        [self ah_release];
        return nil;
    }
    if (anAtlasElements == nil)
    {
        CCLOGWARN(@"ERROR: initializing TextureAtlas. anAtlasElements not present");
        [self ah_release];
        return nil;
    }
    
    if ((self = [self init]))
    {
        self.images = [[anImagesArray mutableCopy] ah_autorelease];
        self.elements = [[anAtlasElements copy] ah_autorelease];
        
        self.loaded = YES;
        
        if (aGenerateTextures)
        {
            [self generateTexturesFromImages];
        }
    }
    return self;
}

- (void)dealloc
{
    [_images ah_release];
    [_textures ah_release];
    [_elements ah_release];

#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Public methods

- (void)releaseImages
{
    if (self.images.count > 0)
    {
        if (self.images.count == self.textures.count)
        {
            [self.images removeAllObjects];
        }
        else
        {
            CCLOGWARN(@"GAFTextureAtlas images cannot be released as textures were not created, releasing of images will lead to complete image data loss");
        }
    }
}

#pragma mark -
#pragma mark Private methods

- (void)generateTexturesFromImages
{
    if (self.images.count == 0)
        return;
    
    if ([EAGLContext currentContext] == nil)
    {
        CCLOGWARN(@"Cannot create CCTexture2D in GAFTextureAtlas because there is no EAGLContext in thread %@", [NSThread currentThread]);
        return;
    }
    
    for (UIImage *image in self.images)
    {
        // kCCResolutioniPad kCCResolutioniPadRetinaDisplay // should not be used - it should work universally across the platforms
        CCTexture2D *texture = [[CCTexture2D alloc] initWithCGImage:image.CGImage resolutionType:kCCResolutioniPadRetinaDisplay];
        [self.textures addObject:texture];
        [texture ah_release];
    }
}

- (void)loadElementsFromAnimationConfigDictionary:(NSDictionary *)aConfigDictionary
{
    if (aConfigDictionary == nil)
    {
        CCLOGWARN(@"Error in loadElementsFromAnimationConfigDictionary. aConfigDictionary is nil.");
        return;
    }
    
    NSArray *nElements = aConfigDictionary[kElementsKey];
    
    if (nElements != nil)
    {
        @autoreleasepool
        {
            NSMutableDictionary *parsedElements = [[NSMutableDictionary alloc] initWithCapacity:[nElements count]];
            for (id nElement in nElements)
            {
                if ([nElement isKindOfClass:[NSDictionary class]])
                {
                    GAFTextureAtlasElement *element =
                        [[GAFTextureAtlasElement alloc] initWithDictionary:(NSDictionary *)nElement];
                    if (element != nil)
                    {
                        parsedElements[element.name] = element;
                        [element ah_release];
                    }
                }
                else
                {
                   CCLOGWARN(@"Error when parsing texture atlas element JSON. Atlas element not of NSDictionary type.");
                }
            }
            self.elements = parsedElements;
            [parsedElements ah_release];
        }
    }
    else
    {
        CCLOGWARN(@"Error when parsing texture atlas element JSON.");
        return;
    }
    
    self.loaded = YES;
}

@end
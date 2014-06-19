#import "GAFTextureAtlas.h"
#import "GAFTextureAtlasElement.h"
#import "CCTexture2D.h"
#import "CCFileUtils.h"

static NSString * const kElementsKey     = @"elements";
static NSString * const kImagePathKey    = @"source";
static NSString * const kTextureAtlasKey = @"textureAtlas";

@interface GAFTextureAtlas ()

@property (nonatomic, retain) NSMutableArray *images;
@property (nonatomic, retain) NSMutableArray *textures;

- (void)loadElementsFromAnimationConfigDictionary:(NSDictionary *)aConfigDictionary;

@end

@implementation GAFTextureAtlas

#pragma mark -
#pragma mark Properties

- (CCTexture2D *)texture
{
    if (self.textures != nil && [self.textures count] > 0)
    {
        return self.textures[0];
    }
    return nil;
}

- (NSArray *)textures
{
    if (_textures == nil)
    {
        _textures = [[NSMutableArray alloc] initWithCapacity:self.images.count];
        
        for (UIImage *image in self.images)
        {
            CCTexture2D *texture = [[CCTexture2D alloc] initWithCGImage:image.CGImage resolutionType:kCCResolutioniPad];
            [_textures addObject:texture];
            [texture release];
        }
    }
    return _textures;
}

#pragma mark -
#pragma mark Initialization & Release

- (id)init
{
    self = [super init];
    if (nil != self)
    {
        self.images = [[[NSMutableArray alloc] initWithCapacity:3] autorelease];
        
        self.loaded = NO;
    }
    return self;
}


- (id)initWithJsonPath:(NSString*)jsonPath configDictionary:(NSDictionary *)aConfigDictionary imagesData:(NSArray *)anImageDatas
{
    NSAssert(anImageDatas != nil && aConfigDictionary != nil, @"parameters should not equal to nil");
    
    self = [self init];
	
	NSString * atlasDirectory = [jsonPath stringByDeletingLastPathComponent];
    if (nil != self)
    {
        for (NSDictionary *data in anImageDatas)
        {
			NSString * source = [data objectForKey:kImagePathKey];
            UIImage * image = [UIImage imageWithContentsOfFile:[atlasDirectory stringByAppendingPathComponent:source]]; // autoreleased object
            [_images addObject:image];
        }
        
        [self loadElementsFromAnimationConfigDictionary:aConfigDictionary];
    }
    return self;
}

- (void)dealloc
{
    [_images release];
    [_textures release];
    [_elements release];
    [super dealloc];
}

#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

- (void)loadElementsFromAnimationConfigDictionary:(NSDictionary *)aConfigDictionary
{
    NSArray *nElements = [aConfigDictionary objectForKey:kElementsKey];
    
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
                        [parsedElements setObject:element forKey:element.name];
                        [element release];
                    }
                }
                else
                {
                   CCLOGWARN(@"Error when parsing texture atlas element JSON. Atlas element not of NSDictionary type");
                }
            }
            self.elements = parsedElements;
            [parsedElements release];
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
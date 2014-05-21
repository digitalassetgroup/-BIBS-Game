#import "GAFTextureAtlasElement.h"
#import "ccMacros.h"

static NSString * const kNameKey = @"name";
static NSString * const kXKey = @"x";
static NSString * const kYKey = @"y";
static NSString * const kHeightKey = @"height";
static NSString * const kWidthKey = @"width";
static NSString * const kPivotXKey = @"pivotX";
static NSString * const kPivotYKey = @"pivotY";
static NSString * const kAtlasIDKey = @"atlasID";


@interface GAFTextureAtlasElement ()
@end

@implementation GAFTextureAtlasElement

#pragma mark -
#pragma mark Properties

@synthesize name;
@synthesize pivotPoint;
@synthesize bounds;
@synthesize atlasIdx;

#pragma mark -
#pragma mark Initialization & Release

- (id)initWithDictionary:(NSDictionary *)aDictionary
{
    self = [super init];
    if (nil != self)
    {
        self.name = [aDictionary objectForKey:kNameKey];
        NSNumber *nX = [aDictionary objectForKey:kXKey];
        NSNumber *nY = [aDictionary objectForKey:kYKey];
        NSNumber *nHeight = [aDictionary objectForKey:kHeightKey];
        NSNumber *nWidth = [aDictionary objectForKey:kWidthKey];
        if (nX != nil && nY != nil && nHeight != nil && nWidth != nil)
        {
            self.bounds = CGRectMake([nX integerValue], [nY integerValue],
                                     [nWidth integerValue], [nHeight integerValue]);
        }
        else
        {
            CCLOGWARN(@"Error while creating GAFTextureAtlasElement from NSDictionary. One or more size attributes missing.");
            [self release];
            return nil;
        }
        
        NSNumber *nPivotX = [aDictionary objectForKey:kPivotXKey];
        NSNumber *nPivotY = [aDictionary objectForKey:kPivotYKey];
        if (nPivotX != nil && nPivotY != nil)
        {
            self.pivotPoint = CGPointMake([nPivotX integerValue], [nPivotY integerValue]);
        }
        else
        {
            CCLOGWARN(@"Error while creating GAFTextureAtlasElement from NSDictionary. One or more pivot attributes missing.");
            [self release];
            return nil;
        }
        
        NSNumber *atlasId = aDictionary[kAtlasIDKey];
        if (atlasId != nil)
        {
            self.atlasIdx = [atlasId integerValue] - 1;
        }
        else
        {
            self.atlasIdx = 0;
            CCLOGWARN(@"AtlasID is missing, assuming first atlas is used.");
        }
    }
    return self;
}

- (void)dealloc
{
    [name release];
    [super dealloc];
}

#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

@end
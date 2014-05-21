#import "GAFActionObject.h"
#import "GAFTextureAtlas.h"
#import "GAFTextureAtlasElement.h"
#import "ccMacros.h"


static NSString * const kClassNameKey   = @"className";
static NSString * const kNameKey        = @"name";
static NSString * const kXKey           = @"x";
static NSString * const kYKey           = @"y";
static NSString * const kRectXKey       = @"rectX";
static NSString * const kRectYKey       = @"rectY";
static NSString * const kRectWidthKey   = @"rectWidth";
static NSString * const kRectHeightKey  = @"rectHeight";


@interface GAFActionObject ()
@end

@implementation GAFActionObject

#pragma mark -
#pragma mark Properties

@synthesize name;
@synthesize pivotPoint;
@synthesize bounds;

#pragma mark -
#pragma mark Initialization & Release

- (id)initWithDictionary:(NSDictionary *)aDictionary
{
    self = [super init];
    if (nil != self)
    {
        self.name = [aDictionary objectForKey:kNameKey];
        
        NSNumber *x = [aDictionary objectForKey:kXKey];
        NSNumber *y = [aDictionary objectForKey:kYKey];
        if (x != nil && y != nil)
        {
            self.pivotPoint = CGPointMake([x integerValue], [y integerValue]);
        }
        else
        {
            CCLOGWARN(@"Error while creating GAFActionObject from NSDictionary. One of x, y attributes missing.");
            [self release];
            return nil;
        }
        
        NSNumber *rectX = [aDictionary objectForKey:kRectXKey];
        NSNumber *rectY = [aDictionary objectForKey:kRectYKey];
        NSNumber *rectWidth = [aDictionary objectForKey:kRectWidthKey];
        NSNumber *rectHeight = [aDictionary objectForKey:kRectHeightKey];
        if (rectX != nil &&
            rectY != nil &&
            rectWidth != nil &&
            rectHeight != nil)
        {
            self.bounds = CGRectMake([rectX integerValue], [rectY integerValue],
                                     [rectWidth integerValue], [rectHeight integerValue]);
        }
        else
        {
            CCLOGWARN(@"Error while creating GAFActionObject from NSDictionary. One of rect* attributes missing.");
            [self release];
            return nil;
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
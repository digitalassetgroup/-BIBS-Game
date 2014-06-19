////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFTextureAtlasElement.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "GAFTextureAtlasElement.h"
#import "ccMacros.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static NSString * const kNameKey = @"name";
static NSString * const kXKey = @"x";
static NSString * const kYKey = @"y";
static NSString * const kScaleKey = @"scale";
static NSString * const kHeightKey = @"height";
static NSString * const kWidthKey = @"width";
static NSString * const kPivotXKey = @"pivotX";
static NSString * const kPivotYKey = @"pivotY";
static NSString * const kAtlasIDKey = @"atlasID";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation GAFTextureAtlasElement

#pragma mark -
#pragma mark Properties

@synthesize name;
@synthesize pivotPoint;
@synthesize bounds;
@synthesize atlasIdx;

#pragma mark -
#pragma mark Initialization & Release

- (id)init
{
    self = [super init];
    if (nil != self)
    {
        self.scale = 1.0;
    }
    return self;
}

- (id)initWithDictionary:(NSDictionary *)aDictionary
{
    self = [self init];
    if (nil != self)
    {
        self.name = aDictionary[kNameKey];
        if (self.name == nil)
        {
            CCLOGWARN(@"ERROR: initializing GAFTextureAtlasElement. 'name' not present.");
            [self ah_release];
            return nil;
        }
        NSNumber *s = aDictionary[kScaleKey];
		if (s == nil)
		{
			self.scale = 1.0f;
		}
		else
		{
			self.scale = [s floatValue];
		}
        NSNumber *nX = aDictionary[kXKey];
        NSNumber *nY = aDictionary[kYKey];
        NSNumber *nHeight = aDictionary[kHeightKey];
        NSNumber *nWidth = aDictionary[kWidthKey];
        if (nX != nil && nY != nil && nHeight != nil && nWidth != nil)
        {
            self.bounds = CGRectMake([nX integerValue], [nY integerValue],
                                     [nWidth integerValue], [nHeight integerValue]);
        }
        else
        {
            CCLOGWARN(@"Error while creating GAFTextureAtlasElement from NSDictionary. One or more size attributes missing.");
            [self ah_release];
            return nil;
        }
        
        NSNumber *nPivotX = aDictionary[kPivotXKey];
        NSNumber *nPivotY = aDictionary[kPivotYKey];
        if (nPivotX != nil && nPivotY != nil)
        {
            self.pivotPoint = CGPointMake([nPivotX integerValue], [nPivotY integerValue]);
        }
        else
        {
            CCLOGWARN(@"Error while creating GAFTextureAtlasElement from NSDictionary. One or more pivot attributes missing.");
            [self ah_release];
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
    [name ah_release];
	
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

@end
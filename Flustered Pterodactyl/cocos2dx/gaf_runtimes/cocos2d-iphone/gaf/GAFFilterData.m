#import "GAFFilterData.h"

NSString * const kBlurFilterName = @"Fblur";

@implementation GAFFilterData

@end

@implementation GAFBlurFilterData

@synthesize blurSize;

- (NSString *)description
{
    return [NSString stringWithFormat:@"{ %@, x:%f, y:%f }", kBlurFilterName, blurSize.width, blurSize.height];
}

@end
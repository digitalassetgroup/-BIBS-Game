#import "GAFAnimationFrame.h"
#import "GAFSubobjectState.h"
#import "GAFTextureAtlas.h"

static NSString * const kStateKey = @"state";

@interface GAFAnimationFrame ()
@end

@implementation GAFAnimationFrame

#pragma mark -
#pragma mark Properties

@synthesize objectsStates;

#pragma mark -
#pragma mark Initialization & Release

- (void)dealloc
{
    [objectsStates release];
    [super dealloc];
}

#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

@end
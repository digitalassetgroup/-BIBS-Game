////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFStencilMaskSprite.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "GAFSprite.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@class GAFStencilMaskSprite;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface CCNode (PCStencilMaskExtensions)

@property (nonatomic, assign) GAFStencilMaskSprite *maskContainer;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface GAFStencilMaskSprite : GAFSprite
{
@private
    NSMutableArray *_maskedObjects;
}

@property (nonatomic, retain, readonly) NSArray *maskedObjects;

- (void)addMaskedObject:(CCNode *)anObject;
- (void)removeMaskedObject:(CCNode *)anObject;
- (void)removeAllMaskedObjects;

// Forces to sort masked objects by zIndex on next draw
- (void)invalidateMaskedObjectsOrder;

@end
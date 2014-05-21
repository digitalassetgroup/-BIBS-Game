////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFStencilMaskSprite.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFStencilMaskSprite.h"
#import "CCShaderCache.h"
#import "CCGLProgram.h"
#import "ccShaders.h"
#import <objc/runtime.h>
#import "Support/OpenGL_Internal.h"
#import "CCGLProgram+GAFExtensions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

static NSString * const kPCStencilMaskAlphaFilterProgramCacheKey = @"kPCScrollLayerAlphaFilterProgramCacheKey";
static NSString * const kPCStencilMaskAlphaFilterFragmentShaderFilename = @"pcShader_PositionTexture_alphaFilter.fs";

static NSString * const kCCNodeMaskContainerAssociatedObject = @"CCNodeMaskContainerAssociatedObject";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Category

@implementation CCNode (GAFStencilMaskExtensions)

- (void)setMaskContainer:(GAFStencilMaskSprite *)maskContainer
{
    objc_setAssociatedObject(self, (__bridge const void *)kCCNodeMaskContainerAssociatedObject, maskContainer, OBJC_ASSOCIATION_ASSIGN);
}

- (GAFStencilMaskSprite *)maskContainer
{
    return (GAFStencilMaskSprite *)objc_getAssociatedObject(self, (__bridge const void *)kCCNodeMaskContainerAssociatedObject);
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFStencilMaskSprite ()
{
    BOOL _isReorderMaskedObjectsDirty;
}

@property (nonatomic, retain) NSArray *maskedObjects;
- (NSMutableArray *)mutableMaskedObjects;

- (CCGLProgram *)programShaderForMask;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFStencilMaskSprite

@synthesize maskedObjects = _maskedObjects;

- (NSMutableArray *)mutableMaskedObjects
{
    return (NSMutableArray *)self.maskedObjects;
}

#pragma mark -
#pragma mark Properties

#pragma mark -
#pragma mark Initialization & Release

- (id)initWithTexture:(CCTexture2D *)texture rect:(CGRect)rect rotated:(BOOL)rotated
{
    if(texture == nil)
    {
        CCLOGWARN(@"ERROR: initializing GAFStencilMaskSprite. Texture not present");
        [self ah_release];
        return nil;
    }
    
    if ((self = [super initWithTexture:texture
                                  rect:rect
                               rotated:rotated]))
    {
        _maskedObjects = [[NSMutableArray alloc] init];
        
        self.shaderProgram = [self programShaderForMask];
        
        _isReorderMaskedObjectsDirty = NO;
    }
    return self;
}

- (void)cleanup
{
    [super cleanup];
    
    [self.maskedObjects makeObjectsPerformSelector:@selector(cleanup)];
}

- (void)dealloc
{
    [_maskedObjects ah_release];

#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Overriden methods

- (void)visit
{
    [super visit];
    
    [self sortAllMaskedObjects];
    
    // Draw subobjects, assuming mask and object are on the same layer
    for (CCNode *object in _maskedObjects)
    {
        [object visit];
    }
    
    // Disable stencil
    glDisable(GL_STENCIL_TEST);
}

- (void)sortAllMaskedObjects
{
	if (_isReorderMaskedObjectsDirty)
	{
        [_maskedObjects sortUsingComparator:^NSComparisonResult(id obj1, id obj2)
         {
             NSUInteger zObj1 = ((CCNode *)obj1).zOrder;
             NSUInteger zObj2 = ((CCNode *)obj2).zOrder;
             if (zObj1 < zObj2)
                 return NSOrderedAscending;
             else if (zObj1 > zObj2)
                 return NSOrderedDescending;
             else
                 return NSOrderedSame;
         }];
        
		_isReorderMaskedObjectsDirty = NO;
	}
}

- (void)draw
{
    // Prepare stencil
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glStencilFunc(GL_NEVER, 1, 1);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
    
    // Draw mask
    [super draw];
    
    // Use stencil
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1);
}

#pragma mark -
#pragma mark Public methods

- (void)addMaskedObject:(CCNode *)anObject
{
    if (anObject.maskContainer != nil)
    {
        [anObject.maskContainer removeMaskedObject:anObject];
    }
    if (anObject.maskContainer != self)
    {
        [anObject setMaskContainer:self];
        [[self mutableMaskedObjects] addObject:anObject];
        _isReorderMaskedObjectsDirty = YES;
    }
}

- (void)removeMaskedObject:(CCNode *)anObject
{
    if ([self.maskedObjects containsObject:anObject])
    {
        [anObject setMaskContainer:nil];
        [[self mutableMaskedObjects] removeObject:anObject];
        _isReorderMaskedObjectsDirty = YES;
    }
}

- (void)removeAllMaskedObjects
{
    for (CCNode *object in self.maskedObjects)
    {
        [object setMaskContainer:nil];
    }
    
    [[self mutableMaskedObjects] removeAllObjects];
    
    _isReorderMaskedObjectsDirty = YES;
}

- (void)invalidateMaskedObjectsOrder
{
    _isReorderMaskedObjectsDirty = YES;
}

#pragma mark -
#pragma mark Private methods

- (CCGLProgram *)programShaderForMask
{
    CCGLProgram *program = [[CCShaderCache sharedShaderCache] programForKey:kPCStencilMaskAlphaFilterProgramCacheKey];
    if (program == nil)
    {
        program = [[CCGLProgram alloc] initWithVertexShaderByteArray:ccPositionTextureColor_vert
                                              fragmentShaderFilename:kPCStencilMaskAlphaFilterFragmentShaderFilename];
        
        if (program != nil)
        {
            [program addAttribute:kCCAttributeNamePosition index:kCCVertexAttrib_Position];
            [program addAttribute:kCCAttributeNameColor index:kCCVertexAttrib_Color];
            [program addAttribute:kCCAttributeNameTexCoord index:kCCVertexAttrib_TexCoords];
            
            [program link];
            [program updateUniforms];
            
            CHECK_GL_ERROR_DEBUG();
            
            [[CCShaderCache sharedShaderCache] addProgram:program forKey:kPCStencilMaskAlphaFilterProgramCacheKey];
            [program ah_release];
        }
        else
        {
            CCLOGWARN(@"Cannot load program for GAFSpriteWithAlpha.");
            [self ah_release];
            return nil;
        }
    }
    
    [program use];
    
    return program;
}

@end
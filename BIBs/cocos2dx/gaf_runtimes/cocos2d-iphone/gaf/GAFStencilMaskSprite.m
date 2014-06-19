////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFStencilMaskSprite.m
//  AnimationTest
//
//  Created by Gregory Maksyuk on 2/20/13.
//  Copyright 2013 Catalyst Apps. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFStencilMaskSprite.h"
#import "CCShaderCache.h"
#import "CCGLProgram.h"
#import "ccShaders.h"
#import "ccMacros.h"
#import <objc/runtime.h>
#import "OpenGL_Internal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

static NSString * const kPCStencilMaskAlphaFilterProgramCacheKey = @"kPCScrollLayerAlphaFilterProgramCacheKey";
static NSString * const kPCStencilMaskAlphaFilterFragmentShaderFilename = @"pcShader_PositionTexture_alphaFilter.fs";

static NSString * const kCCNodeMaskContainerAssociatedObject = @"CCNodeMaskContainerAssociatedObject";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Category

@implementation CCNode (PCStencilMaskExtensions)

- (void)setMaskContainer:(GAFStencilMaskSprite *)maskContainer
{
    objc_setAssociatedObject(self, (const void *)kCCNodeMaskContainerAssociatedObject, maskContainer, OBJC_ASSOCIATION_ASSIGN);
}

- (GAFStencilMaskSprite *)maskContainer
{
    return (GAFStencilMaskSprite *)objc_getAssociatedObject(self, (const void *)kCCNodeMaskContainerAssociatedObject);
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
    if ((self = [super initWithTexture:texture
                                  rect:CGRectMake(0, 0, rect.size.width, rect.size.height)
                               rotated:rotated]))
    {
        _maskedObjects = [[NSMutableArray alloc] init];
        
        self.shaderProgram = [self programShaderForMask];
        
        _isReorderMaskedObjectsDirty = NO;
    }
    return self;
}

- (void)dealloc
{
    [_maskedObjects release];
    [super dealloc];
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
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 1);
    
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
    [anObject setMaskContainer:self];
    [[self mutableMaskedObjects] addObject:anObject];
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
            [program release];
        }
        else
        {
            CCLOGWARN(@"Cannot load program for GAFSpriteWithAlpha.");
            [self release];
            return nil;
        }
    }
    
    [program use];
    
    return program;
}

@end
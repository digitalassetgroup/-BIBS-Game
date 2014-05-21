////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFSprite.m
//  AnimationTest
//
//  Created by Gregory Maksyuk on 12/11/12.
//  Copyright 2012 Catalyst Apps. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFSprite.h"
#import "GAFSprite_Protected.h"
#import "OpenGL_Internal.h"
#import "CCGLProgram.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFSprite ()
@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macroses

// Copied from CCSprite.m
#if CC_SPRITEBATCHNODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__A__) ( (int)(__A__))
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFSprite

#pragma mark -
#pragma mark Properties

@synthesize useExternalTransform;
@synthesize externalTransform = _externalTransform;
@synthesize useChildTransform;
@synthesize childTransform = _childTransform;
@synthesize objectId = _objectId;
@synthesize useSeparateBlendFunc = _useSeparateBlendFunc;
@synthesize blendFuncSeparate;
@synthesize blendEquation = _blendEquation;
@synthesize isLocator = _isLocator;

- (void)setExternalTransform:(CGAffineTransform)anExternalTransform
{
    if (!CGAffineTransformEqualToTransform(_externalTransform, anExternalTransform))
    {
        _externalTransform = anExternalTransform;
        self.useExternalTransform = YES;
        
        _isTransformDirty = _isInverseDirty = YES;
    }
}

- (void)setChildTransform:(CGAffineTransform)aChildTransform
{
    if (!CGAffineTransformEqualToTransform(_childTransform, aChildTransform))
    {
        _childTransform = aChildTransform;
        self.useChildTransform = YES;
        
        _isTransformDirty = _isInverseDirty = YES;
    }
}

#pragma mark -
#pragma mark Init

- (id)initWithTexture:(CCTexture2D *)texture rect:(CGRect)rect rotated:(BOOL)rotated
{
    if ((self = [super initWithTexture:texture rect:rect rotated:rotated]))
    {
        _useSeparateBlendFunc = NO;
        _blendEquation = -1;
		_isLocator = NO;
     
        _externalTransform = CGAffineTransformIdentity;
        _childTransform = CGAffineTransformIdentity;
    }
    return self;
}

- (void)dealloc
{
    [_objectId release];
    [super dealloc];
}

#pragma mark -
#pragma mark Overriden methods

- (CGAffineTransform)nodeToParentTransform
{
    if (self.useExternalTransform)
    {
        if (_isTransformDirty)
        {
            _transform = CGAffineTransformTranslate(_externalTransform, -_anchorPointInPoints.x, -_anchorPointInPoints.y);
            
            if ([_parent isKindOfClass:[GAFSprite class]] && [(GAFSprite *)_parent useChildTransform])
            {
                _transform = CGAffineTransformConcat(_transform, [(GAFSprite *)_parent childTransform]);
            }
            
            _isTransformDirty = NO;
        }
        return _transform;
	}
    else
    {
        return [super nodeToParentTransform];
    }
}

- (void)updateTransform
{
	NSAssert( _batchNode, @"updateTransform is only valid when CCSprite is being rendered using an CCSpriteBatchNode");
    
	// recalculate matrix only if it is dirty
	if (self.dirty)
    {
		// If it is not visible, or one of its ancestors is not visible, then do nothing:
		if (!self.visible || ( _parent && _parent != _batchNode && ((CCSprite*)_parent)->_shouldBeHidden))
        {
			_quad.br.vertices = _quad.tl.vertices = _quad.tr.vertices = _quad.bl.vertices = (ccVertex3F){0,0,0};
			_shouldBeHidden = YES;
		}
		else
        {
			_shouldBeHidden = NO;
            
			if (!_parent || _parent == _batchNode)
            {
				_transformToBatch = [self nodeToParentTransform];
            }
			else
            {
				NSAssert([_parent isKindOfClass:[CCSprite class]], @"Logic error in CCSprite. Parent must be a CCSprite");
                
				_transformToBatch = CGAffineTransformConcat( [self nodeToParentTransform] , ((CCSprite*)_parent)->_transformToBatch);
			}
            
			//
			// calculate the Quad based on the Affine Matrix
			//
            
			CGSize size = _rect.size;
            
            // x1, x2, y1, y2 are changed (comapring to CCSprite) to place Sprite at center
            float x1 = _offsetPosition.x - size.width / 2;
			float y1 = _offsetPosition.y - size.height / 2;
            
			float x2 = x1 + size.width;
			float y2 = y1 + size.height;
            
			float x = _transformToBatch.tx;
			float y = _transformToBatch.ty;
            
			float cr = _transformToBatch.a;
			float sr = _transformToBatch.b;
			float cr2 = _transformToBatch.d;
			float sr2 = -_transformToBatch.c;
			float ax = x1 * cr - y1 * sr2 + x;
			float ay = x1 * sr + y1 * cr2 + y;
            
			float bx = x2 * cr - y1 * sr2 + x;
			float by = x2 * sr + y1 * cr2 + y;
            
			float cx = x2 * cr - y2 * sr2 + x;
			float cy = x2 * sr + y2 * cr2 + y;
            
			float dx = x1 * cr - y2 * sr2 + x;
			float dy = x1 * sr + y2 * cr2 + y;
            
			_quad.bl.vertices = (ccVertex3F) { RENDER_IN_SUBPIXEL(ax), RENDER_IN_SUBPIXEL(ay), _vertexZ };
			_quad.br.vertices = (ccVertex3F) { RENDER_IN_SUBPIXEL(bx), RENDER_IN_SUBPIXEL(by), _vertexZ };
			_quad.tl.vertices = (ccVertex3F) { RENDER_IN_SUBPIXEL(dx), RENDER_IN_SUBPIXEL(dy), _vertexZ };
			_quad.tr.vertices = (ccVertex3F) { RENDER_IN_SUBPIXEL(cx), RENDER_IN_SUBPIXEL(cy), _vertexZ };
		}
        
		[_textureAtlas updateQuad:&_quad atIndex:_atlasIndex];
		_dirty = _recursiveDirty = NO;
	}
    
	// recursively iterate over children
	if( _hasChildren )
		[_children makeObjectsPerformSelector:@selector(updateTransform)];
    
#if CC_SPRITE_DEBUG_DRAW
	// draw bounding box
	CGPoint vertices[4] = {
		ccp( _quad.bl.vertices.x, _quad.bl.vertices.y ),
		ccp( _quad.br.vertices.x, _quad.br.vertices.y ),
		ccp( _quad.tr.vertices.x, _quad.tr.vertices.y ),
		ccp( _quad.tl.vertices.x, _quad.tl.vertices.y ),
	};
	ccDrawPoly(vertices, 4, YES);
#endif // CC_SPRITE_DEBUG_DRAW
    
}

- (void)draw
{
	if (_isLocator)
	{
		return;
	}
	CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, @"CCSprite - draw");
    
	NSAssert(!_batchNode, @"If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");
    CC_NODE_DRAW_SETUP();
    
    if (self.useSeparateBlendFunc)
    {
        glBlendFuncSeparate(blendFuncSeparate.src, blendFuncSeparate.dst,
                            blendFuncSeparate.srcAlpha, blendFuncSeparate.dstAlpha);
    }
    else
    {
        ccGLBlendFunc(_blendFunc.src, _blendFunc.dst);
    }
    
    if (_blendEquation != -1)
    {
        glBlendEquation(_blendEquation);
    }
    
	ccGLBindTexture2D([_texture name]);
    
	//
	// Attributes
	//
    
	ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );
    [self setUniformsForFragmentShader];
    
	CHECK_GL_ERROR_DEBUG();
    
    // 3
#define kQuadSize sizeof(_quad.bl)
    long offset = (long)&_quad;
    
    // vertex
    NSInteger diff = offsetof( ccV3F_C4B_T2F, vertices);
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
    
	// texCoods
	diff = offsetof( ccV3F_C4B_T2F, texCoords);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
    
	// color
	diff = offsetof( ccV3F_C4B_T2F, colors);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
    
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
	CHECK_GL_ERROR_DEBUG();
    

    #if CC_SPRITE_DEBUG_DRAW == 1
	// draw bounding box
	CGPoint vertices[4]={
		ccp(_quad.tl.vertices.x,_quad.tl.vertices.y),
		ccp(_quad.bl.vertices.x,_quad.bl.vertices.y),
		ccp(_quad.br.vertices.x,_quad.br.vertices.y),
		ccp(_quad.tr.vertices.x,_quad.tr.vertices.y),
	};
	ccDrawPoly(vertices, 4, YES);
#elif CC_SPRITE_DEBUG_DRAW == 2
	// draw texture box
	CGSize s = self.textureRect.size;
	CGPoint offsetPix = self.offsetPosition;
	CGPoint vertices[4] = {
		ccp(offsetPix.x,offsetPix.y), ccp(offsetPix.x+s.width,offsetPix.y),
		ccp(offsetPix.x+s.width,offsetPix.y+s.height), ccp(offsetPix.x,offsetPix.y+s.height)
	};
	ccDrawPoly(vertices, 4, YES);
#endif // CC_SPRITE_DEBUG_DRAW
	CC_INCREMENT_GL_DRAWS(1);
    
	CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, @"CCSprite - draw");
}

#pragma mark -
#pragma mark Protected methods

- (void)setUniformsForFragmentShader
{
    // Can be implemented in subclasses
}

- (void)invalidateTransformCache
{
    _isTransformDirty = YES;
}

- (void)invalidateChildrenTranformCache
{
    for (CCNode *child in _children)
    {
        if ([child isKindOfClass:[GAFSprite class]])
        {
            [(GAFSprite *)child invalidateTransformCache];
        }
    }
}

@end
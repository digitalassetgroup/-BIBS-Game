 #define protected public
#include "sprite_nodes/CCSprite.h"
#undef protected

#include "GAFSprite.h"

#if CC_SPRITEBATCHNODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__A__) ( (int)(__A__))
#endif

GAFSprite::GAFSprite()
:
_useExternalTransform(false),
_useChildTransform(false),
_useSeparateBlendFunc(false),
_isLocator(false),
_blendEquation(-1),
_externalTransform(CCAffineTransformIdentity),
_childTransform(CCAffineTransformIdentity)
{
	
}

void GAFSprite::setExternaTransform(const CCAffineTransform& transform)
{
	if (!CCAffineTransformEqualToTransform(_externalTransform, transform))
	{
		_externalTransform = transform;
		_useExternalTransform = true;
		m_bTransformDirty = true;
		m_bInverseDirty = true;
	}
}

void GAFSprite::setChildTransform(const CCAffineTransform& transform)
{
	if (!CCAffineTransformEqualToTransform(_childTransform, transform))
	{
		_childTransform = transform;
		_useChildTransform = true;
		m_bTransformDirty = true;
		m_bInverseDirty = true;
	}	
}

CCAffineTransform GAFSprite::nodeToParentTransform(void)
{
	if (_useExternalTransform)
	{
		if (m_bTransformDirty)
		{
			m_sTransform = CCAffineTransformTranslate(_externalTransform, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
			m_bTransformDirty = false;
			GAFSprite * parent = dynamic_cast<GAFSprite*>(m_pParent);
			if (parent && parent->isUseChildTransform())
			{
				m_sTransform = CCAffineTransformConcat(m_sTransform, parent->childTransform());
			}
			m_bTransformDirty = false;
		}
		return m_sTransform;
		
	}
	else
	{
		return CCSprite::nodeToParentTransform();
	}
}

void GAFSprite::updateTransform(void)
{
	CCAssert( m_pobBatchNode, "updateTransform is only valid when CCSprite is being rendered using an CCSpriteBatchNode");
		
    // recalculate matrix only if it is dirty
    if( isDirty() ) {
		
        // If it is not visible, or one of its ancestors is not visible, then do nothing:
        if( !m_bVisible || ( m_pParent && m_pParent != (CCSprite*)m_pobBatchNode && ((CCSprite*)m_pParent)->m_bShouldBeHidden) )
        {
            m_sQuad.br.vertices = m_sQuad.tl.vertices = m_sQuad.tr.vertices = m_sQuad.bl.vertices = vertex3(0,0,0);
            m_bShouldBeHidden = true;
        }
        else
        {
            m_bShouldBeHidden = false;
			
            if( ! m_pParent || m_pParent == (CCSprite*)m_pobBatchNode )
            {
                m_transformToBatch = nodeToParentTransform();
            }
            else
            {
                CCAssert( dynamic_cast<CCSprite*>(m_pParent), "Logic error in CCSprite. Parent must be a CCSprite");
                m_transformToBatch = CCAffineTransformConcat( nodeToParentTransform() , ((CCSprite*)m_pParent)->m_transformToBatch );
            }
			
            //
            // calculate the Quad based on the Affine Matrix
            //
			
            CCSize size = m_obRect.size;
			
			// x1, x2, y1, y2 are changed (comapring to CCSprite) to place Sprite at center
            float x1 = m_obOffsetPosition.x - size.width / 2;
            float y1 = m_obOffsetPosition.y - size.height / 2;
			
            float x2 = x1 + size.width;
            float y2 = y1 + size.height;
            float x = m_transformToBatch.tx;
            float y = m_transformToBatch.ty;
			
            float cr = m_transformToBatch.a;
            float sr = m_transformToBatch.b;
            float cr2 = m_transformToBatch.d;
            float sr2 = -m_transformToBatch.c;
            float ax = x1 * cr - y1 * sr2 + x;
            float ay = x1 * sr + y1 * cr2 + y;
			
            float bx = x2 * cr - y1 * sr2 + x;
            float by = x2 * sr + y1 * cr2 + y;
			
            float cx = x2 * cr - y2 * sr2 + x;
            float cy = x2 * sr + y2 * cr2 + y;
			
            float dx = x1 * cr - y2 * sr2 + x;
            float dy = x1 * sr + y2 * cr2 + y;
			
            m_sQuad.bl.vertices = vertex3( RENDER_IN_SUBPIXEL(ax), RENDER_IN_SUBPIXEL(ay), m_fVertexZ );
            m_sQuad.br.vertices = vertex3( RENDER_IN_SUBPIXEL(bx), RENDER_IN_SUBPIXEL(by), m_fVertexZ );
            m_sQuad.tl.vertices = vertex3( RENDER_IN_SUBPIXEL(dx), RENDER_IN_SUBPIXEL(dy), m_fVertexZ );
            m_sQuad.tr.vertices = vertex3( RENDER_IN_SUBPIXEL(cx), RENDER_IN_SUBPIXEL(cy), m_fVertexZ );
        }
		
        // MARMALADE CHANGE: ADDED CHECK FOR NULL, TO PERMIT SPRITES WITH NO BATCH NODE / TEXTURE ATLAS
        if (m_pobTextureAtlas)
		{
            m_pobTextureAtlas->updateQuad(&m_sQuad, m_uAtlasIndex);
        }
		
        m_bRecursiveDirty = false;
        setDirty(false);
    }
	
    // MARMALADE CHANGED
    // recursively iterate over children
	/*    if( m_bHasChildren )
	 {
	 // MARMALADE: CHANGED TO USE CCNode*
	 // NOTE THAT WE HAVE ALSO DEFINED virtual CCNode::updateTransform()
	 arrayMakeObjectsPerformSelector(m_pChildren, updateTransform, CCSprite*);
	 }*/
    CCNode::updateTransform();
	
#if CC_SPRITE_DEBUG_DRAW
    // draw bounding box
    CCPoint vertices[4] = {
        ccp( m_sQuad.bl.vertices.x, m_sQuad.bl.vertices.y ),
        ccp( m_sQuad.br.vertices.x, m_sQuad.br.vertices.y ),
        ccp( m_sQuad.tr.vertices.x, m_sQuad.tr.vertices.y ),
        ccp( m_sQuad.tl.vertices.x, m_sQuad.tl.vertices.y ),
    };
    ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW
}

void GAFSprite::draw(void)
{
	if (_isLocator)
	{
		return;
	}
    CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "GAFSprite - draw");
	
    CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");
	
    CC_NODE_DRAW_SETUP();
	
    if (_useSeparateBlendFunc)
    {
        glBlendFuncSeparate(_blendFuncSeparate.src,      _blendFuncSeparate.dst,
                            _blendFuncSeparate.srcAlpha, _blendFuncSeparate.dstAlpha);
    }
    else
    {
        ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);
    }
    
    if (_blendEquation != -1)
    {
        glBlendEquation(_blendEquation);
    }
	
    if (m_pobTexture != NULL)
    {
        ccGLBindTexture2D( m_pobTexture->getName() );
    }
    else
    {
        ccGLBindTexture2D(0);
    }
    
    //
    // Attributes
    //
	
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );
	setUniformsForFragmentShader();
	CHECK_GL_ERROR_DEBUG();
	
#define kQuadSize sizeof(m_sQuad.bl)
    long offset = (long)&m_sQuad;
	
    // vertex
    int diff = offsetof( ccV3F_C4B_T2F, vertices);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
	
    // texCoods
    diff = offsetof( ccV3F_C4B_T2F, texCoords);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
	
    // color
    diff = offsetof( ccV3F_C4B_T2F, colors);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
	
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
    CHECK_GL_ERROR_DEBUG();	
	
    CC_INCREMENT_GL_DRAWS(1);
	
    CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "GAFSprite - draw");
}

void GAFSprite::invalidateTransformCache()
{
	m_bTransformDirty = true;
}

void GAFSprite::invalidateChildrenTranformCache()
{
	for (int i = 0; i < m_pChildren->count(); ++i)
	{
		GAFSprite * child = dynamic_cast<GAFSprite*>(m_pChildren->objectAtIndex(i));
		if (child)
		{
			child->invalidateTransformCache();
		}
	}
}

void GAFSprite::setUseExternalTransform(bool use)
{
	_useExternalTransform = use;
}

void GAFSprite::setUseChildTransform(bool use)
{
	_useChildTransform = use;
}

void GAFSprite::setUniformsForFragmentShader()
{
	
}
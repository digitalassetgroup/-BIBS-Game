#include "GAFStencilMaskSprite.h"
#include "GAFShaderManager.h"
#include "shaders/CCShaderCache.h"
#include "shaders/ccShaders.h"
#include <algorithm>

static const char * kPCStencilMaskAlphaFilterFragmentShaderFilename = "pcShader_PositionTexture_alphaFilter.fs";


static bool compare_stencil_sprites(const void* p1, const void* p2)
{
	GAFSprite* sp1 = (GAFSprite*)p1;
	GAFSprite* sp2 = (GAFSprite*)p2;
	
	return sp1->getZOrder() < sp2->getZOrder();
}

GAFStencilMaskSprite::GAFStencilMaskSprite()
:
_maskedObjects(NULL),
_isReorderMaskedObjectsDirty(false)
{
}

static std::map<CCNode *, GAFStencilMaskSprite *> _object2maskedContainer;

void GAFStencilMaskSprite::updateMaskContainerOf(CCNode * node)
{
	std::map<CCNode *, GAFStencilMaskSprite *>::iterator it = _object2maskedContainer.find(node);
	if (it != _object2maskedContainer.end())
	{
		it->second->invalidateMaskedObjectsOrder();
	}
}

GAFStencilMaskSprite::~GAFStencilMaskSprite()
{
	if (_maskedObjects)
	{
		for (int i = 0; i < _maskedObjects->count(); ++i)
		{
			CCNode * node = (CCNode*)_maskedObjects->objectAtIndex(i);
			std::map<CCNode *, GAFStencilMaskSprite *>::iterator it = _object2maskedContainer.find(node);
			if (it != _object2maskedContainer.end())
			{
				_object2maskedContainer.erase(it);
			}
		}
	}
	
	CC_SAFE_RELEASE(_maskedObjects);
}

bool GAFStencilMaskSprite::initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated)
{
	if (!GAFSprite::initWithTexture(pTexture, CCRectMake(0, 0, rect.size.width, rect.size.height), rotated))
	{
		return false;
	}
	CC_SAFE_RELEASE(_maskedObjects);
	_maskedObjects = new CCArray();
	return true;
}

void GAFStencilMaskSprite::draw()
{
	setShaderProgram(programShaderForMask());
	// Prepare stencil
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 1);
    
    // Draw mask
	GAFSprite::draw();
    
    // Use stencil
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1);
}

void GAFStencilMaskSprite::visit()
{
	GAFSprite::visit();
    
	sortAllMaskedObjects();
    // Draw subobjects, assuming mask and object are on the same layer
	for (int i = 0; i < _maskedObjects->count(); ++i)
	{
		CCNode * object = (CCNode *) _maskedObjects->objectAtIndex(i);
		object->visit();
	}
	glDisable(GL_STENCIL_TEST);
}

void GAFStencilMaskSprite::sortAllMaskedObjects()
{
	if (_isReorderMaskedObjectsDirty)
	{
		std::sort(_maskedObjects->data->arr,
				  _maskedObjects->data->arr + _maskedObjects->data->num,
				  compare_stencil_sprites);
		_isReorderMaskedObjectsDirty = false;
	}
}

CCGLProgram * GAFStencilMaskSprite::programShaderForMask()
{
	CCGLProgram *program = CCShaderCache::sharedShaderCache()->programForKey(kPCStencilMaskAlphaFilterProgramCacheKey);
	
    if (!program)
    {
        program = GAFShaderManager::createWithFragmentFilename(ccPositionTextureColor_vert, kPCStencilMaskAlphaFilterFragmentShaderFilename);
        if (program)
        {
			program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
			program->addAttribute(kCCAttributeNameColor,    kCCVertexAttrib_Color);
			program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            program->link();
			program->updateUniforms();
            CHECK_GL_ERROR_DEBUG();
			CCShaderCache::sharedShaderCache()->addProgram(program, kPCStencilMaskAlphaFilterProgramCacheKey);
            program->release();
        }
        else
        {
            CCLOGERROR("Cannot load program for programShaderForMask.");
            return NULL;
        }
    }
	
    program->use();
	return program;
}

void GAFStencilMaskSprite::addMaskedObject(CCNode * anObject)
{
	CCAssert(_object2maskedContainer.find(anObject) == _object2maskedContainer.end(), "CCNode may be only in one Masked container");
	_object2maskedContainer[anObject] = this;
	_maskedObjects->addObject(anObject);
	_isReorderMaskedObjectsDirty = true;
}

void GAFStencilMaskSprite::invalidateMaskedObjectsOrder()
{
	_isReorderMaskedObjectsDirty = true;
}




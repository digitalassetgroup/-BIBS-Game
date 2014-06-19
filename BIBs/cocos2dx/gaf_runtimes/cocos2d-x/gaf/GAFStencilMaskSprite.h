#pragma once

#ifndef __GAF_STENCIL_MASK_SPRITE__
#define __GAF_STENCIL_MASK_SPRITE__

#include "GAFSprite.h"



namespace cocos2d
{
	class CCArray;
}

#define  kPCStencilMaskAlphaFilterProgramCacheKey "kPCScrollLayerAlphaFilterProgramCacheKey"

using namespace cocos2d;

class GAFStencilMaskSprite : public GAFSprite
{
public:
	GAFStencilMaskSprite();
	~GAFStencilMaskSprite();
	virtual bool initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated);
	CCGLProgram * programShaderForMask();
	virtual void draw();
	virtual void visit();
	void addMaskedObject(CCNode * anObject);
	void invalidateMaskedObjectsOrder();
	
	static void updateMaskContainerOf(CCNode * node);
protected:
	void sortAllMaskedObjects();
private:
	CCArray * _maskedObjects;
	bool     _isReorderMaskedObjectsDirty;
};

#endif // __GAF_STENCIL_MASK_SPRITE__

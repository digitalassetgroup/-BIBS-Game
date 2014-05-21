#pragma once

#ifndef __GAF_STENCIL_MASK_SPRITE__
#define __GAF_STENCIL_MASK_SPRITE__

#include "GAFSprite.h"



namespace GAF
{
	
class GAFStencilMaskSprite : public GAFSprite
{
public:
	GAFStencilMaskSprite();
	static void updateMaskContainerOf(GAFSprite * node);	
	void addMaskedObject(GAFSprite * anObject);
	void initExternal();
	void getParameter(int parameter, void * result);
};
	
	
} // namespace GAF

#endif // __GAF_STENCIL_MASK_SPRITE__

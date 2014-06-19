#include "GAFStencilMaskSprite.h"
#include "GAFMarmalade.h"

namespace GAF
{
	GAFStencilMaskSprite::GAFStencilMaskSprite()
	{
		
	}
	
	void GAFStencilMaskSprite::initExternal()
	{
		setExternalPointer(GAF::GAFMarmaladeGFX::createStencilSprite(this));
	}
	
	void GAFStencilMaskSprite::addMaskedObject(GAFSprite * anObject)
	{
		GAF::GAFMarmaladeGFX::stencilSpriteAddMaskedObject(getExternalPointer(), anObject->getExternalPointer());
	}
	
	void GAFStencilMaskSprite::updateMaskContainerOf(GAFSprite * node)
	{
		if (!node)
		{
			return;
		}
		GAF::GAFMarmaladeGFX::stencilSpriteUpdateMaskContainerOf(node->getExternalPointer());
	}
	
	void GAFStencilMaskSprite::getParameter(int parameter, void * result)
	{
		return GAFSprite::getParameter(parameter, result);
	}
}

#include "GAFSprite.h"

namespace GAF
{


GAFSprite::GAFSprite()
:
_externalPointer(0)
{

}
	
void GAFSprite::initExternal()
{
	_externalPointer = GAF::GAFMarmaladeGFX::createSprite(this);
}
	
GAFSprite::~GAFSprite()
{
	if (_externalPointer)
	{
		GAF::GAFMarmaladeGFX::releaseSprite(_externalPointer);
		_externalPointer = NULL;
	}
}

bool GAFSprite::initWithTexture(void *pTexture, const CCRect& rect, bool rotated)
{
	if (_externalPointer)
	{
		GAF::GAFMarmaladeGFX::releaseSprite(_externalPointer);
		_externalPointer = NULL;
	}
	initExternal();
	return GAF::GAFMarmaladeGFX::initSprite(_externalPointer, pTexture, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, rotated);
}
	
void GAFSprite::setVisible(bool visible)
{
	GAFAssert(_externalPointer, "_externalPointer must be valid");
	GAF::GAFMarmaladeGFX::setSpriteVisible(_externalPointer, visible);
}

void GAFSprite::setExternaTransform(const CCAffineTransform& transform)
{
	GAFAssert(_externalPointer, "_externalPointer must be valid");
	GAF::GAFMarmaladeGFX::setSpriteTransform(_externalPointer, transform);
}
	
void GAFSprite::getParameter(int, void *)
{
	
}

} // namespace GAF
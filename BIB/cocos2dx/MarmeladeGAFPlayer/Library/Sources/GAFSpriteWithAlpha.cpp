#include "GAFSpriteWithAlpha.h"
#include "GAFShaderManager.h"
#include "GAFMarmalade.h"
#include "GAFShaderCache.h"
#include "GAFShaderProgram.h"
#include "GAFTexture.h"

namespace GAF
{
	
void GAFSpriteWithAlpha::initExternal()
{
	setExternalPointer(GAF::GAFMarmaladeGFX::createSprite(this));
}

void GAFSpriteWithAlpha::setColorTransform(const float * mults, const float * offsets)
{
	for (int i = 0; i < 4; ++i)
	{
		_colorTransform[i]     = mults[i];
		_colorTransform[i + 4] = offsets[i];
	}
	GAF::GAFMarmaladeGFX::setAlphaBlendingForSprite(getExternalPointer());
}

void GAFSpriteWithAlpha::setColorTransform(const float * colorTransform)
{
	for (int i = 0; i < 8; ++i)
	{
		_colorTransform[i] = colorTransform[i];
	}
	GAF::GAFMarmaladeGFX::setAlphaBlendingForSprite(getExternalPointer());
}
	
void GAFSpriteWithAlpha::setBlurRadius(const CCSize& blurRadius)
{
	if (_blurRadius.width != blurRadius.width || _blurRadius.height != blurRadius.height)
	{
		_blurRadius = blurRadius;
		GAF::GAFMarmaladeGFX::setSpriteBlurRadius(getExternalPointer(), blurRadius.width, blurRadius.height);
	}
}
	
void GAFSpriteWithAlpha::getParameter(int /*parameter*/, void * result)
{
	// paramter is not used now, but reserved for future needs.
	float * res = (float *) result;
	for (int i = 0; i < 8; ++i)
	{
		res[i] = _colorTransform[i];
	}
}

}
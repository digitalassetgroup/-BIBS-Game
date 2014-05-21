#pragma once

#ifndef __GAF_SPRITE_WITH_ALPHA__
#define __GAF_SPRITE_WITH_ALPHA__

#include "GAFSprite.h"
#include "cocoagaf/GAFGeometry.h"

namespace GAF
{

class CCTexture2D;
	
class GAFSpriteWithAlpha : public GAFSprite
{
public:
	void setColorTransform(const float * mults, const float * offsets);
/// color transform is float[8] 0-3 mults, 4-7 offsets
/// more details here - http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/flash/geom/ColorTransform.html
	const float * getColorTransform() const
	{
		return _colorTransform;
	}
	void initExternal();
	void setColorTransform(const float * colorTransform);
	void setBlurRadius(const CCSize& blurRadius);
	virtual void getParameter(int parameter, void * result);
private:
	CCSize   _blurRadius;
	float _colorTransform[8]; 
};
	
}

#endif // __GAF_SPRITE_WITH_ALPHA__

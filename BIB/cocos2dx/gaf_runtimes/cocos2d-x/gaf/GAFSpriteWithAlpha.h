#pragma once

#ifndef __GAF_SPRITE_WITH_ALPHA__
#define __GAF_SPRITE_WITH_ALPHA__

#include "GAFSprite.h"

using namespace cocos2d;

#define kGAFSpriteWithAlphaShaderProgramCacheKey "kGAFSpriteWithAlphaShaderProgramCache"
#define kGAFSpriteWithAlphaShaderProgramCacheKey  "kGAFSpriteWithAlphaShaderProgramCache"

class GAFSpriteWithAlpha : public GAFSprite
{
public:
	virtual void setUniformsForFragmentShader();
	virtual bool initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated);
	bool ensureShaderValid();
	virtual void draw();
	void setColorTransform(const GLfloat * mults, const GLfloat * offsets);
	const GLfloat * getColorTransform() const
	{
		return _colorTransform;
	}
	void setColorTransform(const GLfloat * colorTransform);
private:
	GLfloat _colorTransform[8]; // 0-3 mults, 4-7 offsets
    GLuint  _colorTrasformLocation;
};

#endif // __GAF_SPRITE_WITH_ALPHA__

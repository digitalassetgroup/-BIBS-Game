#pragma once

#ifndef __GAF_BLURRED_SPRITE___
#define __GAF_BLURRED_SPRITE___

#include "GAFSpriteWithAlpha.h"
#include "cocoa/CCObject.h"

namespace cocos2d
{
	class CCRenderTexture;
}
using namespace cocos2d;

#define kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey  "kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey"
#define kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey    "kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey"

class GAFBlurredSprite : public GAFSpriteWithAlpha
{
public:
	GAFBlurredSprite();
	~GAFBlurredSprite();
	void updateDynamicTexture();
	void setBlurSize(CCSize blurSize);
	bool initWithGAFSprite(GAFSpriteWithAlpha * aSprite, CCSize aBlurSize);
	bool initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated, CCSize aBlurSize);
	virtual bool initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated);
	inline GAFSpriteWithAlpha * sourceSprite()
	{
		return _sourceSprite;
	}
	inline CCSize blurSize() const
	{
		return _blurSize;
	}
	CCSize normalizedBlurSizeFromNeededBlurSize(CCSize aSize);
	virtual void draw();
	
protected:
	CCSize blurTextureSizeForOriginalSize(CCSize anOriginalSize);
	void createGaussianMatrixWithSize(CCSize aSize);
	void prepareDynamicsForSize(CCSize aSize);
private:
	
	// Shader uniforms
    GLint _vertShader_kernelSizeUniformLocation;
    GLint _vertShader_blurDotSizeUniformLocation;
    GLint _vertShader_kernelValuesUniformLocation;
    
    GLint _horzShader_kernelSizeUniformLocation;
    GLint _horzShader_blurDotSizeUniformLocation;
    GLint _horzShader_kernelValuesUniformLocation;
	
	CCGLProgram * programForVerticalBlur();
	CCGLProgram * programForHorizontalBlur();
	CCSize _originalTextureSize;
    CCSize _blurredTextureSize;
	
	CCRenderTexture *_originalTexture;
    CCRenderTexture *_dynamicTexture1;
    CCRenderTexture *_dynamicTexture2;
	
	float * _gaussianValuesX;
    float * _gaussianValuesY;
    CCSize _gaussianValuesArraySize;
	
	CCSize _blurSize;
	
	GAFSpriteWithAlpha * _sourceSprite;
};

#endif // __GAF_BLURRED_SPRITE___

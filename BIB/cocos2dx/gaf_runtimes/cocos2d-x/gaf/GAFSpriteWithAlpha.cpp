#include "GAFSpriteWithAlpha.h"
#include "GAFShaderManager.h"

#include "shaders/CCGLProgram.h"
#include "shaders/CCShaderCache.h"
#include "shaders/ccShaders.h"

static const char * kAlphaFragmentShaderFilename = "pcShader_PositionTextureAlpha_frag.fs";

bool GAFSpriteWithAlpha::initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated)
{
	if (GAFSprite::initWithTexture(pTexture, rect, rotated))
	{
		for (int i = 0; i < 4; ++i)
		{
			_colorTransform[i]     = 1.0f;
			_colorTransform[i + 4] = 0;
		}
		return ensureShaderValid();
	}
	else
	{
		return false;
	}
}

bool GAFSpriteWithAlpha::ensureShaderValid()
{
	CCGLProgram * program = CCShaderCache::sharedShaderCache()->programForKey(kGAFSpriteWithAlphaShaderProgramCacheKey);
	if (!program)
	{
		program = GAFShaderManager::createWithFragmentFilename(ccPositionTextureColor_vert, kAlphaFragmentShaderFilename);
		if (program)
		{
			program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
			program->addAttribute(kCCAttributeNameColor,    kCCVertexAttrib_Color);
			program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
			program->link();
			program->updateUniforms();
			CHECK_GL_ERROR_DEBUG();
			CCShaderCache::sharedShaderCache()->addProgram(program, kGAFSpriteWithAlphaShaderProgramCacheKey);
		}
		else
		{
			CCLOGERROR("Cannot load program for GAFSpriteWithAlpha.");
			CC_SAFE_DELETE(program);
			return false;
		}
	}
	setShaderProgram(program);
	program->use();
	_colorTrasformLocation = (GLuint)glGetUniformLocation(program->getProgram(), "colorTransform");
	if (_colorTrasformLocation <= 0)
	{
		CCLOGERROR("Cannot load program for GAFSpriteWithAlpha.");
		return false;
	}
	return true;
}

void GAFSpriteWithAlpha::draw()
{
	ensureShaderValid();
	GAFSprite::draw();
}

void GAFSpriteWithAlpha::setUniformsForFragmentShader()
{	
	glUniform4fv(_colorTrasformLocation, 2, _colorTransform);
}

void GAFSpriteWithAlpha::setColorTransform(const GLfloat * mults, const GLfloat * offsets)
{
	for (int i = 0; i < 4; ++i)
	{
		_colorTransform[i]     = mults[i];
		_colorTransform[i + 4] = offsets[i];
	}
}

void GAFSpriteWithAlpha::setColorTransform(const GLfloat * colorTransform)
{
	for (int i = 0; i < 8; ++i)
	{
		_colorTransform[i] = colorTransform[i];
	}
}

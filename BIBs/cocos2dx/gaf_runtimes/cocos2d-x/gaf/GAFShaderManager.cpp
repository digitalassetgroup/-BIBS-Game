#include "GAFShaderManager.h"
#include "GAFData.h"

#include "platform/CCFileUtils.h"
#include "shaders/CCGLProgram.h"

#if CC_ENABLE_CACHE_TEXTURE_DATA
	#include "shaders/CCShaderCache.h"
	#include "GAFSpriteWithAlpha.h"
	#include "GAFStencilMaskSprite.h"
	#include "GAFBlurredSprite.h"
#endif

CCGLProgram * GAFShaderManager::createWithFragmentFilename(const char * vertexSource, const char * fragmentFilename)
{
	if (!vertexSource || !fragmentFilename)
	{
		return NULL;
	}
	std::string fp = CCFileUtils::sharedFileUtils()->fullPathForFilename(fragmentFilename);
	GAFData data;
	data.delete_data = true;
	data.ptr = CCFileUtils::sharedFileUtils()->getFileData(fp.c_str(), "r", &data.size);
	if (!data.ptr)
	{
		CCLOGERROR("Cannot load fragment shader with name %s", fragmentFilename);
		return NULL;
	}
	CCGLProgram * res = new CCGLProgram();
	if (!res)
	{
		return NULL;
	}
	if (data.size)
	{
		data.ptr[data.size - 1] = 0;
	}
	
	if (!res->initWithVertexShaderByteArray(vertexSource, (const char *)data.ptr))
	{
		delete res;
		return NULL;
	}
	return res;
}

#if CC_ENABLE_CACHE_TEXTURE_DATA
static inline void _invalidate(const char * shader_key)
{
	CCGLProgram *program = CCShaderCache::sharedShaderCache()->programForKey(shader_key);
	if (program)
	{
		program->reset();	
	}
}
#endif

void GAFShaderManager::handleEnterBackground()
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
	_invalidate(kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey);
	_invalidate(kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey);
	_invalidate(kPCStencilMaskAlphaFilterProgramCacheKey);
	_invalidate(kGAFSpriteWithAlphaShaderProgramCacheKey);
	CCShaderCache::sharedShaderCache()->purgeSharedShaderCache();
#endif
}

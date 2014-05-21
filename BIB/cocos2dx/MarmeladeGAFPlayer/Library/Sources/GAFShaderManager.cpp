#include "GAFShaderManager.h"
#include "GAFData.h"

#include "platform/CCFileUtils.h"
#include "shaders/CCGLProgram.h"

#if CC_ENABLE_CACHE_TEXTURE_DATA
	#include "shaders/CCShaderCache.h"
	#include "GAFSpriteWithAlpha.h"
	#include "GAFStencilMaskSprite.h"
#endif

namespace GAF
{
	
CCGLProgram * GAFShaderManager::createWithFragmentFilename(const char * vertexSource, const char * fragmentFilename, CCGLProgram * p)
{
	if (!vertexSource || !fragmentFilename)
	{
		return NULL;
	}
	std::string fp = CCFileUtils::sharedFileUtils()->fullPathForFilename(fragmentFilename);
	GAF::GAFData data;
	data.setDeleteData(true);
	unsigned long size;
	data.setBytes(CCFileUtils::sharedFileUtils()->getFileData(fp.c_str(), "r", &size));
	data.setSize(size);
	if (!data.bytes())
	{
		CCLOGERROR("Cannot load fragment shader with name %s", fragmentFilename);
		return NULL;
	}
	CCGLProgram * res;
	
	if (p)
	{
		res = p;
	}
	else
	{
		res = new CCGLProgram();
	}
	
	if (!res)
	{
		return NULL;
	}
	if (data.size())
	{
		data.bytes()[data.size() - 1] = 0;
	}
	
	if (!res->initWithVertexShaderByteArray(vertexSource, (const char *)data.bytes()))
	{
		CC_SAFE_RELEASE(res);
		return NULL;
	}
	if (!p)
	{
		res->autorelease();
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
	
} // namespace GAF





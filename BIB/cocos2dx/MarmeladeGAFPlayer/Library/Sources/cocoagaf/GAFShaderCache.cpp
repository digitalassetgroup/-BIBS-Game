#if 0

#include "GAFShaderCache.h"
#include "GAFDictionary.h"
#include "GAFShaderProgram.h"

namespace GAF
{
	
static CCShaderCache *_sharedShaderCache = 0;
	
CCShaderCache::CCShaderCache()
{
	_shaders = new CCDictionary();
}
	
CCShaderCache::~CCShaderCache()
{
	CC_SAFE_RELEASE(_shaders);
}
	
CCShaderCache* CCShaderCache::sharedShaderCache()
{
	if (!_sharedShaderCache)
	{
        _sharedShaderCache = new CCShaderCache();
    }
    return _sharedShaderCache;
}

CCGLProgram * CCShaderCache::programForKey(const char* key)
{
	return (CCGLProgram *)_shaders->objectForKey(key);
}
	
void CCShaderCache::addProgram(CCGLProgram* program, const char* key)
{
	_shaders->setObject(program, key);
}
	
}

#endif
#pragma once

#ifndef __GAF_SHADER_MANAGER__
#define __GAF_SHADER_MANAGER__

namespace cocos2d
{
	class CCGLProgram;
}

using namespace cocos2d;

namespace GAF
{

class GAFShaderManager
{
public:
	static CCGLProgram * createWithFragmentFilename(const char * vertexSource, const char * fragmentFilename, CCGLProgram * p = 0);
	static void handleEnterBackground();

}; // GAFShaderManager
	
} // namespace GAF

#endif // __GAF_SHADER_MANAGER__
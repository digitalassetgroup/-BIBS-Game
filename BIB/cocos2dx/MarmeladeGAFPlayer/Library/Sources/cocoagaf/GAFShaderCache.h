#pragma once

#ifndef __GAF_SHADER__CACHE___
#define __GAF_SHADER__CACHE___

#if 0

#include "GAFObject.h"
#include <string>

namespace GAF
{
	class CCDictionary;
	class CCGLProgram;
	
	class CCShaderCache : public CCObject
	{
	public:
		static CCShaderCache* sharedShaderCache();
		CCShaderCache();
		~CCShaderCache();
		CCGLProgram * programForKey(const char* key);
		void addProgram(CCGLProgram* program, const char* key);
	private:
		CCDictionary * _shaders;
	};
}

#endif

#endif // __GAF_COCOA_CCSTRING___

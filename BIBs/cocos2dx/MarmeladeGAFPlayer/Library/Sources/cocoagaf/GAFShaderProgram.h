
#pragma once

#ifndef __GAF_SHADER_PROGRAM___
#define __GAF_SHADER_PROGRAM___

// note at this momemet there is no notion of shader in internals
// of GAF player, but first implementation used it and files are kept
//#include "GAFObject.h"
#include <string>

namespace GAF
{
	enum GAFGLProgramType
	{
		CCGLPROGRAM_ALPHA_SPRITE_SHADER
	};
#if 0	
	class CCGLProgram : public CCObject
	{
	public:
		CCGLProgram();
		~CCGLProgram();
		static CCGLProgram * create(GAFGLProgramType type);
		void * externalPointer()
		{
			return _externalPointer;
		}
	private:
		void * _externalPointer;
	};
#endif	
}

#endif // __GAF_SHADER_PROGRAM___

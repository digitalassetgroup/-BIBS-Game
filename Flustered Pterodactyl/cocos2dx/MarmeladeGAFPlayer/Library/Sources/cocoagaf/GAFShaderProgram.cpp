#if 0

#include "GAFShaderProgram.h"
#include "GAFMarmalade.h"

namespace GAF
{

CCGLProgram::CCGLProgram()
	:
_externalPointer(NULL)
{
}
	
CCGLProgram::~CCGLProgram()
{
	if (_externalPointer)
	{
		GAF::releaseShader(_externalPointer);
	}
}
	
CCGLProgram * CCGLProgram::create(GAFGLProgramType type)
{
	CCGLProgram* pRet = new CCGLProgram();
	pRet->_externalPointer = GAF::createShader(type);
	if (pRet->_externalPointer)
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = 0;
	}    
    return pRet;
}
	
}

#endif
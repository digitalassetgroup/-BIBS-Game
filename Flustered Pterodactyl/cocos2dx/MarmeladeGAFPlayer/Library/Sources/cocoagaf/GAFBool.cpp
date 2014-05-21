#include "GAFBool.h"

namespace GAF
{
	
CCBool * CCBool::create(bool v)
{
	CCBool* pRet = new CCBool(v);
    pRet->autorelease();
    return pRet;
}
	
}
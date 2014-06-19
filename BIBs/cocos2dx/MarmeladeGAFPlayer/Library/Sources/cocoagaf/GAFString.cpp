#include "GAFString.h"

namespace GAF
{

CCString * CCString::create(const char * str)
{
	CCString* pRet = new CCString(str);
    pRet->autorelease();
    return pRet;
}

CCString * CCString::create(const std::string & str)
{
	CCString* pRet = new CCString(str);
    pRet->autorelease();
    return pRet;
}
	
}
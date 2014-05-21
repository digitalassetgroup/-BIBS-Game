#include "GAFInteger.h"

namespace GAF
{
	
CCInteger * CCInteger::create(int v)
{
	CCInteger* pRet = new CCInteger(v);
    pRet->autorelease();
    return pRet;
}

}
#include "GAFFilterData.h"

GAFBlurFilterData * GAFBlurFilterData::create(int bX, int bY)
{
	GAFBlurFilterData * ret = new GAFBlurFilterData();
	if (ret)
	{
		ret->blurX = bX;
		ret->blurY = bY;
		ret->autorelease();
		return ret;
	}
	return NULL;
}

GAFBlurFilterData::GAFBlurFilterData()
{
	
}

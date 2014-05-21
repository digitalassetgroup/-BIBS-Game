#include "GAFTexture.h"
#include "GAFMarmalade.h"
#include "GAFImage.h"

namespace GAF
{
	CCTexture2D::CCTexture2D()
	:
	_externalPointer(NULL)
	{
	}
	
	CCTexture2D::~CCTexture2D()
	{
		if (_externalPointer)
		{
			GAF::GAFMarmaladeGFX::releaseTexture(_externalPointer);
			_externalPointer = NULL;
		}
	}
	
	CCTexture2D * CCTexture2D::create()
	{
		CCTexture2D* pRet = new CCTexture2D();
		pRet->autorelease();
		return pRet;
	}
	
	bool CCTexture2D::initWithImage(CCImage * image)
	{
		if (_externalPointer)
		{
			GAF::GAFMarmaladeGFX::releaseTexture(_externalPointer);
			_externalPointer = NULL;
		}
		_externalPointer = GAF::GAFMarmaladeGFX::createTexture(image->getExternalPointer());
		return _externalPointer;
	}
	
}
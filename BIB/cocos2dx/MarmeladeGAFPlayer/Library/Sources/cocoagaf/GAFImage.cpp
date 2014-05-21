#include "GAFImage.h"
#include "GAFMarmalade.h"

namespace GAF
{
	
CCImage::CCImage()
:
_externalPointer(NULL)
{
	
}

CCImage::~CCImage()
{
	if (_externalPointer)
	{
		GAF::GAFMarmaladeGFX::releaseImage(_externalPointer);
		_externalPointer = NULL;
	}
}
	
CCImage * CCImage::create()
{
	CCImage* pRet = new CCImage();
	pRet->autorelease();
	return pRet;
}
	
bool CCImage::initWithImageFile(const char * path)
{
	if (_externalPointer)
	{
		GAF::GAFMarmaladeGFX::releaseImage(_externalPointer);
		_externalPointer = NULL;
	}
	_externalPointer = GAF::GAFMarmaladeGFX::createImage(path);
	
	return _externalPointer;
}
	
}
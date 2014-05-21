#include "GAFSpriteFrame.h"
#include "GAFMarmalade.h"
#include "GAFImage.h"
#include "GAFTexture.h"

namespace GAF
{
	CCSpriteFrame::CCSpriteFrame()
	:
	_externalPointer(NULL)
	{
	}
	
	CCSpriteFrame::~CCSpriteFrame()
	{
		if (_externalPointer)
		{
			GAF::GAFMarmaladeGFX::releaseSpriteFrame(_externalPointer);
			_externalPointer = NULL;
		}
	}
	
	CCSpriteFrame * CCSpriteFrame::createWithTexture(CCTexture2D* pobTexture, const CCRect& rect)
	{
		CCSpriteFrame* pRet = new CCSpriteFrame();
		pRet->_externalPointer = GAF::GAFMarmaladeGFX::createSpriteFrame(pobTexture->getExternalPointer(), rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
		pRet->autorelease();
		return pRet;
	}
	
}
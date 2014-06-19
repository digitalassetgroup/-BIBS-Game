#include "ui_popup_box.h"
#include "ig_interface.h"
#include "../utilities/ig_utilities.h"

using namespace cocos2d;

namespace ig_interface
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	PopupBox::PopupBox(CCNode *image) :
		m_boxImage(image)
	{
		addChild(m_boxImage);
		autorelease();
		
		auto sizeInPixels = CCSize(m_boxImage->boundingBox().size.width+2, 
										m_boxImage->boundingBox().size.height+2);
		this->setContentSize(sizeInPixels);
		//m_tContentSizeInPixels = sizeInPixels;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	PopupBox::~PopupBox(void)
	{
		//May want to make sure it is cleaned properly, this causes a crash, perahps a double deletion crash.
		//delete m_boxImage;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	PopupBox* PopupBox::makeSmallBox(void)
	{
		const CCPoint centerScreen(gScreenSize.width/2, gScreenSize.height/2);
		//CCNode* boxImage = ig_interface::g_uiSpriteLoader->spriteWithUniqueName(TEXTURES::IMG_COMMON_UI_SMALL_POPUP_BOX, ccp(0.0f, 0.0f));
		PopupBox* newBox(new PopupBox(NULL/*boxImage*/));
		newBox->setPosition(centerScreen);
		return newBox;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	PopupBox* PopupBox::makeMediumBox(void)
	{
		const CCPoint centerScreen(gScreenSize.width/2, gScreenSize.height/2);
		//CCNode* boxImage = g_uiSpriteLoader->spriteWithUniqueName(TEXTURES::IMG_COMMON_UI_MEDIUM_POPUP_BOX, ccp(0.0f, 0.0f));
		PopupBox* newBox(new PopupBox(NULL));
		newBox->setPosition(centerScreen);
		return newBox;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	PopupBox* PopupBox::makeLargeBox(void)
	{
		const CCPoint centerScreen(gScreenSize.width/2, gScreenSize.height/2);
		//CCNode* boxImage = g_uiSpriteLoader->spriteWithUniqueName(TEXTURES::IMG_COMMON_UI_LARGE_POPUP_BOX, ccp(0.0f, 0.0f));
		PopupBox* newBox(new PopupBox(NULL));
		newBox->setPosition(centerScreen);
		return newBox;
	}	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}; /* namespace ig_interface */

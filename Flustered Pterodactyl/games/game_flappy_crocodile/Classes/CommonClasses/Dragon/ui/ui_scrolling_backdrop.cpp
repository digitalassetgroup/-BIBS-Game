#include "ui_scrolling_backdrop.h"
#include "ig_interface.h"

#include "../utilities/ig_utilities.h"
//#include "../cannonboy.h"	/* for the ability to read gScreenSize */

namespace ig_interface
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingBackdrop::ScrollingBackdrop(CCNode* image, CCNode* image2) :		
		m_scrollOffsetY(0.0f)
	{
        m_backdropImage[0] = image;
        m_backdropImage[1] = image2;
        
		cocos2d::CCPoint anchor(0.5f, 0.0f);
		m_backdropImage[0]->setAnchorPoint(anchor);
		m_backdropImage[1]->setAnchorPoint(anchor);
		
		addChild(m_backdropImage[0]);
		addChild(m_backdropImage[1]);
		autorelease();
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingBackdrop::~ScrollingBackdrop(void)
	{

	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingBackdrop* ScrollingBackdrop::makeBackdrop(const std::string& shSheetName, const std::string& imageName)
	{
		//SpriteHelperLoader shl(shSheetName);
		return makeBackdrop(imageName);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingBackdrop* ScrollingBackdrop::makeBackdrop(const std::string& imageName)
	{
		const cocos2d::CCPoint atCenter(gScreenSize.width/2, gScreenSize.height/2);
		/*
		cocos2d::CCNode* image = shl->spriteWithUniqueName(imageName, atCenter);		
		if (nullptr == image)
		{
			assert_if(true, "Scrolling Backdrop image not found.");
			return nullptr;
		}
		
		return new ScrollingBackdrop(image, shl->spriteWithUniqueName(imageName, atCenter));
		*/
		return NULL;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingBackdrop* ScrollingBackdrop::makeDebugGridBackdrop(void)
	{
		return makeBackdrop("a_test_sheet_debug.pshs", "a_test_sheet_debug");
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingBackdrop::scrollBy(const float scrollAmount)
	{
		m_scrollOffsetY += scrollAmount;
		
		/* Tile the background image as it scrolls along. */
		const float backdropHeight(m_backdropImage[0]->boundingBox().size.height);		
		assert_if(abs(scrollAmount) > backdropHeight, "Attempting to scroll too fast for the backdrop to keep up.");
		
		const float anchorOffset(backdropHeight * 0.5f - (backdropHeight * m_backdropImage[0]->getAnchorPoint().y));
		if (m_scrollOffsetY + anchorOffset > 0.0f)
			m_scrollOffsetY -= backdropHeight;
		if (m_scrollOffsetY + anchorOffset < -backdropHeight)
			m_scrollOffsetY += backdropHeight;
		
		cocos2d::CCPoint backdropPos(gScreenSize.width/2, gScreenSize.height/2 + m_scrollOffsetY);
		
		m_backdropImage[0]->setPosition(backdropPos); /* Bottom backdrop tile */
		backdropPos.y += backdropHeight - 1.0f;
		m_backdropImage[1]->setPosition(backdropPos); /* Top backdrop tile */
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingBackdrop::flipX(void)
	{
		m_backdropImage[0]->setScaleX(-m_backdropImage[0]->getScaleX());
		m_backdropImage[1]->setScaleX(-m_backdropImage[1]->getScaleX());
	}
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}; /* namespace ig_interface */

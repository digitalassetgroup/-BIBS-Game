#include "ui_toggle_button.h"
#include "ig_interface.h"

#include "../utilities/ig_constants.h"

using namespace cocos2d;
using namespace DracoGames::Constants;

namespace ig_interface
{
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ToggleButton::ToggleButton(const std::string& activeSprite, const std::string& inactiveSprite,
							   IBtnDelegate* const handle, const std::string& labelText) :
		Button(),
		m_activeSprite(nullptr),
		m_inactiveSprite(nullptr),
		m_isToggleActive(true),
		m_activeAlwaysVisible(true)
	{
		InitWithTextureNames(handle, labelText, activeSprite, "");

		m_activeSprite = m_upSprite;
		
		//m_inactiveSprite = spriteLoader->spriteWithUniqueName(inactiveSprite, OriginPoint);
		assert_if(nullptr == m_inactiveSprite, "A sprite named %s does not exist on provided sprite sheet.\n", inactiveSprite.c_str());
		addChild(m_inactiveSprite);
		
		setToggleActive(m_isToggleActive);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ToggleButton::~ToggleButton(void)
	{		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ToggleButton* ToggleButton::makeToggleButton(IBtnDelegate* const handle, const std::string& activeSprite, 
										  const std::string& inactiveSprite,  
										  const std::string& labelText)
	{
		return new ToggleButton(activeSprite, inactiveSprite, handle, labelText);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	void ToggleButton::setToggleActive(const bool isActive)
	{
		m_isToggleActive = isActive;
		if(false == m_activeAlwaysVisible)
		{
			m_activeSprite->setVisible(m_isToggleActive);
		}
		
		m_inactiveSprite->setVisible(!m_isToggleActive);
		
		/* The following is needed because the base Button will call setVisible(true) on upSprite... */
		if(true == m_isToggleActive)
		{
			m_upSprite = m_activeSprite;
		}
		else
		{
			m_upSprite = m_inactiveSprite;
		}		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	void ToggleButton::toggleActive(void)
	{
		setToggleActive(!m_isToggleActive);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
		
	void ToggleButton::setOpactity(GLubyte opacity)
	{
		auto activeSprite = dynamic_cast<CCSprite*>(m_activeSprite);
		if (activeSprite != NULL)
			activeSprite->setOpacity(opacity);

		auto inactiveSprite = dynamic_cast<CCSprite*>(m_inactiveSprite);
		if (inactiveSprite != NULL)
			inactiveSprite->setOpacity(opacity);

		Button::setOpactity(opacity);
	}	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	void ToggleButton::onClicked(void)
	{
		Button::onClicked();		
		toggleActive();
	}	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
}; /* namespace ig_interface */

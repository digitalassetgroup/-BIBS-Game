#pragma once
#ifndef DRACO_GAMES_UI_TOGGLE_BUTTON_H
#define DRACO_GAMES_UI_TOGGLE_BUTTON_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ui_button.h"

namespace ig_interface
{
	
	class ToggleButton : public Button
	{
	public:

		virtual ~ToggleButton(void);
    	
		static ToggleButton* makeToggleButton(IBtnDelegate* const handle, const std::string& activeSprite, 
										const std::string& inactiveSprite,  const std::string& labelText = "");
		
		/**
		 *	Called whenever a touch is dispatched to the button.  began, moved or ended.
		 */
//		virtual void onTouch(const TouchEvent& evt);
		
		void setToggleActive(const bool isActive);
		void toggleActive(void);
		bool isToggleActive(void) const { return m_isToggleActive; }
		
		virtual void setOpactity(GLubyte opacity);
		
	protected:
		virtual void onClicked(void);		
		
		cocos2d::CCNode* m_activeSprite;		
		cocos2d::CCNode* m_inactiveSprite;
		
	private:
		/* Prevent accidental use: If you need to create a button, use the static factory methods above "Make" */
		ToggleButton(const std::string& upName, const std::string& downName,
			   IBtnDelegate* const handle = nullptr, const std::string& labelText = "");
		ToggleButton(const ToggleButton &obj);
		ToggleButton& operator=(const ToggleButton &obj);		

		bool m_isToggleActive;	//True for the buttons Active state, False for the buttons Inactive state.
		bool m_activeAlwaysVisible;
	};
	
}; /* namespace ig_interface */

#endif 

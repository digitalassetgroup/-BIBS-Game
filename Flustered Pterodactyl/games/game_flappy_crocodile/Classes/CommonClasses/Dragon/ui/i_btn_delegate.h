#pragma once
#ifndef SUPER_KID_CANNON_I_BTN_DELEGATE_H
#define SUPER_KID_CANNON_I_BTN_DELEGATE_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ig_interface
{
	struct TouchEvent;
	class Button; /* defined in ui/ui_button.h */

	class IBtnDelegate
	{
	public:
		virtual ~IBtnDelegate(void)
		{}

		/**
		 *	Override OnTouch() to handle special effects with the button like hovering, finger over/off, etc.
		 */
		virtual void onTouch(Button* const target, const TouchEvent& evt)
		{}

		/*
		 *	Override and make use of the OnClick function to handle when the user pressed and released a button, 
		 *	ie- when the button should be doing something.
		 */
		virtual void onClickAction(Button* const target) = 0;
	};

}; /* namespace ig_interface */

#endif /* SUPER_KID_CANNON_I_BTN_DELEGATE_H */

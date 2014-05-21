#pragma once
#ifndef _cannonboy_ui_popup_box_h_
#define _cannonboy_ui_popup_box_h_ "$Rev: 518 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include "../objects/obj_base.h"
#include "cocos2d.h"
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ig_interface
{
	
	class PopupBox : public cocos2d::CCNode
	{
	public:
		virtual ~PopupBox(void);

		/**
		 *	Creates a popup box that resembles a quick "yes" / "no" question or very brief information with "accept."
		 *	@note the PopupBox is autoreleased when the scene dies; clean up after hanging pointers, but do not delete.		 
		 */
		static PopupBox* makeSmallBox(void);
		
		/**
		 *	Creates a popup box that contains more information than the small version."
		 *	@note the PopupBox is autoreleased when the scene dies; clean up after hanging pointers, but do not delete.
		 */		
		static PopupBox* makeMediumBox(void);
		
		/**
		 *	Creates a popup box that can contain a large amount of information.
		 *	@note the PopupBox is autoreleased when the scene dies; clean up after hanging pointers, but do not delete.
		 */		
		static PopupBox* makeLargeBox(void);
		
	private:
		/* Prevent accidental use.  To Create a PopupBox use the static Factory methods. */
		PopupBox(CCNode *image);
		PopupBox(const PopupBox& obj);
		PopupBox& operator = (const PopupBox& obj);
		
		CCNode* m_boxImage;
	};
	
}; /* namespace ig_interface */

#endif /* _cannonboy_ui_popup_box_h_ */

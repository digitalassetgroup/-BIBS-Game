#pragma once
#ifndef _cannonboy_ui_scrolling_backdrop_h_
#define _cannonboy_ui_scrolling_backdrop_h_ "$Rev: 518 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cocos2d.h"
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ig_interface
{
	
	class ScrollingBackdrop : public cocos2d::CCNode
	{
	public:
		virtual ~ScrollingBackdrop(void);

		/**
		 *	Create a scrolling backdrop by calling this factor method.  The backdrop will clean up after itself much
		 *	like the ig_interface::Label.
		 *
		 *	@param sheetName is the name of the Sprite Helper scene that the scrolling backdrop image exists on.
		 *	@param imageName is the name of the image to use as the scrolling image.
		 *
		 *	@note this is not as efficient as using an already loaded sprite helper scene as a temporary scene will be
		 *	loaded and destroyed within.
		 */
		static ScrollingBackdrop* makeBackdrop(const std::string& sheetName, const std::string& imageName);

		/**
		 *	Create a scrolling backdrop by calling this factor method.  The backdrop will clean up after itself much
		 *	like the ig_interface::Label.
		 *
		 *	@param loader is a pointer to an already existing sprite helper scene.
		 *	@param imageName is the name of the image to use as the scrolling image.
		 */		
		static ScrollingBackdrop* makeBackdrop(const std::string& imageName);
		
		/**
		 *	Creates a grid overlay for debugging screen coordinates.
		 *	@note this should only be called in developer builds.
		 */
		static ScrollingBackdrop* makeDebugGridBackdrop(void);
		
		/**
		 *	Move the backdrop by the amount specified and tile as neccessary.
		 *	
		 *	@param scrollAmount is the amount to scroll by.  Absolute value cannot exceed backdrop width or height.
		 */
		void scrollBy(const float scrollAmount);
		
		/**
		 *	Flips the backdrop images horizontally by setting scaleX to -scaleX.
		 *
		 *	@note this sort of hackish for the level results screen flipping squiggly horizontally.
		 */
		void flipX(void);
		
	private:
		/* Prevent accidental use.  To Create a scrolling backdrop use the static Factory methods. */
		ScrollingBackdrop(CCNode* image, CCNode* image2);
		ScrollingBackdrop(const ScrollingBackdrop& obj);
		ScrollingBackdrop& operator=(const ScrollingBackdrop& obj);
		
		CCNode* m_backdropImage[2];
		float m_scrollOffsetY;
	};
	
}; /* namespace ig_interface */

#endif /* _cannonboy_ui_scrolling_backdrop_h_ */

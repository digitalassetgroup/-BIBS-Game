#pragma once
#ifndef DRACOC_GAMES_IG_INTERFACE_H
#define DRACOC_GAMES_IG_INTERFACE_H 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "ui_button.h"
#include "ui_toggle_button.h"
#include "ui_label.h"
#include "ui_counting_label.h"
#include "ui_popup_box.h"
#include "ui_scrolling_backdrop.h"
#include "ui_scrolling_sprite_field.h"
#include "ui_scrolling_list.h"
#include "ui_fullscreen_quad.h"
//#include "ui_system_dialog.h"

#include "i_btn_delegate.h"

#include "cocos2d.h"

#include "../effects/node_effect_button_slide_in.h"		/* uiElement slideIn / slideOut depends on this. */

namespace ig_interface
{
	typedef cocos2d::CCNode uiElement;
	
	//extern const SpriteHelperLoader const* g_uiSpriteLoader;	/* Setup in AppDelegate.cpp */
	static const cocos2d::CCPoint ZeroVec2(0.0f, 0.0f);			/* Used primarily as a default value. */
	
	float sideEdgeSpacing(void);
	float bottomEdgeSpacing(void);
	float topEdgeSpacing(void);
	
	float leftOfScreen(const bool uiEdge = false);
	float rightOfScreen(const bool uiEdge = false);
	float topOfScreen(const bool uiEdge = false);
	float bottomOfScreen(const bool uiEdge = false);
	
	/**
	 *	Computes the centerOfTheScreen horizontally based on the leftEdge and rightEdge.
	 *	@note This does assume the left edge will always be less than the right edge.
	 */
	float centerOfScreenX(void);
	
	/**
	 *	Computes the centerOfTheScreen vertically based on the bottomEdge and topEdge.
	 *	@note this does assume the bottomEdge will always be less than the top edge.
	 */
	float centerOfScreenY(void);

	/**
	 *	Get a point at the center of the screen.
	 */
	cocos2d::CCPoint centerOfScreen(void);
	
	
	/*
	 *	Docking Functions:
	 *
	 *	The following functions will place an interface element to the edge of the screen, accounting for the padding
	 *	that DracoGames desires.  Use of these functions, with a small offset if needed, is greatly encouraged over
	 *	setPosition.  Most ui sprites, buttons and labels should be using these docking functions.
	 *
	 *	@note converting/scaling of offsets for the different devices is AUTOMAGICALLY handled in the docking
	 *	functions.  DO NOT scale the offsetX or offsetY variables that are sent it or things will look odd.
	 *
	 *	@note the docking functions assume that the position of the node is placed by the center point.
	 */

	void dockToCenterCenter(uiElement& nodeToDock, const float offsetX = 0.0f, const float offsetY = 0.0f);
	void dockToBottomLeft(uiElement& nodeToDock, const float offsetX = 0.0f, const float offsetY = 0.0f, const bool uiEdge = true);
	void dockToBottomRight(uiElement& nodeToDock, const float offsetX = 0.0f, const float offsetY = 0.0f, const bool uiEdge = true);
	void dockToBottomCenter(uiElement& nodeToDock, const float offsetX = 0.0f, const float offsetY = 0.0f, const bool uiEdge = true);
	void dockToTopLeft(uiElement& nodeToDock, const float offsetX = 0.0f, const float offsetY = 0.0f, const bool uiEdge = true);
	void dockToTopRight(uiElement& nodeToDock, const float offsetX = 0.0f, const float offsetY = 0.0f, const bool uiEdge = true);
	void dockToTopCenter(uiElement& nodeToDock, const float offsetX = 0.0f, const float offsetY = 0.0f, const bool uiEdge = true);
	
	
	/*
	 *	Alignment Functions:
	 *
	 *	@note stationaryNode should be const.
	 *
 	 *	@note the docking functions assume that the position of the node is placed by the center point.
	 */
	
	void alignLeftEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingX = 0.0f);
	void alignRightEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingX = 0.0f);
	void alignHorizontalCenters(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingX = 0.0f);	
	void alignTopEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingY = 0.0f);
	void alignBottomEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingY = 0.0f);
	void alignVerticalCenters(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingY = 0.0f);
	
	/*
	 *	SlideIn / SlideOut
	 */
	
	/**
	 *	Start an effect on the node to slide from a direction to it's current position.  The button will POP to 
	 *	a new position based on the NodeEffectButtonSlideIn to keep all buttons consistent!
	 */
	void slideInFromDirection(uiElement& nodeToSlide, const ButtonSlideDirection& fromDirection, const float startDelay);
	void slideOutToDirection(uiElement& nodeToSlide, const ButtonSlideDirection& towardDirection, const float startDelay);
	
	void slideInFromDirection(uiElement& nodeToSlide, const ButtonSlideDirection& fromDirection, const int elementIndex);
	void slideOutToDirection(uiElement& nodeToSlide, const ButtonSlideDirection& towardDirection, const int elementIndex);
	
}; /* namespace ig_interface */

#endif 

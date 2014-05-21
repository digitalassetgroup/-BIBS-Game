#include "ig_interface.h"
//#include "../../cannonboy.h"	/* We depend on some constants like gScreenSize - currently. */
#include "../effects/effect_manager.h"
#include "../utilities/ig_utilities.h"

namespace ig_interface
{
	static bool isEyePad(void) 
	{ 
		return false; //return LHSettings::sharedInstance()->isIpad(); 
	}
	
	/* Alex wanted the iPod to have less space around the border so the buttons are closer to the edge: 9/20/12 */
	float sideEdgeSpacing(void) { return (isEyePad()) ? 34.0f * g_convertRatio->x : 2.0f * g_convertRatio->x; }
	float bottomEdgeSpacing(void) { return (isEyePad()) ? 30.0f * g_convertRatio->y : 8.0f * g_convertRatio->y; }
	float topEdgeSpacing(void) { return (isEyePad()) ? 30.0f * g_convertRatio->y : 8.0f * g_convertRatio->y; }	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float leftOfScreen(const bool uiEdge)
	{
		if(true == uiEdge)
			return sideEdgeSpacing();
		return 0.0f;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float rightOfScreen(const bool uiEdge)
	{
		if(true == uiEdge)
			return gScreenSize.width - sideEdgeSpacing();
		return gScreenSize.width;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float topOfScreen(const bool uiEdge)
	{
		if(true == uiEdge)
			return gScreenSize.height - topEdgeSpacing();
		return gScreenSize.height;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float bottomOfScreen(const bool uiEdge)
	{
		if(true == uiEdge)
			return bottomEdgeSpacing();
		return 0.0f;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float centerOfScreenX(void)
	{
		assert_if(leftOfScreen(false) >= rightOfScreen(false), "Invalid assumption being made; right edge is leftier than left.");
		return (rightOfScreen(false) - leftOfScreen(false)) / 2.0f;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float centerOfScreenY(void)
	{
		assert_if(bottomOfScreen(false) >= topOfScreen(false), "Invalid assumption being made; bottom edge is higher than top.");
		return (topOfScreen(false) - bottomOfScreen(false)) / 2.0f;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	cocos2d::CCPoint centerOfScreen(void)
	{
		return cocos2d::CCPoint(centerOfScreenX(), centerOfScreenY());
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
	static void dockToTop(uiElement& nodeToDock, const float offsetY, const bool uiEdge)
	{
		assert_if(nodeToDock.boundingBox().size.height <= 0.0f, "This node has no height to use for docking alignment!?");		
		const float halfHeight(nodeToDock.boundingBox().size.height / 2.0f);		
		const float topEdge(ig_interface::topOfScreen(uiEdge));
		nodeToDock.setPositionY(topEdge - halfHeight + (offsetY * g_convertRatio->y));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static void dockToBottom(uiElement& nodeToDock, const float offsetY, const bool uiEdge)
	{
		assert_if(nodeToDock.boundingBox().size.height <= 0.0f, "This node has no height to use for docking alignment!?");		
		const float halfHeight(nodeToDock.boundingBox().size.height / 2.0f);
		const float bottomEdge(ig_interface::bottomOfScreen(uiEdge));
		nodeToDock.setPositionY(bottomEdge + halfHeight + (offsetY * g_convertRatio->y));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static void dockToLeft(uiElement& nodeToDock, const float offsetX, const bool uiEdge)
	{
		assert_if(nodeToDock.boundingBox().size.width <= 0.0f, "This node has no width to use for docking alignment!?");
		const float halfWidth(nodeToDock.boundingBox().size.width / 2.0f);				  
		const float leftEdge(ig_interface::leftOfScreen(uiEdge));
		nodeToDock.setPositionX(leftEdge + halfWidth + (offsetX * g_convertRatio->x));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static void dockToRight(uiElement& nodeToDock, const float offsetX, const bool uiEdge)
	{
		assert_if(nodeToDock.boundingBox().size.width <= 0.0f, "This node has no width to use for docking alignment!?");
		const float halfWidth(nodeToDock.boundingBox().size.width / 2.0f);		
		const float rightEdge(ig_interface::rightOfScreen(uiEdge));
		nodeToDock.setPositionX(rightEdge - halfWidth + (offsetX * g_convertRatio->x));
	}	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void dockToCenterCenter(uiElement& nodeToDock, const float offsetX, const float offsetY)
	{
		nodeToDock.setPositionX(ig_interface::centerOfScreenX() + (offsetX * g_convertRatio->x));
		nodeToDock.setPositionY(ig_interface::centerOfScreenY() + (offsetY * g_convertRatio->y));		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void dockToBottomLeft(uiElement& nodeToDock, const float offsetX, const float offsetY, const bool uiEdge)
	{
		dockToBottom(nodeToDock, offsetY, uiEdge);
		dockToLeft(nodeToDock, offsetX, uiEdge);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void dockToBottomRight(uiElement& nodeToDock, const float offsetX, const float offsetY, const bool uiEdge)
	{
		dockToBottom(nodeToDock, offsetY, uiEdge);
		dockToRight(nodeToDock, offsetX, uiEdge);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void dockToBottomCenter(uiElement& nodeToDock, const float offsetX, const float offsetY, const bool uiEdge)
	{
		dockToBottom(nodeToDock, offsetY, uiEdge);
		nodeToDock.setPositionX(ig_interface::centerOfScreenX() + (offsetX * g_convertRatio->x));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void dockToTopLeft(uiElement& nodeToDock, const float offsetX, const float offsetY, const bool uiEdge)
	{
		dockToTop(nodeToDock, offsetY, uiEdge);
		dockToLeft(nodeToDock, offsetX, uiEdge);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void dockToTopRight(uiElement& nodeToDock, const float offsetX, const float offsetY, const bool uiEdge)
	{
		dockToTop(nodeToDock, offsetY, uiEdge);
		dockToRight(nodeToDock, offsetX, uiEdge);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void dockToTopCenter(uiElement& nodeToDock, const float offsetX, const float offsetY, const bool uiEdge)
	{
		dockToTop(nodeToDock, offsetY, uiEdge);
		nodeToDock.setPositionX(ig_interface::centerOfScreenX() + (offsetX * g_convertRatio->x));
	}	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
	
	void alignLeftEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingX)
	{
		assert_if(nodeToAlign.boundingBox().size.width <= 0.0f, "This node has no width to use for alignment!?");
		assert_if(stationaryNode.boundingBox().size.width <= 0.0f, "This node has no width to use for alignment!?");
		
		const float halfWidth((nodeToAlign.boundingBox().size.width / 2.0f) + (stationaryNode.boundingBox().size.width / 2.0f));
		nodeToAlign.setPositionX(stationaryNode.getPositionX() + halfWidth + (paddingX * g_convertRatio->x));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	void alignRightEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingX)
	{
		assert_if(nodeToAlign.boundingBox().size.width <= 0.0f, "This node has no width to use for alignment!?");
		assert_if(stationaryNode.boundingBox().size.width <= 0.0f, "This node has no width to use for alignment!?");
		
		const float halfWidth((nodeToAlign.boundingBox().size.width / 2.0f) + (stationaryNode.boundingBox().size.width / 2.0f));
		nodeToAlign.setPositionX(stationaryNode.getPositionX() - halfWidth - (paddingX * g_convertRatio->x));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void alignHorizontalCenters(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingX)
	{
		nodeToAlign.setPositionX(stationaryNode.getPositionX() + (paddingX * g_convertRatio->x));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void alignTopEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingY)
	{
		assert_if(nodeToAlign.boundingBox().size.height <= 0.0f, "This node has no height to use for alignment!?");
		assert_if(stationaryNode.boundingBox().size.height <= 0.0f, "This node has no height to use for alignment!?");
		
		const float halfHeight((nodeToAlign.boundingBox().size.height / 2.0f) + (stationaryNode.boundingBox().size.height / 2.0f));
		nodeToAlign.setPositionY(stationaryNode.getPositionY() - halfHeight - (paddingY * g_convertRatio->y));
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void alignBottomEdgeTo(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingY)
	{
		assert_if(nodeToAlign.boundingBox().size.height <= 0.0f, "This node has no height to use for alignment!?");
		assert_if(stationaryNode.boundingBox().size.height <= 0.0f, "This node has no height to use for alignment!?");
		
		const float halfHeight((nodeToAlign.boundingBox().size.height / 2.0f) + (stationaryNode.boundingBox().size.height / 2.0f));
		nodeToAlign.setPositionY(stationaryNode.getPositionY() + halfHeight + (paddingY * g_convertRatio->y));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void alignVerticalCenters(uiElement& nodeToAlign, uiElement& stationaryNode, const float paddingY)
	{
		nodeToAlign.setPositionY(stationaryNode.getPositionY() + (paddingY * g_convertRatio->y));
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	void slideInFromDirection(uiElement& nodeToSlide, const ButtonSlideDirection& fromDirection, const float startDelay)
	{
		g_effectMgr->addGameEffect(NULL, new NodeEffectButtonSlideIn(nodeToSlide, fromDirection, startDelay));
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	void slideOutToDirection(uiElement& nodeToSlide, const ButtonSlideDirection& towardDirection, const float startDelay)
	{
		g_effectMgr->addGameEffect(NULL, new NodeEffectButtonSlideOut(nodeToSlide, towardDirection, startDelay));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void slideInFromDirection(uiElement& nodeToSlide, const ButtonSlideDirection& fromDirection, const int elementIndex)
	{
		const float delay(NodeEffectButtonSlideIn::SceneStartDelay + (NodeEffectButtonSlideIn::PerNodeDelay * elementIndex));
		g_effectMgr->addGameEffect(NULL, new NodeEffectButtonSlideIn(nodeToSlide, fromDirection, delay));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void slideOutToDirection(uiElement& nodeToSlide, const ButtonSlideDirection& towardDirection, const int elementIndex)
	{
		const float delay(NodeEffectButtonSlideIn::SceneStartDelay + (NodeEffectButtonSlideIn::PerNodeDelay * elementIndex));
		g_effectMgr->addGameEffect(NULL, new NodeEffectButtonSlideOut(nodeToSlide, towardDirection, delay));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
}; /* namespace ig_interface */


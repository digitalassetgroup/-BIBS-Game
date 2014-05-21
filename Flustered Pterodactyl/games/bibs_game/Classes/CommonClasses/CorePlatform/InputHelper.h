#pragma once
#ifndef InputHelper_h
#define InputHelper_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
#include "Core/IntPoint.h"
#include <cocoa/CCGeometry.h>
using namespace cocos2d;
namespace cocos2d { class CCScene; class CCNode; class CCSet; class CCTouch; class CCAcceleration; }


namespace InputHelper
{
	CCPoint GetTouchPoint(CCTouch* touch);
	CCPoint GetPreviousTouchPoint(CCTouch* touch);
	CCPoint GetTouchPointBySet(CCSet* touches);
	
	CCPoint GetPointCenter();
	CCPoint GetPointRelativeSomePoint(const CCPoint& anchorPoint, const CCPoint& relativePoint);
	bool IsIpadWidth();
	bool IsIpadHeight();
	float GetVerticalRotationByAcceleration(CCAcceleration& prevAcceleration, CCAcceleration* acceleration, float kFilteringFactor = 0.5f);

	void SetTextMenuScale(CCNode* node);
	CCPoint ConvertToNodeSpace(CCNode* node, const CCPoint& worldPoint);
	CCPoint ConvertTouchToNodeSpace(CCNode* node, CCTouch* touch);
	CCPoint ConvertToWorldSpace(CCNode* node, const CCPoint& nodePoint);
	void SetSceneScale(CCScene* scene);

	//
	IntSize GetReferenceScreenSize();
	IntSize GetDesktopWindowSize();
	void SetDesktopWindowSize(const IntSize& size);
	void SetUnscaledWindowSize(const CCSize& size);
	CCSize GetUnscaledWindowSize();
	void SetNodeScale(float nodeScale);
	float GetNodeScale();
	void SetIsLoadSD(bool isLoadSD);
	bool IsLoadSD();	
}

#endif

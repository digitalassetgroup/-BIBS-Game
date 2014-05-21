#include "InputHelper.h"
#include "CorePlatform/CommonProperties.h"
#include "LogHelper.h"
#include "CorePlatform/CCPointMath.h"
#include <cocos2d.h>

CCPoint InputHelper::GetPreviousTouchPoint(CCTouch* touch)
{
	CCPoint location = touch->PREVIOUS_LOCATION_IN_VIEW(touch);
    location = CCDirector::sharedDirector()->convertToGL(location);
	return location;
}

//вернуть абсолютного позицию поинта на экране virtual
CCPoint InputHelper::GetTouchPoint(CCTouch* touch)
{
    CCPoint location = touch->LOCATION_IN_VIEW(touch);
    location = CCDirector::sharedDirector()->convertToGL(location);
	return location;
}

CCPoint InputHelper::GetTouchPointBySet(CCSet* touches)
{
	auto touch = (CCTouch*)touches->anyObject();
	auto point = touch->LOCATION_IN_VIEW(touch);
	point = cocos2d::CCDirector::sharedDirector()->convertToGL(point);
	return point;
}

CCPoint InputHelper::GetPointCenter()
{
	static CCPoint pointCenter;
	static bool isLoadedPointCenter = false;

	if (!isLoadedPointCenter)
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSizeInPixels();
		pointCenter = ccp(winSize.width/2, winSize.height/2);
		isLoadedPointCenter = true;
	}
	return pointCenter;
}

//вернуть точку relativePoint относительно точки anchorPoint
//relativePoint - точка с числовыми значениями в ретиновских пикселях, например - (435,19)
CCPoint InputHelper::GetPointRelativeSomePoint(const CCPoint& anchorPoint, const CCPoint& relativePoint)
{
	return ccp(anchorPoint.x + relativePoint.x, anchorPoint.y + relativePoint.y);
}

bool InputHelper::IsIpadWidth()
{
	const CCSize iPadSize = CCSize(768, 1024);
	auto winSize = InputHelper::GetUnscaledWindowSize();
	return (winSize.width == iPadSize.width);
}

bool InputHelper::IsIpadHeight()
{
	const CCSize iPadSize = CCSize(768, 1024);
	auto winSize = InputHelper::GetUnscaledWindowSize();
	return (winSize.height == iPadSize.height);
}

float InputHelper::GetVerticalRotationByAcceleration(CCAcceleration& prevAcceleration, CCAcceleration* acceleration, float kFilteringFactor)
{
	//MY_LOG_F("x = %.2f; y = %.2f; z = %.2f", acceleration->x, acceleration->y, acceleration->z);

    //kFilteringFactor - 0.05f;  //for plavnosti
    prevAcceleration.x = acceleration->x * kFilteringFactor + prevAcceleration.x * (1.0f - kFilteringFactor);
    prevAcceleration.y = acceleration->y * kFilteringFactor + prevAcceleration.y * (1.0f - kFilteringFactor);
    prevAcceleration.z = acceleration->z * kFilteringFactor + prevAcceleration.z * (1.0f - kFilteringFactor);

	//works if orientation is vertical and home buttom on down
	float lampRotation = Math::Clamp((float)-prevAcceleration.x * 90, -70.0f, 70.0f);
    if (CommonProperties::IsOrientationUpsideDown())  //if home button on up
		lampRotation = Math::Clamp((float)-prevAcceleration.y * 90, -70.0f, 70.0f);

	return lampRotation;
}

//текст скейлим обратно скейлу, т.к. текст должен быть одного размера на всех экранах
void InputHelper::SetTextMenuScale(CCNode* node)
{
	float scale = CommonProperties::GetScreenScale();
	//text can not seen (
	scale *= !CommonProperties::IsLoadHDImages() ? 3 : 1;
    node->setScale(scale);
	//if (scale != 1.0f)
	//	node->setScale(1/scale);
}

CCPoint InputHelper::ConvertToNodeSpace(CCNode* node, const CCPoint& worldPoint)
{
	return CCPointApplyAffineTransform(worldPoint, node->worldToNodeTransform());
}

CCPoint InputHelper::ConvertTouchToNodeSpace(CCNode* node, CCTouch *touch)
{
	CCPoint point = touch->LOCATION_IN_VIEW(touch);
	point = CCDirector::sharedDirector()->convertToGL(point);
	return InputHelper::ConvertToNodeSpace(node, point);
}

CCPoint InputHelper::ConvertToWorldSpace(CCNode* node, const CCPoint& nodePoint)
{
	return CCPointApplyAffineTransform(nodePoint, node->nodeToWorldTransform());
}

void InputHelper::SetSceneScale(CCScene* scene)
{
	float scale = CommonProperties::GetScreenScale();
	if (scale == 1.0f)
		return;

	scene->setScale(scale);
}

//эталонное разрешение экрана
static IntSize _referenceScreenSize(960, 640);
IntSize InputHelper::GetReferenceScreenSize() { return _referenceScreenSize; }

static IntSize _desktopWindowSize;
//for desktop
void InputHelper::SetDesktopWindowSize(const IntSize& size) { _desktopWindowSize = size; }
IntSize InputHelper::GetDesktopWindowSize() { return _desktopWindowSize; }

static CCSize _unscaledWindowSize(960, 640);
//design resolution
void InputHelper::SetUnscaledWindowSize(const CCSize& size) { _unscaledWindowSize = size; }
CCSize InputHelper::GetUnscaledWindowSize() { return _unscaledWindowSize; }

static float _nodeScale = 1;
void InputHelper::SetNodeScale(float nodeScale) { _nodeScale = nodeScale; }
float InputHelper::GetNodeScale() { return _nodeScale; }

static bool _isLoadSD = false;
void InputHelper::SetIsLoadSD(bool isLoadSD) { _isLoadSD = isLoadSD; }
bool InputHelper::IsLoadSD() { return _isLoadSD; }
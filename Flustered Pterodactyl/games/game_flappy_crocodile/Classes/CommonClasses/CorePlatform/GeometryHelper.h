#pragma once
#ifndef GeometryHelper_h
#define GeometryHelper_h

#include "Core/CoreAll.h"
#include "Core/StringHelper.h"
#include "CorePlatformAll.h"
#include <cocoa/CCGeometry.h>
using namespace cocos2d;


namespace GeometryHelper
{
	bool IsPointEmpty(const CCPoint& point);
	bool IsPointInRect(const CCPoint& point, const CCPoint& rectDownLeft, const CCPoint& rectTopRight);
	bool IsIntersectRects(const CCPoint& rectDownLeft1, const CCPoint& rectTopRight1, const CCPoint& rectDownLeft2, const CCPoint& rectTopRight2);
	CCPoint RotatePointAroundCenter(CCPoint sourcePoint, int direction /* -1 или 1 */);
	CCPoint RotatePointAroundPoint(const CCPoint& pointToRotate, const CCPoint& centerPoint, float angle);
	CCRect CCRectMultiply(const CCRect& rect, float number);
	CCRect TruncRect(const CCRect& rect);
	CCRect RoundRect(const CCRect& rect);
	CCRect GetIntersectedRect(const CCRect& r1, const CCRect& r2);
	CCRect GetUnionRect(const CCRect& r1, const CCRect& r2);
	CCRect GetExtendedRect(const CCRect& rect, float extendSize);
	float GetPointDistance(const CCPoint& point);
	CCPoint MoveTowardsOnDistance(const CCPoint& pointBegin, const CCPoint& pointEnd, float distance);
}


struct Rect : public CCRect
{
	public_ Rect() : CCRect() {}
	public_ Rect(float x, float y, float width, float height) : CCRect(x, y, width, height) {}
	public_ bool IsEmpty() const { return this->equals(Null); }

	public_ static Rect Null;

	//вернуть бокс, чуть меньший чем текущий. Юзается в столкновениях
	public_ Rect GetLilRect() 
	{
		const float delta = 0.1f;
		return Rect(origin.x + delta, origin.y + delta, size.width - delta*2, size.height - delta*2); 
	}

	public_ string Dump()
	{
		return StringHelper::format("x = %.1f, y = %.1f, w = %.1f, h = %.1f", origin.x, origin.y, size.width, size.height);
	}
};

#endif

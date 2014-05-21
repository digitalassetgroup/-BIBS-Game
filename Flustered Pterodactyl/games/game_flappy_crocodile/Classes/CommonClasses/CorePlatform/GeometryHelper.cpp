#include "GeometryHelper.h"
#include "Core/MathHelper.h"
#include <support/CCPointExtension.h>

using namespace GeometryHelper;

Rect Rect::Null = Rect();

bool GeometryHelper::IsPointEmpty(const CCPoint& point)
{
	return point.x == 0.0f && point.y == 0.0f;
}

bool GeometryHelper::IsPointInRect(const CCPoint& point, const CCPoint& rectDownLeft, const CCPoint& rectTopRight)
{
	return Digit(point.x).IsBetween(rectDownLeft.x, rectTopRight.x) 
		&& Digit(point.y).IsBetween(rectDownLeft.y, rectTopRight.y);
}

//пересекается ли ректангл с углами rectDownLeft1 и rectTopRight1 с ректанглом с углами rectDownLeft2 и rectTopRight2
bool GeometryHelper::IsIntersectRects(const CCPoint& rectDownLeft1, const CCPoint& rectTopRight1, const CCPoint& rectDownLeft2, const CCPoint& rectTopRight2)
{
	return IsPointInRect(rectDownLeft1, rectDownLeft2, rectTopRight2) 
		|| IsPointInRect(rectTopRight1, rectDownLeft2, rectTopRight2)
		|| IsPointInRect(ccp(rectDownLeft1.x, rectTopRight1.y), rectDownLeft2, rectTopRight2)
		|| IsPointInRect(ccp(rectTopRight1.x, rectTopRight1.y), rectDownLeft2, rectTopRight2);
}

CCPoint GeometryHelper::RotatePointAroundCenter(CCPoint sourcePoint, int direction /* -1 или 1 */)
{
	//direction > 0, если ПРОТИВ часовой стрелке
	
	CCPoint result;

	if (direction > 0)
	{	
		//x -> -y; -x -> y
		//y -> x; -y -> -x;
		result.x = sourcePoint.y; 
		result.y = -sourcePoint.x;
	}
	else if (direction < 0)
	{
		//x -> y; -x -> -y;
		//y -> -x; -y -> x;
		result.x = -sourcePoint.y; 
		result.y = sourcePoint.x;
	}

	return result;
}

CCPoint GeometryHelper::RotatePointAroundPoint(const CCPoint& pointToRotate, const CCPoint& centerPoint, float angle)
{
	CCPoint res;

	float angleSin = sin(Math::DegreesToRadians(angle));
	float angleCos = cos(Math::DegreesToRadians(angle));

	res.x = ((pointToRotate.x - centerPoint.x) * angleCos) - ((pointToRotate.y - centerPoint.y) * angleSin) + centerPoint.x;
	res.y = ((pointToRotate.y - centerPoint.y) * angleCos) - ((pointToRotate.x - centerPoint.x) * angleSin) + centerPoint.y;
	return res;
}

CCRect GeometryHelper::CCRectMultiply(const CCRect& rect, float number)
{
    CCRect result = rect;
    result.origin.x *= number;
    result.origin.y *= number;
    result.size.width *= number;
    result.size.height *= number;
    return result;
}

CCRect GeometryHelper::RoundRect(const CCRect& rect)
{
	CCRect result = CCRect(Math::RoundF(rect.origin.x), Math::RoundF(rect.origin.y),
		Math::RoundF(rect.size.width), Math::RoundF(rect.size.height));
	return result;
}

CCRect GeometryHelper::TruncRect(const CCRect& rect)
{
	CCRect result = CCRect(Math::TruncF(rect.origin.x), Math::TruncF(rect.origin.y),
		Math::TruncF(rect.size.width), Math::TruncF(rect.size.height));
	return result;
}

CCRect GeometryHelper::GetIntersectedRect(const CCRect& r1, const CCRect& r2)
{
	CCRect result;

	result.origin.x = r1.origin.x > r2.origin.x ? r1.origin.x : r2.origin.x;
	result.origin.y = r1.origin.y > r2.origin.y ? r1.origin.y : r2.origin.y;

	result.size.width = (r1.getMaxX() < r2.getMaxX() ? r1.getMaxX() : r2.getMaxX()) - result.origin.x;
	result.size.height = (r1.getMaxY() < r2.getMaxY() ? r1.getMaxY() : r2.getMaxY()) - result.origin.y;

	return result;
}

CCRect GeometryHelper::GetUnionRect(const CCRect& r1, const CCRect& r2)
{
	CCRect res;
	res.origin.x = r1.origin.x < r2.origin.x ? r1.origin.x : r2.origin.x;
	res.origin.y = r1.origin.y < r2.origin.y ? r1.origin.y : r2.origin.y;
	res.size.width = (r1.getMaxX() > r2.getMaxX() ? r1.getMaxX() : r2.getMaxX()) - res.origin.x;
	res.size.height = (r1.getMaxY() > r2.getMaxY() ? r1.getMaxY() : r2.getMaxY()) - res.origin.y;
	return res;
}

//вернуть рекс, который больше на extendSize пикселей с каждой стороны
CCRect GeometryHelper::GetExtendedRect(const CCRect& rect, float extendSize)
{
	auto result = rect;
	result.origin.x -= extendSize;
	result.origin.y -= extendSize;
	result.size.width += 2*extendSize;
	result.size.height += 2*extendSize;
	return result;
}

float GeometryHelper::GetPointDistance(const CCPoint& point)
{
	return sqrtf(point.x * point.x + point.y * point.y);
}

//вернуть точку, которая стоит на расстоянии distance в направлении (pointEnd - pointBegin)
CCPoint GeometryHelper::MoveTowardsOnDistance(const CCPoint& pointBegin, const CCPoint& pointEnd, float distance)
{
	float realDistance = (pointEnd - pointBegin).getLength();
	distance = Math::Min(realDistance, distance);
	float percent = distance / realDistance;
	auto pointResult = pointBegin + (pointEnd - pointBegin) * percent;
	return pointResult;
}
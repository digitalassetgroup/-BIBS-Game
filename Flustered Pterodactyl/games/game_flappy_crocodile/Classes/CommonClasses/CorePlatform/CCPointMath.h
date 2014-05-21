#pragma once
#ifndef CCPointMath_h
#define CCPointMath_h

#include "support/CCPointExtension.h"

namespace cocos2d
{
	inline CCPoint operator/(const CCPoint& a, const CCPoint& b) { return ccp(a.x/b.x, a.y/b.y); }
	inline CCPoint operator*(const CCPoint& a, const CCPoint& b) { return ccp(a.x*b.x, a.y*b.y); }

	inline CCPoint& operator+=(CCPoint& a, const CCPoint& b)
	{
		a.x += b.x;
		a.y += b.y;
		return a;
	}
	
	inline CCPoint& operator-=(CCPoint& a, const CCPoint& b)
	{
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}
	
	inline CCPoint toVec(const CCSize& vec) { return CCPoint(vec.width, vec.height); }

	//CCSize

	inline CCSize operator/(const CCSize& a, const CCSize& b) { return CCSize(a.width/b.width, a.height/b.height); }
	inline CCSize operator*(const CCSize& a, const CCSize& b) { return CCSize(a.width*b.width, a.height*b.height); }

	inline CCSize& operator+=(CCSize& a, const CCSize& b)
	{
		a.width += b.width;
		a.height += b.height;
		return a;
	}
	
	inline CCSize& operator-=(CCSize& a, const CCSize& b)
	{
		a.width -= b.width;
		a.height -= b.height;
		return a;
	}

	inline bool operator==(const CCPoint& a, const CCPoint& b)
	{
		return Math::IsNumEquals(a.x, b.x) && Math::IsNumEquals(a.y, b.y);
	}

	inline bool operator!=(const CCPoint& a, const CCPoint& b)
	{
		return !operator==(a, b);
	}
}

#endif

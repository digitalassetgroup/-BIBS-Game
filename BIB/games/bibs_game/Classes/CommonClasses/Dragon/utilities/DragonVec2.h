#pragma once
#ifndef DragonVec2_h
#define DragonVec2_h

#include "cocoa/CCGeometry.h"

struct DragonVec2
{
	/// Default constructor does nothing (for performance).
	DragonVec2() {}

	/// Construct using coordinates.
	DragonVec2(float x, float y) : x(x), y(y) {}

	/// Set this vector to all zeros.
	void SetZero() { x = 0.0f; y = 0.0f; }

	/// Set this vector to some specified coordinates.
	void Set(float x_, float y_) { x = x_; y = y_; }

	/// Negate this vector.
	DragonVec2 operator -() const { DragonVec2 v; v.Set(-x, -y); return v; }
	
	/// Read from and indexed element.
	float operator () (int i) const
	{
		return (&x)[i];
	}

	/// Write to an indexed element.
	float& operator () (int i)
	{
		return (&x)[i];
	}

	/// Add a vector to this vector.
	void operator += (const DragonVec2& v)
	{
		x += v.x; y += v.y;
	}
	
	/// Subtract a vector from this vector.
	void operator -= (const DragonVec2& v)
	{
		x -= v.x; y -= v.y;
	}

	/// Multiply this vector by a scalar.
	void operator *= (float a)
	{
		x *= a; y *= a;
	}

	/// Get the length squared. For performance, use this instead of
	/// DragonVec2::Length (if possible).
	float LengthSquared() const
	{
		return x * x + y * y;
	}

	/// Get the skew vector such that dot(skew_vec, other) == cross(vec, other)
	DragonVec2 Skew() const
	{
		return DragonVec2(-y, x);
	}

	float x, y;
};


namespace cocos2d
{
	inline CCPoint toVec(const DragonVec2& vec) { return CCPoint(vec.x, vec.y); }
	inline DragonVec2 toVec(const CCPoint& vec) { return DragonVec2(vec.x, vec.y); }
}

#endif

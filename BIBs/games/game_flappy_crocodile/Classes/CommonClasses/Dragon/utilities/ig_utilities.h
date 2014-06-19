#pragma once
#ifndef _DracoGames_igUtilities_h_
#define _DracoGames_igUtilities_h_

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <math.h>

#include "ig_assert.h"
#include "ig_constants.h"
#include "ig_defines.h"
#include "DragonVec2.h"
#include "ig_touchEvent.h"

#include "CorePlatform/InputHelper.h"

struct igScreenSize
{
	float width;
	float height;
	float halfWidth;
	float halfHeight;
};


#define gScreenSize InputHelper::GetUnscaledWindowSize()
extern const cocos2d::CCPoint* g_convertRatio;


/* Define SIZE_T_MAX unless defined through <limits.h>. */
#ifndef SIZE_T_MAX
# define SIZE_T_MAX     ((size_t)-1)
#endif /* !SIZE_T_MAX */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DragonVec2;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace DracoGames
{
	/* After Super Kid Cannon these should be placed in DracoGames::Constants */
	static const float igPI(static_cast<float>(M_PI));
	static const float igPI2(static_cast<float>(M_PI * 2.0));
	static const float igEpsilon(0.000000119209289550781250000f); /* about pow(2, -23) */
	static const float igE(static_cast<float>(M_E));

	namespace Utils
	{
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		/* The following Interpolation functions expect a value from 0 to 1 and return a value 0 to 1- unless noted. */
		
		/**
		 *	Smooth step is very similar to linear interpolation, but it moves slower near 0 and 1, faster in the middle
		 *	to make up for the time.
		 */
		inline float Interp_SmoothStep(float v) { return v * v * (3 - 2 * v); }
		
		inline float Interp_HopUp(float v) { return sinf(v * DracoGames::igPI); }
		
		/**
		 *	Performs a curve based interpolation that can extend beyond the 0 to 1 range based on inputs.
		 *	@note see http://sol.gfxile.net/interpolation/index.html for more information.
		 */
		inline float Interp_CatMullRom(float t, float p0, float p1, float p2, float p3)
		{
			return 0.5f * ((2 * p1) +
						   (-p0 + p2) * t +
						   (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
						   (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);			
		}			

		/**
		 *	Computes the distance from a to b.  This involves a square-root.
		 */
		float DistanceTo(const DragonVec2 &a, const DragonVec2 &b);
		
		/**
		 *	Computes the distance from x1,y1 to x2,y2.  This involves a square-root.
		 */
		float DistanceTo(float x1, float y1, float x2, float y2);
		
		/**
		 *	Computes the squared distance from x1,y1, to x2,y2 without using the square-root, this is faster than
		 *	the non Sq version, but can only be used in some places.
		 */
		float DistanceToSq(float x1, float y1, float x2, float y2);
		
		/**
		 *	Computes how the closest placec the point (xPt,yPt) is to the line segment (x1,y1 - x2,y2)
		 */
		float DistanceFromLine(float x1, float y1, float x2, float y2, float xPt, float yPt);

		/*
		 *	Note: The angle returned would be in degrees as follows: 0.0 degrees for a vector (0,-1) 90.0 degrees for a
		 *  vector (1, 0) 180.0 degrees for a vector (0, 1) 270.0 degrees for a vector (-1, 0) 
		 */
		float AngleFromVec2(float x, float y);
		float AngleFromVec2(float *xy);
		void Vec2FromAngle(float *x, float *y, float angle);
		float* Vec2FromAngle(float *xy, float angle);
		DragonVec2 Vec2FromAngle(float angle);

		inline float RadiansToDegrees(const float rad) { return (rad / igPI) * 180.0f; }		
		inline float DegreesToRadians(const float deg) { return (deg / 180.0f) * igPI; }
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template <typename T> T Max(T a, T b)
		{
			return a < b ? b : a; 
		}
		
		template <typename T> T Min(T a, T b)
		{
			return a < b ? a : b; 
		}
		
		template <typename T> T Clamp(T v, T min, T max)
		{
			if(v < min) return min;
			if(max < v) return max;
			return v;
		}

		template <typename T> void Swap(T &a, T &b)
		{
			T c = a; 
			a = b; 
			b = c; 
		}

		template <typename T> std::string ToString(const T &v)
		{
			std::stringstream ss; 
			ss << v; 
			return ss.str ();
		}
		
		template <typename T> T FromString(const std::string &str)
		{
			std::stringstream ss (str); 
			T v; 
			ss >> v; 
			return v; 
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		float AngleBetween2Vectors(const DragonVec2 &a, const DragonVec2 &b);

		/**
		 *	Computes a random integer just as rand() would.
		 *	@note Actually returns rand in the current implementation.
		 */
		int randomInt(void);
		
		/**
		 *	Returns a random number between min and max, including min and max.  For instance if called randomInt(3, 6)
		 *	possible return values are 3, 4, 5, and 6.
		 *
		 *	@note min must be less than max.
		 */
		int randomInt(int min, int max);
		
		/**
		 *	Returns a random value between (and including) 0.0f and 1.0f
		 */
		float randomFloat(void);
		
		/**
		 *	Returns a random number between min and max, including min and max.  If called randomFloat(0.0f, 2.0f)
		 *	return value will be:  0.0f <= value <= 1.0f
		 */
		float randomFloat(float min, float max);

		/**
		 *	Checks if a float is near enough zero to be called zero; less than igEpsilon.
		 */
		bool isZero(const float value);		
		
		/**
		 *	Checks if two floats are within igEpsilon of eachother to consider them equal.
		 */
		bool isEqual(const float lhs, const float rhs);
		
		
		typedef struct 
		{
			float theta;
			float phi;
			float rho;
		} Spherical_Coord;

		Spherical_Coord spherical_from_cartesian(float x, float y, float z);		
		bool compare_y_ccpoint(const cocos2d::CCPoint& first,const cocos2d::CCPoint& second);
		
    } /* namespace Utils */
} /* namespace DracoGames */

#endif /* _DracoGames_igUtilities_h_ */

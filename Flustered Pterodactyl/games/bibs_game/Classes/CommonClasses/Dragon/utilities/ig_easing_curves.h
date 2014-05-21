#pragma once
#ifndef _IG_EASING_CURVES_H_
#define _IG_EASING_CURVES_H_

#include "ig_assert.h"
#include <cmath>

#define DEFAULT_OVERSHOOT 1.70158f

#if !defined(M_PI)
	#define M_PI (3.14159265358979323846f)
#endif


#define IG_ASSERT_01_INC(x) assert_if( (x) < 0.0f && (x) > 1.0f, "Easing paramter is out of [0, 1], fix this!")


// WARNING : bouncing is not implemented still !

namespace DracoGames
{

	enum EasingCurveType
	{
		EasingCurveNone = -1,

		EasingCurveLinear,
		EasingCurveInQuad,
		EasingCurveOutQuad,
		EasingCurveInOutQuad,
		EasingCurveOutInQuad,
		EasingCurveInCubic,
		EasingCurveOutCubic,
		EasingCurveInOutCubic,
		EasingCurveOutInCubic,
		EasingCurveInQuard,
		EasingCurveOutQuard,
		EasingCurveInOutQuard,
		EasingCurveOutInQuard,
		EasingCurveInQuint,
		EasingCurveOutQuint,
		EasingCurveInOutQuint,
		EasingCurveOutInQuint,
		EasingCurveInSine,
		EasingCurveOutSine,
		EasingCurveInOutSine,
		EasingCurveOutInSine,
		EasingCurveInExpo,
		EasingCurveOutExpo,
		EasingCurveInOutExpo,
		EasingCurveOutInExpo,
		EasingCurveInCirc,
		EasingCurveOutCirc,
		EasingCurveInOutCirc,
		EasingCurveOutInCirc,
		EasingCurveInElastic,
		EasingCurveOutElastic,
		EasingCurveInOutElastic,
		EasingCurveOutInElastic,
		EasingCurveInBack,
		EasingCurveOutBack,
		EasingCurveInOutBack,
		EasingCurveOutInBack,
		EasingCurveInBounce,
		EasingCurveOutBounce,
		EasingCurveInOutBounce,
		EasingCurveOutInBounce,
		NumEasingCurveTypes,
        EasingCurveUnknown
        
	}; // EasingCurveType

	struct EasingCurves
	{
		inline static float ec_linear(float x)
		{
			IG_ASSERT_01_INC(x);
			return x;
		}

//	- - quad - -
		inline static float ec_in_quad(float x)
		{
			IG_ASSERT_01_INC(x);
			return x * x;
		}

		inline static float ec_out_quad(float x)
		{
			IG_ASSERT_01_INC(x);
			return - x * (x - 2.0f);
		}

		inline static float ec_in_out_quad(float x)
		{
			IG_ASSERT_01_INC(x);
			x *= 2.0f;
			if (x < 1) 
			{
				return x * x * 0.5f;
			}
			else
			{
				x -= 1.0f;
				return -0.5f * (x * (x - 2.0f) - 1.0f);
			}	
		}

		inline static float ec_out_in_quad(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return 0.5f * ec_out_quad(x * 2.0f) ;
			return 0.5f * ec_in_quad((2.0f * x) - 1.0f) + 0.5f;		
        }

		inline static float ec_in_cubic(float x)
		{
			IG_ASSERT_01_INC(x);
			return x * x * x ;
		}

//	- - cubic - -
		inline static float ec_out_cubic(float x)
		{
			IG_ASSERT_01_INC(x);
			x -= 1.0f;
			return x * x * x + 1.0f ;
		}

		inline static float ec_in_out_cubic(float x)
		{
			IG_ASSERT_01_INC(x);
			x *= 2.0f;
			if (x < 1)
			{
				return x * x * x * 0.5f;
			}
			else 
			{
				x -= 2.0f;
				return 0.5f * (x * x * x + 2.0f);
			}	
		}

		inline static float ec_out_in_cubic(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_cubic (2.0f * x) * 0.5f;
			return ec_in_cubic(2.0f * x - 1.0f) * 0.5f + 0.5f;		}

//	- - quard - -
		inline static float ec_in_quard(float x)
		{
			IG_ASSERT_01_INC(x);
			x *= x;
			return x * x;
		}

		inline static float ec_out_quard(float x)
		{
			IG_ASSERT_01_INC(x);
			x -= 1.0f;
			x *= x;
			return -(x * x - 1.0f);
		}

		inline static float ec_in_out_quard(float x)
		{
			IG_ASSERT_01_INC(x);
			x *= 2.0f;
			if (x < 1) 
			{
				x *= x;
				return x * x * 0.5f;
			} 
			else
			{
				x -= 2.0f;
				x *= x;
				return -0.5f * (x * x - 2.0f);
			}	
		}

		inline static float ec_out_in_quard(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_quard(x * 2.0f) * 0.5f;
			return ec_in_quard(2.0f * x - 1.0f) * 0.5f + 0.5f;
		}

//	- - quint - -
		inline static float ec_in_quint(float x)
		{
			IG_ASSERT_01_INC(x);
			float x2 = x * x;
			return x * x2 * x2;
		}

		inline static float ec_out_quint(float x)
		{
			IG_ASSERT_01_INC(x);
			x -= 1.0f;
			float x2 = x * x;
			return x * x2 * x2 + 1.0f;
		}

		inline static float ec_in_out_quint(float x)
		{
			IG_ASSERT_01_INC(x);
			x *= 2.0f;
			if (x < 1)
			{
				float x2 = x * x;
				return x * x2 * x2 * 0.5f;
			} 
			else 
			{
				x -= 2.0f;
				float x2 = x * x;
				return 0.5f * (x * x2 * x2 + 2.0f);
			}	
		}

		inline static float ec_out_in_quint(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_quint(x * 2.0f) * 0.5f;
			return ec_in_quint(2.0f * x - 1.0f) * 0.5f + 0.5f;
		}

//	- - sine - -
		inline static float ec_in_sine(float x)
		{
			IG_ASSERT_01_INC(x);
			return static_cast<float>((0.0f == x) ? 1.0f : -cos(x * M_PI * 0.5f) + 1.0f);
		}

		inline static float ec_out_sine(float x)
		{
			IG_ASSERT_01_INC(x);
			return static_cast<float>(sin(x * M_PI * 0.5f));
		}

		inline static float ec_in_out_sine(float x)
		{
			IG_ASSERT_01_INC(x);
			return static_cast<float>(-.5f * (cos(M_PI * x) - 1.0f));
		}

		inline static float ec_out_in_sine(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_sine(x * 2.0f) * 0.5f;
			return ec_in_sine(2.0f * x - 1.0f) * 0.5f + 0.5f;
		}

//	- - expo - -
		inline static float ec_in_expo(float x)
		{
			IG_ASSERT_01_INC(x);
			return (0 == x) ? 1.0f : (float)pow(2.0f, 10 * (x - 1) - .0001f);
		}

		inline static float ec_out_expo(float x)
		{
			IG_ASSERT_01_INC(x);
			return (1.0f == x) ? 1.0f : 1.001f * (-(float)pow(2.0f, -10 * x) + 1);
		}

		inline static float ec_in_out_expo(float x)
		{
			IG_ASSERT_01_INC(x);
			if (0.0f == x) return 0.0f;
			if (1.0f == x) return 1.0f;
			x *= 2.0f;
			if (x < 1.0f) return 0.5f * (float)pow(2.0f, 10 * (x - 1)) - .0005f;
			return static_cast<float>(.5f * 1.0005 * (-pow(2.0f, -10.0f * (x - 1.0f)) +2.0f));
		}

		inline static float ec_out_in_expo(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_expo(x * 2.0f) * 0.5f;
			return ec_in_expo(2.0f * x - 1.0f) * 0.5f + 0.5f;
		}

//	- - circ - -
		inline static float ec_in_circ(float x)
		{
			IG_ASSERT_01_INC(x);
			return -((float)sqrt(1.0f - x * x) - 1.0f);
		}

		inline static float ec_out_circ(float x)
		{
			IG_ASSERT_01_INC(x);
			x -= 1.0f;
			return (float)sqrt(1.0f - x * x);
		}

		inline static float ec_in_out_circ(float x)
		{
			IG_ASSERT_01_INC(x);
			x *= 2.0f;
			if (x < 1.0f) 
			{
				return -0.5f * ((float)sqrt(1.0f - x * x) - 1.0f);
			}
			else 
			{
				x -= 2.0f; 
				return 0.5f * ((float)sqrt(1.0f - x * x) + 1.0f);
			}
		}

		inline static float ec_out_in_circ(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_circ(x * 2.0f) * 0.5f;
			return ec_in_circ(2.0f * x - 1.0f) * 0.5f + 0.5f;
		}

//	- - elastic - -
		inline static float ec_in_elastic(float x)
		{
			IG_ASSERT_01_INC(x);
			return -((float)sqrt(1.0f - x * x) - 1.0f);
		}

		inline static float ec_out_elastic(float x)
		{
			IG_ASSERT_01_INC(x);
			x -= 1.0f;
			return (float)sqrt(1.0f - x * x);
		}

		inline static float ec_in_out_elastic(float x)	
		{
			IG_ASSERT_01_INC(x);
			x *= 2.0f;
			if (x < 1.0f) 
			{
				return -0.5f * ((float)sqrt(1.0f - x * x) - 1.0f);
			}
			else 
			{
				x -= 2.0f; 
				return 0.5f * ((float)sqrt(1.0f - x * x) + 1.0f);
			}
		}

		inline static float ec_out_in_elastic(float x)	
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_elastic(x * 2.0f) * 0.5f;
			return ec_in_elastic(2.0f * x - 1.0f) * 0.5f + 0.5f;
		}

//	- - back - -
		inline static float ec_in_back(float x)
		{
			IG_ASSERT_01_INC(x);
            float s = get_overshoot();
            return x * x * ((s + 1.0f) * x - s);
		}

		inline static float ec_out_back(float x)
		{
			IG_ASSERT_01_INC(x);
            float s = get_overshoot();
            x -= 1.0f;
            return x * x * ((s + 1.0f) * x + s) + 1;
		}

		static float ec_in_out_back(float x);

		inline static float ec_out_in_back(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_back(x * 2.0f) * 0.5f;
			return 0.5f * ec_in_back(2.0f * x - 1.0f) + 0.5f;
		}

//	- - bounce - -
		inline static float ec_in_bounce(float x)
		{
			IG_ASSERT_01_INC(x);
			return -((float)sqrt(1.0f - x * x) - 1.0f);
		}

		inline static float ec_out_bounce(float x)
		{
			IG_ASSERT_01_INC(x);
			x -= 1.0f;
			return (float)sqrt(1.0f - x * x);
		}

		inline static float ec_in_out_bounce(float x)
		{
			IG_ASSERT_01_INC(x);
			x *= 2.0f;
			if (x < 1.0f) 
			{
				return -0.5f * ((float)sqrt(1.0f - x * x) - 1.0f);
			}
			else 
			{
				x -= 2.0f; 
				return 0.5f * ((float)sqrt(1.0f - x * x) + 1.0f);
			}
		}

		inline static float ec_out_in_bounce(float x)
		{
			IG_ASSERT_01_INC(x);
			if (x < 0.5) return ec_out_bounce(x * 2.0f) * 0.5f;
			return ec_in_bounce(2.0f * x - 1.0f) * 0.5f + 0.5f;
		}

		typedef float (*EasingFunc)(float);

		static EasingFunc GetEasingFunc(EasingCurveType t);
		static float eval(float x, EasingCurveType t);
        static float easing_interpolate(float from, float to, float percent, EasingCurveType t);
        static void push_overshoot(float overshoot);
        static void pop_overshoot();
    private:
        static float get_overshoot();
	};

	
} // namespace DracoGames

#endif




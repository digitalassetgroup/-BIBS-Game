#include "ig_easing_curves.h"
#include <vector>



namespace DracoGames
{


std::vector<float> _overshoot_stack;
    
    
void EasingCurves::push_overshoot(float overshoot)
{
    _overshoot_stack.push_back(overshoot);
}
    
void EasingCurves::pop_overshoot()
{
    if (!_overshoot_stack.empty())
    {
        _overshoot_stack.pop_back();
    }    
}

float EasingCurves::get_overshoot()
{
    if (!_overshoot_stack.empty())
    {
        return _overshoot_stack.back();
    }

    return DEFAULT_OVERSHOOT;
}
    
EasingCurves::EasingFunc EasingCurves::GetEasingFunc(EasingCurveType t)
{
	switch(t)
	{
		case EasingCurveLinear: return ec_linear;
		case EasingCurveInQuad: return ec_in_quad;
		case EasingCurveOutQuad: return ec_out_quad;
		case EasingCurveInOutQuad: return ec_in_out_quad;
		case EasingCurveOutInQuad: return ec_out_in_quad;
		case EasingCurveInCubic: return ec_in_cubic;
		case EasingCurveOutCubic: return ec_out_cubic;
		case EasingCurveInOutCubic: return ec_in_out_cubic;
		case EasingCurveOutInCubic: return ec_out_in_cubic;
		case EasingCurveInQuard: return ec_in_quard;
		case EasingCurveOutQuard: return ec_out_quard;
		case EasingCurveInOutQuard: return ec_in_out_quard;
		case EasingCurveOutInQuard: return ec_out_in_quard;
		case EasingCurveInQuint: return ec_in_quint;
		case EasingCurveOutQuint: return ec_out_quint;
		case EasingCurveInOutQuint: return ec_in_out_quint;
		case EasingCurveOutInQuint: return ec_out_in_quint;
		case EasingCurveInSine: return ec_in_sine;
		case EasingCurveOutSine: return ec_out_sine;
		case EasingCurveInOutSine: return ec_in_out_sine;
		case EasingCurveOutInSine: return ec_out_in_sine;
		case EasingCurveInExpo: return ec_in_expo;
		case EasingCurveOutExpo: return ec_out_expo;
		case EasingCurveInOutExpo: return ec_in_out_expo;
		case EasingCurveOutInExpo: return ec_out_in_expo;
		case EasingCurveInCirc: return ec_in_circ;
		case EasingCurveOutCirc: return ec_out_circ;
		case EasingCurveInOutCirc: return ec_in_out_circ;
		case EasingCurveOutInCirc: return ec_out_in_circ;
		case EasingCurveInElastic: return ec_in_elastic;
		case EasingCurveOutElastic: return ec_out_elastic;
		case EasingCurveInOutElastic: return ec_in_out_elastic;
		case EasingCurveOutInElastic: return ec_out_in_elastic;
		case EasingCurveInBack: return ec_in_back;
		case EasingCurveOutBack: return ec_out_back;
		case EasingCurveInOutBack: return ec_in_out_back;
		case EasingCurveOutInBack: return ec_out_in_back;
		case EasingCurveInBounce: return ec_in_bounce;
		case EasingCurveOutBounce: return ec_out_bounce;
		case EasingCurveInOutBounce: return ec_in_out_bounce;
		case EasingCurveOutInBounce: return ec_out_in_bounce;
		default:
			warning_if(true, "unknown easing type, use linear easing by default")
			return NULL;
	}

	return NULL;
}
    
float EasingCurves::eval(float x, EasingCurveType t)
{
	switch (t)
	{
		case EasingCurveLinear:
			return ec_linear(x);
		case EasingCurveInQuad:
			return ec_in_quad(x);
		case EasingCurveOutQuad:
			return ec_out_quad(x);
		case EasingCurveInOutQuad:
			return ec_in_out_quad(x);
		case EasingCurveOutInQuad:
			return ec_out_in_quad(x);
		case EasingCurveInCubic:
			return ec_in_cubic(x);
		case EasingCurveOutCubic:
			return ec_out_cubic(x);
		case EasingCurveInOutCubic:
			return ec_in_out_cubic(x);
		case EasingCurveOutInCubic:
			return ec_out_in_cubic(x);
		case EasingCurveInQuard:
			return ec_in_quard(x);
		case EasingCurveOutQuard:
			return ec_out_quard(x);
		case EasingCurveInOutQuard:
			return ec_in_out_quard(x);
		case EasingCurveOutInQuard:
			return ec_out_in_quard(x);
		case EasingCurveInQuint:
			return ec_in_quint(x);
		case EasingCurveOutQuint:
			return ec_out_quint(x);
		case EasingCurveInOutQuint:
			return ec_in_out_quint(x);
		case EasingCurveOutInQuint:
			return ec_out_in_quint(x);
		case EasingCurveInSine:
			return ec_in_sine(x);
		case EasingCurveOutSine:
			return ec_out_sine(x);
		case EasingCurveInOutSine:
			return ec_in_out_sine(x);
		case EasingCurveOutInSine:
			return ec_out_in_sine(x);
		case EasingCurveInExpo:
			return ec_in_expo(x);
		case EasingCurveOutExpo:
			return ec_out_expo(x);
		case EasingCurveInOutExpo:
			return ec_in_out_expo(x);
		case EasingCurveOutInExpo:
			return ec_out_in_expo(x);
		case EasingCurveInCirc:
			return ec_in_circ(x);
		case EasingCurveOutCirc:
			return ec_out_circ(x);
		case EasingCurveInOutCirc:
			return ec_in_out_circ(x);
		case EasingCurveOutInCirc:
			return ec_out_in_circ(x);
		case EasingCurveInElastic:
			return ec_in_elastic(x);
		case EasingCurveOutElastic:
			return ec_out_elastic(x);
		case EasingCurveInOutElastic:
			return ec_in_out_elastic(x);
		case EasingCurveOutInElastic:
			return ec_out_in_elastic(x);
		case EasingCurveInBack:
			return ec_in_back(x);
		case EasingCurveOutBack:
			return ec_out_back(x);
		case EasingCurveInOutBack:
			return ec_in_out_back(x);
		case EasingCurveOutInBack:
			return ec_out_in_back(x);
		case EasingCurveInBounce:
			return ec_in_bounce(x);
		case EasingCurveOutBounce:
			return ec_out_bounce(x);
		case EasingCurveInOutBounce:
			return ec_in_out_bounce(x);
		case EasingCurveOutInBounce:
			return ec_out_in_bounce(x);
		default:
			warning_if(true, "unknown easing type, use linear easing by default")
			return x;

	};
    warning_if(true, "unknown easing type, use linear easing by default")
	return x;
}
    
  //  static float eval(float x, EasingCurveType t);
//    static float easing_interpolate(float from, float to, float percent, EasingCurveType t);


float EasingCurves::easing_interpolate(float from, float to, float percent, EasingCurveType t)
{
    return from + (to - from) * eval(percent, t);
}
    
//#define EASING_IN_OUT_IS_CHEKING

float EasingCurves::ec_in_out_back(float x)
{
	IG_ASSERT_01_INC(x);

#ifdef EASING_IN_OUT_IS_CHEKING
	float realX = x;
#endif

    float s = get_overshoot();
	x *= 2.0f;
	if (x < 1.0f) 
	{
        s *= 1.525f;
        float res = 0.5f * (x * x * ((s+1)*x - s));

#ifdef EASING_IN_OUT_IS_CHEKING
		static float min = 9999;
		if (res < min)
			min = res;
		else if (res > min)
		{
			int x11 = 5; 
			if (res > 0) //first time here will be extremum. need breakpoint
			{
				int x22 = 33;
			}
		}
#endif

		return res;
	}
	else 
	{
		x -= 2;

        s *= 1.525f;
        float res = 0.5f * (x * x *((s+1) * x + s) + 2.0f);

#ifdef EASING_IN_OUT_IS_CHEKING
		static float max = -9999;
		if (res > max)
			max = res;
		else if (res < max)
		{
			int x11 = 5;
		}
#endif

		return res;
	}
}

} // namespace DracoGames

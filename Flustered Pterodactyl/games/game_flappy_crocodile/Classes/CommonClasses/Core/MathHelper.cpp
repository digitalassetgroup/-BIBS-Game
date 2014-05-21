#include "MathHelper.h"

#include <math.h>
#include <ctime>

//рандом >= first, но <= second
int Math::RandomRange(int first, int second)
{
	static bool isRandomInited;
	if (!isRandomInited)
	{
		isRandomInited = true;
		srand((unsigned int)time(0));
	}

	if (first == second)
		return first;

	return first + rand()%((second + 1) - first);
}

bool Math::RandomBool()
{
	return RandomRange(1, 100) <= 50;
}

float Math::RandomRangeF(float first, float second)
{
	float t = RandomRange(0, 100) * 0.01f;
	float result = Lerp(first, second, t);
	return result;
}

int Math::RandomFromCenter(int centerValue, int variance)
{
	int result = Math::RandomRange(centerValue - variance, centerValue + variance);
	return result;
}

float Math::RandomFromCenterF(float centerValue, float variance)
{
	float result = Math::RandomRangeF(centerValue - variance, centerValue + variance);
	return result;
}

float Math::RoundF(float source)  //округление до ближайшего целого
{
	float min = floor(source);
	return (source - min) < 0.5 ? min : min + 1;
}

//возвращает максимальное целое число, которое меньше или равно source
//т.е. возвращает целую часть source
//то же самое, что и floor
float Math::TruncF(float source)
{
	float trunk = floor(source);
	return trunk;
}

float Math::CeilF(float source)
{
	float result = ceil(source);
	return result;
}

//вернуть n-ную (с конца) цифру числа number
//n - начиная с нуля
int Math::GetDigit(int number, int n)
{
	int r1 = number/((int)pow(10.0, n));
	int r2 = r1%10;
	return r2;
}

//Returns the median of the values, that is, the middle value. 
float Math::Median(float* source, int N)
{
	if (N == 0)
		return 0;

	float max = source[0];
	float min = source[0];

	//определяем минимум и максимум
	for (int i = 1; i < N; ++i)
	{
		if (source[i] > max) max = source[i];
		if (source[i] < min) min = source[i];
	}
	
	float middleWanted = (min + max)/2.0f; //серединное значение массива
	float middleReal = source[0];
	for (int i = 1; i < N; ++i)
	{
		if (Math::Abs(source[i] - middleWanted) < Math::Abs(middleReal - middleWanted))
			middleReal = source[i];
	}

	return middleReal;
}

//пересекаются ли отрезки [start1; end1] и [start2; end2]
bool Math::IsIntersects(float start1, float end1, float start2, float end2)
{
	if (&(MinByRef(start1, end1)) != &start1)
		Swap(start1, end1);

	if (&(MinByRef(start2, end2)) != &start2)
		Swap(start2, end2);

	return Digit(start1).IsBetween(start2, end2) || Digit(end1).IsBetween(start2, end2)
		|| Digit(start2).IsBetween(start1, end1) || Digit(end2).IsBetween(start1, end1);
}

//Прибоавить к value значение delta. Если произошел выход за пределы диапазона [min;max], то идем по кругу)
int Math::RepeatInRange(int value, int delta, int min, int max)
{
	value += delta;
	if (value > max) value = min;
	if (value < min) value = max;
	return value;
}

#pragma region From Unity

//Clamps a value between a minimum float and maximum float value
float Math::Clamp(float value, float min, float max)
{
    if (value < min)
		value = min;
    else if (value > max)
		value = max;
    return value;
}

int Math::Clamp(int value, int min, int max)
{
    if (value < min)
		value = min;
    else if (value > max)
		value = max;
    return value;
}

//Clamps value between 0 and 1 and returns value
float Math::Clamp01(float value)
{
    if (value < 0.0)
		return 0.0f;
    if (value > 1.0)
		return 1.0f;
    else
		return value;
}

//Interpolates a towards b by t. t is clamped between 0 and 1.
//When t = 0 returns from. When t = 1 return to. When t = 0.5 returns the average of a and b.
float Math::Lerp(float from, float to, float t)
{
    return from + (to - from) * Math::Clamp01(t);
}

//Same as Lerp but makes sure the values interpolate correctly when they wrap around 360 degrees.
//Variables a and b are assumed to be in degrees.
float Math::LerpAngle(float a, float b, float t)
{
    float num = Math::Repeat(b - a, 360.0f);
    if (num > 180.0)
		num -= 360.0f;
    return a + num * Math::Clamp01(t);
}

//Moves a value current towards target.
//This is essentially the same as Mathf.Lerp but instead the function will ensure that 
//the speed never exceeds maxDelta. Negative values of maxDelta pushes the value away from target
float Math::MoveTowards(float current, float target, float maxDelta)
{
    if (Math::Abs(target - current) <= maxDelta)
		return target;
    else
		return current + Math::Sign(target - current) * maxDelta;
}

//Same as MoveTowards but makes sure the values interpolate correctly when they wrap around 360 degrees.
//Variables current and target are assumed to be in degrees. For optimization reasons, 
//negative values of maxDelta are not supported and may cause oscillation. 
//To push current away from a target angle, add 180 to that angle instead.
float Math::MoveTowardsAngle(float current, float target, float maxDelta)
{
    target = current + Math::DeltaAngle(current, target);
    return Math::MoveTowards(current, target, maxDelta);
}

//Interpolates between min and max and eases in and out at the limits.
float Math::SmoothStep(float from, float to, float t)
{
    t = Math::Clamp01(t);
    t = (float)(-2.0 * t * t * t + 3.0 * t * t);
    return (float)(to * t + from * (1.0 - t));
}

float Math::Gamma(float value, float absmax, float gamma)
{
    bool flag = false;
    if (value < 0.0)
		flag = true;
    float num1 = Math::Abs(value);
    if (num1 > absmax)
    {
		if (flag)
			return -num1;
		else
			return num1;
    }
    else
    {		
		float num2 = pow(num1 / absmax, gamma) * absmax;
		if (flag)
			return -num2;
		else
			return num2;
    }
}

//Compares two floating point values if they are similar.
//Due to floating point imprecision it is not recommended to compare floats using the equal operator. 
//eg. 1.0 == 10.0 / 10.0 might not return true.
bool Math::Approximately(float a, float b)
{
    return Math::Abs(b - a) < Math::Max(1E-06f * Math::Max(Math::Abs(a), Math::Abs(b)), 1.121039E-44f);
}

float Math::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float deltaTime)
{
    float maxSpeed = 1000000000.0f; //плюс-бесконечность
    return Math::SmoothDamp(current, target, currentVelocity, smoothTime, maxSpeed, deltaTime);
}

//Gradually changes a value towards a desired goal over time.
//The value is smoothed by some spring-damper like function, which will never overshoot. 
//The function can be used to smooth any kind of value, positions, colors, scalars.
//current			The current position. 
//target			The position we are trying to reach. 
//currentVelocity	The current velocity, this value is modified by the function every time you call it. 
//smoothTime		Approximately the time it will take to reach the target. A smaller value will reach the target faster. 
//maxSpeed			Optionally allows you to clamp the maximum speed. 
//deltaTime			The time since the last call to this function. By default Time.deltaTime.
float Math::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
{
    smoothTime = Math::Max(0.0001f, smoothTime);
    float num1 = 2.0f / smoothTime;
    float num2 = num1 * deltaTime;
    float num3 = (float) (1.0 / (1.0 + (double) num2 + 0.479999989271164 * (double) num2 * (double) num2 + 0.234999999403954 * (double) num2 * (double) num2 * (double) num2));
    float num4 = current - target;
    float num5 = target;
    float max = maxSpeed * smoothTime;
    float num6 = Math::Clamp(num4, -max, max);
    target = current - num6;
    float num7 = (currentVelocity + num1 * num6) * deltaTime;
    currentVelocity = (currentVelocity - num1 * num7) * num3;
    float num8 = target + (num6 + num7) * num3;
    if (num5 - current > 0.0 == num8 > num5)
    {
		num8 = num5;
		currentVelocity = (num8 - num5) / deltaTime;
    }
    return num8;
}

float Math::SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float deltaTime)
{
    float maxSpeed = 1000000000.0f; //плюс-бесконечность
    return Math::SmoothDampAngle(current, target, currentVelocity, smoothTime, maxSpeed, deltaTime);
}

//Gradually changes an angle given in degrees towards a desired goal angle over time.
//The value is smoothed by some spring-damper like function. The function can be used to smooth 
//any kind of value, positions, colors, scalars. The most common use is for smoothing a follow camera.
float Math::SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
{
    target = current + Math::DeltaAngle(current, target);
    return Math::SmoothDamp(current, target, currentVelocity, smoothTime, maxSpeed, deltaTime);
}

//Loops the value t, so that it is never larger than length and never smaller than 0.
//__example:
//__Set the x position to loop between 0 and 3
//__transform.position = Vector3(Mathf.Repeat(Time.time, 3), transform.position.y, transform.position.z);
float Math::Repeat(float t, float length)
{
	return t - floor(t / length) * length;
}

//PingPongs the value t, so that it is never larger than length and never smaller than 0.
//The returned value will move back and forth between 0 and length.
float Math::PingPong(float t, float length)
{
    t = Math::Repeat(t, length * 2.0f);
    return length - Math::Abs(t - length);
}

//Calculates the Lerp parameter between of two values
float Math::InverseLerp(float from, float to, float value)
{
    if (from < to)
    {
		if (value < from)
			return 0.0f;
		if (value > to)
			return 1.0f;
		value -= from;
		value /= to - from;
		return value;
    }
    else
    {
		if (from <= to)
			return 0.0f;
		if (value < to)
			return 1.0f;
		if (value > from)
			return 0.0f;
		else
			return (float)(1.0 - (value - to)/(from - to));
    }
}

//вернуть угол от -180 до 180 градусов
float Math::GetNormalizedAngle(float angle)
{
	float num = Math::Repeat(angle, 360.0f);
    if (num > 180.0f)
		num -= 360.0f;
    return num;
}

//Calculates the shortest difference between two given angles
float Math::DeltaAngle(float current, float target)
{
    float num = Math::Repeat(target - current, 360.0f);
    if (num > 180.0f)
		num -= 360.0f;
    return num;
}


//Interpolates between min and max and eases in at the limits.
float Math::EasyIn(float from, float to, float t, float rate)
{
	float tReal = powf(t, rate);
	float result = Math::Lerp(from, to, tReal);
    return result;
}

float Math::EasyOut(float from, float to, float t, float rate)
{
	float tReal = powf(t, 1.0f/rate);
	float result = Math::Lerp(from, to, tReal);
    return result;
}

float Math::EasyInOut(float from, float to, float t, float rate)
{
	int r = (int)rate;
	int sign = (r % 2 == 0) ? -1 : 1;
	t = t * 2;

	float tReal = (t < 1.0f) 
		? 0.5f * powf(t, rate)
		: 0.5f * sign * (powf(t - 2, rate) + sign*2);
	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseExponentialIn(float from, float to, float t, float rate)
{
	float tReal = (t == 0.0f) ? 0 : powf(2, 10 * (t/1 - 1)) - 1 * 0.001f;
	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseExponentialOut(float from, float to, float t, float rate)
{
	float tReal = (t == 1.0f) ? 1 : (-powf(2, -10 * t / 1) + 1);
	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseExponentialInOut(float from, float to, float t, float rate)
{
	t = t * 2;
	float tReal = (t < 1) 
		? 0.5f * powf(2, 10 * (t - 1))
		: 0.5f * (-powf(2, 10 * (t - 1)) + 2);
	float result = Math::Lerp(from, to, tReal);
	return result;
}

//period = 0.3f
float Math::EaseElasticIn(float from, float to, float t, float period)
{
	float tReal = 0;
	if (t == 0 || t == 1)
	{
		tReal = t;
	}
	else
	{
		float s = period / 4;
		t = t - 1;
		tReal = -powf(2, 10 * t) * sinf((t - s) * Math::PIx2 / period);
	}

	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseElasticOut(float from, float to, float t, float period)
{
	float tReal = 0;
	if (t == 0 || t == 1)
	{
		tReal = t;
	}
	else
	{
		float s = period / 4;
		tReal = powf(2, -10 * t) * sinf((t - s) * Math::PIx2 / period) + 1;
	}

	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseElasticInOut(float from, float to, float t, float period)
{
	float tReal = 0;
	if (t == 0 || t == 1)
	{
		tReal = t;
	}
	else
	{
		t = t * 2;
		if (!period)
		{
			period = 0.3f * 1.5f;
		}

		float s = period / 4;

		t = t - 1;
		if (time < 0)
		{
			tReal = -0.5f * powf(2, 10 * t) * sinf((t - s) * Math::PIx2 / period);
		}
		else
		{
			tReal = powf(2, -10 * t) * sinf((t - s) * Math::PIx2 / period) * 0.5f + 1;
		}
	}

	float result = Math::Lerp(from, to, tReal);
	return result;
}

static float bounceTime(float t)
{
	if (t < 1 / 2.75)
	{
		return 7.5625f * t * t;
	} 
	else if (t < 2 / 2.75)
	{
		t -= 1.5f / 2.75f;
		return 7.5625f * t * t + 0.75f;
	} 
	else if(t < 2.5 / 2.75)
	{
		t -= 2.25f / 2.75f;
		return 7.5625f * t * t + 0.9375f;
	}

	t -= 2.625f / 2.75f;
	return 7.5625f * t * t + 0.984375f;
}

float Math::EaseBounceIn(float from, float to, float t)
{
	float tReal = 1 - bounceTime(1 - t);
	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseBounceOut(float from, float to, float t)
{
	float tReal = bounceTime(t);
	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseBounceInOut(float from, float to, float t)
{
	float tReal = 0;
	if (t < 0.5f)
	{
		t = t * 2;
		tReal = (1 - bounceTime(1 - t)) * 0.5f;
	}
	else
	{
		tReal = bounceTime(t * 2 - 1) * 0.5f + 0.5f;
	}

	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseBackIn(float from, float to, float t)
{
	const float overshoot = 1.70158f;
	float tNew = t * t * ((overshoot + 1) * t - overshoot);
	float result = Math::Lerp(from, to, tNew);
	return result;
}

float Math::EaseBackOut(float from, float to, float t)
{
	const float overshoot = 1.70158f;
	t = t - 1;
	float tReal = t * t * ((overshoot + 1) * t + overshoot) + 1;
	float result = Math::Lerp(from, to, tReal);
	return result;
}

float Math::EaseBackInOut(float from, float to, float t)
{
	const float overshoot = 1.70158f * 1.525f;

	float tReal = 0;
	t = t * 2;
	if (t < 1)
	{
		tReal = (t * t * ((overshoot + 1) * t - overshoot)) / 2;
	}
	else
	{
		t = t - 2;
		tReal = (t * t * ((overshoot + 1) * t + overshoot)) / 2 + 1;
	}

	float result = Math::Lerp(from, to, tReal);
	return result;
}

#pragma endregion

#if UNIT_TEST == 1
	static bool unittest()
	{
		bool isNeedTrue = false;
		bool isNeedTrue2 = false;
		bool isNeedTrue3 = false;
		bool isNeedTrue4 = false;

		isNeedTrue = Math::IsIntersects(40, 50, 45, 55);
		isNeedTrue2 = Math::IsIntersects(50, 40, 45, 55);
		isNeedTrue3 = Math::IsIntersects(40, 50, 55, 45);
		isNeedTrue4 = Math::IsIntersects(50, 40, 55, 45);

		float res = Math::EasyInOut(0, 1, 0.54f, 3);

		float source1[] = { 1, 2, 6, 7, 3 };
		bool isNeedTrue6 = Math::Median(source1) == 3;

		return true;
	}

	static bool isR = unittest();
#endif

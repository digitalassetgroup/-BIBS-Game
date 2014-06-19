#pragma once
#ifndef MathHelper_h
#define MathHelper_h

#include <stdlib.h>
#include <math.h>

namespace Math
{
    const float ConstPI = 3.14159265358979323846f; //чтобы не конфликтовало с #define PI 
	const float PIx2 = ConstPI * 2.0f;
    const float Deg2Rad =  0.01745329252f; //PI/180
    const float Rad2Deg = 57.29577951f; //PI*180
    const float Epsilon = 0.000000119209289550781250000f; /* about pow(2, -23) */;
	const float NormalFps = 60.0f;
	const float SecondDivide60 = 0.01666666667f; //1/60 секунды
	const float SecondDivide30 = SecondDivide60 * 2; //1/30 секунды
	const float SecondDivide20 = SecondDivide60 * 3; //1/20 секунды

	inline double Sqr(double x) { return x*x; }
	inline float SqrF(float x) { return x*x; }

	inline float DegreesToRadians(float angle) { return angle * Deg2Rad; }
	inline float RadiansToDegrees(float angle) { return angle * Rad2Deg; }	

	inline int Abs(int num) { return (num < 0) ? -num : num; }
	inline float Abs(float num) { return (num < 0) ? -num : num; }

	//вернуть угол прямой, образованный точкой p с началом координат
	template<typename TPoint>
	float GetAngleOfPoint(const TPoint& p)
	{
		return RadiansToDegrees(atan2(p.y, p.x));
	}

	inline float SignFloat(float source)
	{
		if (source > 0.0f)
			return 1.0f;
		return (source < 0) ? -1.0f : 0.0f;
	}

	inline int Sign(float source)
	{
		if (source > 0)
			return 1;
		return (source < 0) ? -1 : 0;
	}
	
	bool RandomBool();
	int RandomRange(int first, int second);
	float RandomRangeF(float first, float second);
	int RandomFromCenter(int centerValue, int variance);
	float RandomFromCenterF(float centerValue, float variance);
	float RoundF(float source);  //округление до ближайшего целого
	inline int RoundI(float source) { return (int)RoundF(source); }
	float TruncF(float source);
	float CeilF(float source);
	inline bool IsNumEquals(float num1, float num2) { return Abs(num1 - num2) <= Epsilon; }
	inline bool IsNumEquals(float num1, float num2, float k) { return Abs(num1 - num2) <= powf(0.1f, k); }
	inline bool IsZero(float num) { return Abs(num) <= Epsilon; }
	int GetDigit(int number, int n);

	float Median(float* source, int N);
	template<int N> inline float Median(float (&source)[N]) { return Median(source, N); }

	/// <summary>
	/// source больше ли n1 (включительно) и меньше n2 (не включительно)
	/// </summary>
	template<typename T>
	inline bool IsBetweenExcludeLast(T source, T n1, T n2)
	{
		return source >= n1 && source < n2;
	}
	
	template<typename T>
	inline bool IsBetween(T source, T n1, T n2)
	{
		return source >= n1 && source <= n2;
	}

	template<typename T>
	inline T& MinByRef(T& n1, T& n2) { return n1 < n2 ? n1 : n2; }

	template<typename T>
	inline T Min(T n1, T n2) { return n1 < n2 ? n1 : n2; }
	template<typename T>
	inline T Max(T n1, T n2) { return n1 > n2 ? n1 : n2; }
	template<typename T>
	inline void Swap(T& n1, T& n2) 
	{ 
		T temp = n1; 
		n1 = n2; 
		n2 = temp;
	}

	//пересекаются ли отрезки [start1; end1] и [start2; end2]
	bool IsIntersects(float start1, float end1, float start2, float end2);

	inline float RoundToNearest(float source, float n)
	{
		return n * RoundF(source/n);
	}
	
	/// <summary>
	/// Округлить source до ближайшего, кратного n, начиная с begin
	/// Например: 61, начиная с 25, до ближайшего, кратного 40 = 65
	/// </summary>
	inline float RoundToNearest(float source, float begin, float n)
	{
		return n * RoundF((source - begin)/n) + begin;
	}

	int RepeatInRange(int value, int delta, int min, int max);

	float Clamp(float value, float min, float max);
	int Clamp(int value, int min, int max);
	float Clamp01(float value);
	float Lerp(float from, float to, float t);
	float LerpAngle(float a, float b, float t);
	float MoveTowards(float current, float target, float maxDelta);
	float MoveTowardsAngle(float current, float target, float maxDelta);
	float SmoothStep(float from, float to, float t);
	float Gamma(float value, float absmax, float gamma);
	bool Approximately(float a, float b);
	float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float deltaTime);
	float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
	float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float deltaTime);
	float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
	float Repeat(float t, float length);
	float PingPong(float t, float length);
	float InverseLerp(float from, float to, float value);
	float DeltaAngle(float current, float target);
	float GetNormalizedAngle(float angle);

	float EasyIn(float from, float to, float t, float rate);
	float EasyOut(float from, float to, float t, float rate);
	float EasyInOut(float from, float to, float t, float rate);
	float EaseExponentialIn(float from, float to, float t, float rate);
	float EaseExponentialOut(float from, float to, float t, float rate);
	float EaseExponentialInOut(float from, float to, float t, float rate);
	float EaseElasticIn(float from, float to, float t, float period);
	float EaseElasticOut(float from, float to, float t, float period);
	float EaseElasticInOut(float from, float to, float t, float period);
	float EaseBounceIn(float from, float to, float t);
	float EaseBounceOut(float from, float to, float t);
	float EaseBounceInOut(float from, float to, float t);
	float EaseBackIn(float from, float to, float t);
	float EaseBackOut(float from, float to, float t);
	float EaseBackInOut(float from, float to, float t);
}


#pragma region Digit

template<typename T>
struct DigitStruct
{
private: 
	T _digit;
	
public:
	DigitStruct(T digit) : _digit(digit) { }
	bool IsBetweenExcludeLast(T n1, T n2) { return Math::IsBetweenExcludeLast(_digit, n1, n2); }	
	bool IsBetween(T n1, T n2) { return Math::IsBetween(_digit, n1, n2); }
    
	float RoundToNearest(float n) { return Math::RoundToNearest(_digit, n); }	
	float RoundToNearest(float begin, float n) { return Math::RoundToNearest(_digit, begin, n); }
};

template<typename T>
inline DigitStruct<T> Digit(T digit) { return DigitStruct<T>(digit); }

#pragma endregion

#endif

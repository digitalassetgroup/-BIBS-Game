#include "TotalTimeCounter.h"
#include <platform/platform.h>
#include <CCDirector.h>
#include "LogHelper.h"

float TotalTimeCounter::_totalTime = 0.0f;
int TotalTimeCounter::_frameNumber = 0;
float TotalTimeCounter::_deltaTime = 0;
float TotalTimeCounter::_timeScale = 1;

using namespace cocos2d;

//Установить режим скейла deltaTime (а-ля Unity)
void TotalTimeCounter::SetupScaleDeltaTime(float timeScale)
{
	_timeScale = timeScale;
	CCDirector::sharedDirector()->SetDeltaTimePreprocessing(&TotalTimeCounter::ScaleTime);
}

//Установить режим нормализации deltaTime
void TotalTimeCounter::SetupNormalizeDeltaTime()
{
	CCDirector::sharedDirector()->SetDeltaTimePreprocessing(&TotalTimeCounter::NormalizeDeltaTime);
}

//вернуть текущее время
//для замеров производительности
float TotalTimeCounter::GetRealTime()
{
	cc_timeval now;
	if (CCTime::gettimeofdayCocos2d(&now, NULL) != 0)
		return 0; //error

	float result = now.tv_sec%1000 + now.tv_usec / 1000000.0f;
	return result;
}

//если в текущий момент времени фпс как бы меньше 20, то нормализуем deltу
float TotalTimeCounter::NormalizeDeltaTime(ccTime deltaTime)
{
	if (deltaTime > Math::SecondDivide60 * 3)
	{
		//MY_LOG("fps like " << (1/deltaTime));
		deltaTime = Math::SecondDivide60 * 3;
	}

	return deltaTime;
}

float TotalTimeCounter::ScaleTime(ccTime deltaTime)
{
	return _timeScale * NormalizeDeltaTime(deltaTime);
}

//Update вызывается только тогда, когда нет паузы
void TotalTimeCounter::Update(ccTime deltaTime)
{
	_deltaTime = deltaTime;
	_totalTime += deltaTime;
	++_frameNumber;
}

//Вернуть приращение (то есть скорость, НЕ ускорение) в зависимости от текущего deltaTime
//value - это приращение при FPS = 60
//return - приращение при FPS = 1/deltaTime
float TotalTimeCounter::GetDeltaValueByDeltaTime(float value, ccTime deltaTime)
{
	if (Math::IsZero(deltaTime))
		return value * Math::Epsilon;

	float deltaValue = value * deltaTime * Math::NormalFps; //(deltaTime/Math::SecondDivide60);
	return deltaValue;
}

void TotalTimeCounter::MeasureFunction(const QFunction<void()>& funcToMeasure)
{
	const float t1 = GetRealTime();
	{
		funcToMeasure();
	}
	const float t = GetRealTime() - t1;
	MY_LOG("TimeToLoad = " << t);
}
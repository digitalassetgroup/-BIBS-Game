#pragma once
#ifndef TotalTimeCounter_h
#define TotalTimeCounter_h

#include "Core/CommonMacros.h"
#include "CorePlatform/CorePlatformAll.h"
#include "Core/QFunction.h"
using namespace cocos2d;
class GlobalUpdater;

class TotalTimeCounter
{
	friend class GlobalUpdater;

	private_ static int _frameNumber;   //номер кадра
	private_ static float _totalTime;   //время в секундах, прошедшее с начала игры
	private_ static float _deltaTime;
	private_ static float _timeScale;

	public_ static int GetFrameNumber() { return _frameNumber; }
	public_ static float GetTotalTime() { return _totalTime; }
	public_ static float GetDeltaTime() { return _deltaTime; }

	public_ static void SetupScaleDeltaTime(float timeScale);
	public_ static void SetupNormalizeDeltaTime();	

	private_ static float ScaleTime(ccTime deltaTime);
	private_ static float NormalizeDeltaTime(ccTime deltaTime);
	public_ static float GetRealTime();
	private_ static void Update(ccTime deltaTime);

	public_ static float GetDeltaValueByDeltaTime(float value, ccTime deltaTime);
	public_ static void MeasureFunction(const QFunction<void()>& funcToMeasure);
};

#endif

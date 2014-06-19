#pragma once
#ifndef CoroutineSecondsGenerator_h
#define CoroutineSecondsGenerator_h

#include "Core/CoroutineGenerator.h"
#include "TotalTimeCounter.h"

class YieldInstruction
{ 
	public_ static YieldInstruction WaitForSeconds(float numberOfSeconds)
	{
		auto result = YieldInstruction(false);
		result._numberOfSeconds = numberOfSeconds;
		return result;
	}

	private_ bool _isEnd;
	public_ bool IsEnd() { return _isEnd; }
	private_ float _numberOfSeconds;
	public_ float GetNumberOfSeconds() { return _numberOfSeconds; }

	public_ YieldInstruction(bool isEnd) : _isEnd(isEnd), _numberOfSeconds() { }
};

inline YieldInstruction WaitForSeconds(float numberOfSeconds)
{
	return YieldInstruction::WaitForSeconds(numberOfSeconds);
}


class BoolGeneratorWithSeconds : public BoolGenerator
{
	private_ float _timeToResume;

	public_ BoolGeneratorWithSeconds() 
		: _timeToResume()
	{
	}

	public_ bool operator()(float deltaTime)
	{
		if (_timeToResume == 0 || TotalTimeCounter::GetTotalTime() < _timeToResume)
			return false;

		auto inst = Generate(deltaTime);
		if (inst.GetNumberOfSeconds() != 0)
			_timeToResume = TotalTimeCounter::GetTotalTime() + inst.GetNumberOfSeconds();

		return inst.IsEnd();
	}

	protected_ virtual YieldInstruction Generate(float deltaTime) = 0;
};


//пока не заюзан >_<
class Sample1111 : public BoolGeneratorWithSeconds
{
	private_ YieldInstruction Generate(float deltaTime) GEN_BEGIN_METHOD
	{
		yield_return(false);
		yield_return(WaitForSeconds(2));
	}
	GEN_END
};

#endif

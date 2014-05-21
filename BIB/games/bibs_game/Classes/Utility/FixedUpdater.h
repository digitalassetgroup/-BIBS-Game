#pragma once
#ifndef FixedUpdater_h
#define FixedUpdater_h

#include "Core/QFunction.h"

class FixedUpdater
{
private:
	float _timeForFixedUpdate;

public:
	FixedUpdater() 
		: _timeForFixedUpdate(0)
	{
	}

	void Update(float dt, const QFunction<void(float)>& fixedUpdate)
	{
		//fixed update - http://gafferongames.com/game-physics/fix-your-timestep/
		const float dtFixed = 1.0f/60;
		_timeForFixedUpdate += dt;
		while (_timeForFixedUpdate >= dtFixed)
		{
			fixedUpdate(dtFixed);
			_timeForFixedUpdate -= dtFixed;
		}
	}
};

#endif
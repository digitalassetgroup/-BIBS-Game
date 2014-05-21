#include "FixedUpdater.h"

FixedUpdater::FixedUpdater()
	: _timeForFixedUpdate(0)
{
}

bool FixedUpdater::Update(float dt)
{
	bool result = false;

	const float dtFixed = 1.0f/60;
	_timeForFixedUpdate += dt;
	while (_timeForFixedUpdate > 0)
	{
		_timeForFixedUpdate -= dtFixed;
			
		result = FixedUpdate(dtFixed);
	}

	return result;
}
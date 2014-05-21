#ifndef FixedUpdater_h
#define FixedUpdater_h

#include "Core/CommonMacros.h"

//for update physics, for example
class FixedUpdater
{
	private_ float _timeForFixedUpdate;

	protected_ FixedUpdater();
	public_ bool Update(float dt);
	protected_ virtual bool FixedUpdate(float dt) = 0;

	public_ virtual ~FixedUpdater() {}
};

#endif

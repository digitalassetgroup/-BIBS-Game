#pragma once
#ifndef TouchEvent_h
#define TouchEvent_h

#include "DragonVec2.h"

namespace ig_interface
{
	namespace TouchPhase
	{
		enum Phase
		{
			None = 0,
			Began,
			Ended,
			Moved
		};
	}


	struct TouchEvent 
	{
		TouchEvent() : phase(TouchPhase::None) {}
		TouchEvent(TouchPhase::Phase phase_) : phase(phase_) {}

		TouchPhase::Phase phase;
		DragonVec2 atScreen;
		DragonVec2 atWorld;
	};
}

#endif

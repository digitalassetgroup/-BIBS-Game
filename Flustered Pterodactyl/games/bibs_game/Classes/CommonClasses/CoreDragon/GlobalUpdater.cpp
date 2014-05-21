#include "GlobalUpdater.h"
#include "Dragon/effects/effect_manager.h"
#include "Core/ObserverEvent.h"
#include "CorePlatform/TotalTimeCounter.h"

GlobalUpdater& GlobalUpdater::Inst()
{
	static GlobalUpdater inst;
	static bool isInited = false;
	if (!isInited)
	{
		//-100 - чем ниже приоритет, тем раньше выполняется 
		CCScheduler_SHARED->scheduleUpdateForTarget(&inst, -100, false);
		isInited = true;
	}
	return inst;
}

void GlobalUpdater::update(ccTime deltaTime)
{
	//call NormalizeDeltaTime only on root node - done!!
	TotalTimeCounter::Update(deltaTime);
	g_effectMgr->update(deltaTime);

	EventLocator::Inst().Notify<GlobalUpdater>(deltaTime);
}
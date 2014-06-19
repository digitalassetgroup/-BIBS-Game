#include "SceneManager.h"
#include <cocos2d.h>
#include "CorePlatform/TotalTimeCounter.h"
#include "CorePlatform/ScreenLogLayer.h"
#include "CoreDragon/GlobalUpdater.h"
#include "Dragon/effects/effect_manager.h"
#include "Dragon/scenes/scene_manager.h"
#include "../FlappyGame/Scenes/FlappyGameplayLayer.h"
using namespace cocos2d;

SceneManager& SceneManager::Inst()
{
	static SceneManager inst;
	return inst;
}

void SceneManager::RunAtFirst()
{
	CCDirector::sharedDirector()->SetOnBeforePurgeDirector(&SceneManager::OnBeforePurgeDirector);
	TotalTimeCounter::SetupNormalizeDeltaTime();

	CEffectManager::createInstance();
	gSceneMgr = new CSceneManager();
	GlobalUpdater::Inst();
	InitLogScreen();

	RunFirstScene(FlappyGameplayLayer::GetScene());
}

//метод, который вызывается до удаления OpenGL view
void SceneManager::OnBeforePurgeDirector()
{
	return; //temp

#ifdef DEBUG
	SAFE_DELETE(gSceneMgr);

	ScreenLogLayer::Release();

	SceneManager::Inst().Destroy();
  	CEffectManager::destroyInstance();
	//remove at the end - creating on assert
	CCScriptEngineManager::purgeSharedManager();

#endif

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
		exit(0);
	#endif
}

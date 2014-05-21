#include "SceneBase.h"
#include "SceneManagerBase.h"
#include "ScreenLogLayer.h"

SceneBase::SceneBase(int sceneType) 
	: _sceneType(sceneType) 
{
}

bool SceneBase::init() 
{
	return CCScene::init(); 
}

void SceneBase::onEnter()
{
	//на входе любой сцены показываем логгер
	CCScene::onEnter();
	if (ScreenLogLayer::Inst().getParent() != NULL)
		ScreenLogLayer::Inst().removeFromParentAndCleanup(false);
	addChild(&ScreenLogLayer::Inst(), 1000);
}

void SceneBase::onExit()
{
	CCScene::onExit();
	//removeChild(&ScreenLogLayer::Inst(), false);
}

SceneBase::~SceneBase()
{
	SceneManagerBase::GetInstance()->RemoveSceneFromStack(this); //на всякий случай
}
#include "SceneManagerBase.h"
#include "SceneBase.h"
#include "ScreenLogLayer.h"
#include "IScenesHideOutShowIn.h"
#include <cocos2d.h>

SceneManagerBase* SceneManagerBase::_inst = NULL;

SceneManagerBase::SceneManagerBase() 
	: _currentSceneType()
{
	_inst = this;
}

SceneManagerBase::~SceneManagerBase()
{
}

void SceneManagerBase::RunFirstScene(SceneBase* scene)
{
	CCDirector::sharedDirector()->runWithScene(scene);
	_scenes.push_back(scene);
  	InputHelper::SetSceneScale(scene);
	scene->OnEnter();
}

//Инициализировать панель для отображения лог
void SceneManagerBase::InitLogScreen()
{
	ScreenLogLayer::Inst().SetVisibleClearLogButton(true);	
}

void SceneManagerBase::RemoveSceneFromStack(SceneBase* scene)
{
	//CollectionHelper::Remove(_scenes, scene);
}

void SceneManagerBase::Destroy()
{
	//мы не юзаем PushScene/PopScene. Только ReplaceScene
    //when deleting scenes, I need in the destructor of scene remove this scene from that stack
	while (!_scenes.empty())
	{
		auto scene = _scenes.back();
		_scenes.pop_back();
		scene->release();
	}
}

void SceneManagerBase::Exit()
{
	//exit from currentScene
	if (!_scenes.empty())
	{
		auto currentScene = _scenes.back();
		currentScene->OnLeave();
	}

	//про выход в андроиде также см. http://www.cocos2d-x.org/boards/6/topics/5327
	CCDirector::sharedDirector()->end();
}

//
void SceneManagerBase::ReplaceSceneInternal(SceneBase* scene, bool isNeedShading, IScenesHideOutShowIn* onHideOutShowIn)
{
	if (isNeedShading)
	{
		auto transition = new CCTransitionFadeFixed(onHideOutShowIn);
		transition->initWithDuration(1, scene);
		CCDirector::sharedDirector()->replaceScene(transition);
		transition->release();
	}
	else
	{
		CCDirector::sharedDirector()->replaceScene(scene);
	}

	scene->release();
}

void SceneManagerBase::ReplaceScene(SceneBase* scene, bool isNeedShading, IScenesHideOutShowIn* onHideOutShowIn)
{
	auto currentScene = _scenes.back();

	ReplaceSceneInternal(scene, isNeedShading, onHideOutShowIn);

	if (_scenes.back() == currentScene)
		_scenes.pop_back();
	_scenes.push_back(scene);
}

void SceneManagerBase::PushScene(SceneBase* newScene, bool isNeedShading, IScenesHideOutShowIn* onHideOutShowIn)
{
	ReplaceSceneInternal(newScene, isNeedShading, onHideOutShowIn);
	_scenes.push_back(newScene);	
	//newScene->release();
}

//return - poppedScene
SceneBase* SceneManagerBase::PopScene(bool isNeedShading)
{
	auto currentScene = _scenes.back();
	_scenes.pop_back();

	if (_scenes.empty())
	{
		Exit();
		return currentScene;
	}

	auto prevScene = _scenes.back();
	
	ReplaceSceneInternal(prevScene, isNeedShading, NULL);
	//CCDirector::sharedDirector()->popScene();
	return currentScene;
}

void SceneManagerBase::PopSceneAndRelease(bool isNeedShading)
{
	auto poppedScene = PopScene(isNeedShading);
	if (poppedScene != NULL)
		poppedScene->release();
}

static const unsigned int TagSceneFade = 0xFADEFADE;

class CCTransitionFadeChild : public CCTransitionFade
{
	public_ virtual void onEnter()
	{
		CCTransitionScene::onEnter();

		CCLayerColor* l = CCLayerColor::create(m_tColor);
		m_pInScene->setVisible(false);

		addChild(l, 2, TagSceneFade);
		CCNode* f = getChildByTag(TagSceneFade);
		CCActionInterval* a = (CCActionInterval*)CCSequence::create(
				CCFadeIn::create(m_fDuration/2),
				CCCallFunc::create(this, callfunc_selector(CCTransitionScene::hideOutShowIn)),
				NULL);
		f->runAction(a);
	}

	public_ virtual void hideOutShowIn()
	{
		CCTransitionFade::hideOutShowIn();

		schedule(schedule_selector(CCTransitionFadeChild::PerformOnLeaveAndOnEnter), 0.01f);
	}

	private_ void PerformOnLeaveAndOnEnter(ccTime dt)
	{
		unschedule(schedule_selector(CCTransitionFadeChild::PerformOnLeaveAndOnEnter));

		((SceneBase*)m_pOutScene)->OnLeave();
		((SceneBase*)m_pInScene)->OnEnter();
		schedule(schedule_selector(CCTransitionFadeChild::RunFadeOut), 0.01f);
	}

	private_ void RunFadeOut(ccTime dt)
	{
		unschedule(schedule_selector(CCTransitionFadeChild::RunFadeOut));

		CCNode* f = getChildByTag(TagSceneFade);
		CCActionInterval* a = (CCActionInterval *)CCSequence::create(
				CCFadeOut::create(m_fDuration/2),
				CCCallFunc::create(this, callfunc_selector(CCTransitionScene::finish)),
				NULL);
		f->runAction(a);
	}
    
    protected_ virtual void finish()
    {
        CCTransitionFade::finish();
        
        InputHelper::SetSceneScale((SceneBase*)m_pInScene);
    }
};


//newScene - scene with retain
void SceneManagerBase::ReplaceSceneWithDefaultTransition(SceneBase* newScene)
{
	SceneBase* currentScene = _scenes.back();
	currentScene->release();

	//
	auto transition = new CCTransitionFadeChild();
	transition->initWithDuration(1, newScene);
	CCDirector::sharedDirector()->replaceScene(transition);
	transition->release();
	//
	//newScene->release();
	InputHelper::SetSceneScale(newScene);
    
	if (_scenes.back() == currentScene)
		_scenes.pop_back();
	_scenes.push_back(newScene);
}

void SceneManagerBase::ReplaceSceneWithNoTransition(SceneBase* newScene)
{
	auto currentScene = _scenes.back();
	currentScene->OnLeave();
	currentScene->release();

	CCDirector::sharedDirector()->replaceScene(newScene);
	//newScene->release();    
   	InputHelper::SetSceneScale(newScene);
	newScene->OnEnter();

	if (_scenes.back() == currentScene)
		_scenes.pop_back();
	_scenes.push_back(newScene);
}

void SceneManagerBase::PushSceneNew(SceneBase* newScene)
{
	CCDirector::sharedDirector()->replaceScene(newScene);
   	InputHelper::SetSceneScale(newScene);
	newScene->OnEnter();

	_scenes.push_back(newScene);
}

void SceneManagerBase::PopSceneNew()
{
	auto currentScene = _scenes.back();
	currentScene->OnLeave();
	currentScene->release();
	_scenes.pop_back();

	auto prevScene = _scenes.back();

	CCDirector::sharedDirector()->replaceScene(prevScene);
   	InputHelper::SetSceneScale(prevScene);
	prevScene->OnSchedule();
}

CCScene* SceneManagerBase::GetCurrentScene()
{
	auto currentScene = _scenes.empty() ? NULL : _scenes.back();
	return currentScene;
}



class CCTransitionFadeInChild : public CCTransitionFade
{
	public_ virtual void onEnter()
	{
		CCTransitionScene::onEnter();

		CCLayerColor* l = CCLayerColor::create(m_tColor);
        l->setOpacity(255);
        
		addChild(l, 2, TagSceneFade);
		CCNode* f = getChildByTag(TagSceneFade);

		hideOutShowIn();

		CCActionInterval* a = (CCActionInterval *)CCSequence::create
			(
				CCFadeOut::create(m_fDuration),
				CCCallFunc::create(this, callfunc_selector(CCTransitionScene::finish)),
				NULL
			);
		f->runAction(a);
	}
};


void SceneManagerBase::RunFirstSceneWithTransitionFadeIn(SceneBase* newScene)
{
    newScene->OnEnter();
    
	auto transition = new CCTransitionFadeInChild();
	transition->initWithDuration(0.5f, newScene);
	CCDirector::sharedDirector()->pushScene(transition);
	transition->release();

	_scenes.push_back(newScene);
}

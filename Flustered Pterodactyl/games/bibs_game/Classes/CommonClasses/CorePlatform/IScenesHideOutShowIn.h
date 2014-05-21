#pragma once
#ifndef IScenesHideOutShowIn_h
#define IScenesHideOutShowIn_h

#include "Core/CoreAll.h"
#include <cocos2d.h>
using namespace cocos2d;
#include "SceneBase.h"
#include "TotalTimeCounter.h"
#include "LogHelper.h"
#include "InputHelper.h"
#include "NodeHelper.h"


//для шейдинга между сценами
//гребаный динамический полиморфизм. происходит срезка, если передавать IScenesHideOutShowIn как копию :'(
//вот что значит, когда нет лямбд с поддержкой замыканий как копий :'( приходится делать мелкие классы
struct IScenesHideOutShowIn
{
	virtual void OnBeforeHideOutShow(SceneBase* sceneOut, SceneBase* sceneIn) {};
	virtual void OnHideOutShowInDefault(SceneBase* sceneOut, SceneBase* sceneIn) = 0;
	virtual void OnShowedIn(SceneBase* sceneOut, SceneBase* sceneIn) {};
	virtual ~IScenesHideOutShowIn() { }
};


struct ScenesHideOutShowInUnloadTexture : public IScenesHideOutShowIn
{
	static void ApplyFuncToAllChildren(CCNode* node, void (CCNode::*func)())
	{
		(node->*func)();
		CCArray_ForEach(CCNode*, child, node->getChildren())
		{
			ApplyFuncToAllChildren(child, func);
		}
		CCArray_ForEachEnd()
	}

	virtual void OnBeforeHideOutShow(SceneBase* sceneOut, SceneBase* sceneIn)
	{
		ApplyFuncToAllChildren(sceneOut, &CCNode::unscheduleAllSelectors); //если это не вызвать, то будет schedulePause
	}

	virtual void OnHideOutShowInDefault(SceneBase* sceneOut, SceneBase* sceneIn)
	{
		//sceneOut->removeAllChildrenr:WithCleanup(true); //!этот метод выполняется 0.5сек! очищаем использование всех текстур в sceneOut
		sceneOut->removeAllChildrenWithCleanup(false);  //этот метод не тормозит
		
		//выгрузка неиспользованных текстур
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
		CCTextureCache::sharedTextureCache()->removeUnusedTextures();	
		CCSpriteFrameCache::purgeSharedSpriteFrameCache();	

		sceneIn->init(); //загрузка текстур для сцены sceneIn
	}
};


class CCTransitionFadeFixed : public CCTransitionFade
{
	private_ CCLayerColor* _layerColor;
	private_ IScenesHideOutShowIn* _onHideOutShowIn;
	public_ CCTransitionFadeFixed(IScenesHideOutShowIn* onHideOutShowIn) 
		: _onHideOutShowIn(onHideOutShowIn), _layerColor()
	{
	}

	public_ ~CCTransitionFadeFixed()
	{
		SAFE_DELETE(_onHideOutShowIn);
		SAFE_DELETE(_layerColor);
	}

	public_ virtual bool initWithDuration(ccTime duration, SceneBase* scene, const ccColor3B& color)
	{
		return CCTransitionFade::initWithDuration(duration, scene, color);
	}

	public_ virtual bool initWithDuration(ccTime duration, SceneBase* scene)
	{
		return CCTransitionFade::initWithDuration(duration, scene);
	}

	private_ float _timeOnBeforeHideOut;

	protected_ virtual void onEnter()
	{
		//CCTransitionFade::onEnter();
		CCScene::onEnter();

		_layerColor = new CCLayerColor();
		_layerColor->initWithColor(m_tColor);

		m_pInScene->setVisible(false);
		addChild(_layerColor, 2);

		_layerColor->runAction((CCActionInterval *)CCSequence::create
			(
				CCFadeIn::create(m_fDuration/2),
				CCCallFunc::create(this, callfunc_selector(CCTransitionFadeFixed::hideOutShowIn)),
				NULL
			));
	}

	private_ void hideOutShowIn()
	{
		_timeOnBeforeHideOut = TotalTimeCounter::GetRealTime();

		m_pOutScene->onExit();
		m_pOutScene->setVisible(false);
		if (_onHideOutShowIn != NULL)
			_onHideOutShowIn->OnBeforeHideOutShow((SceneBase*)m_pOutScene, (SceneBase*)m_pInScene);
		
		//это нежно для того, чтобы перерисовалась сцена хотя бы 1 раз (чтобы с экранов исчезла картинка сцены m_pOutScene)
		schedule(schedule_selector(CCTransitionFadeFixed::hideOutShowIn2), 0.01f);
	}

	private_ void hideOutShowIn2(ccTime deltaTime)
	{		
		unschedule(schedule_selector(CCTransitionFadeFixed::hideOutShowIn2));

		if (_onHideOutShowIn != NULL)
		{
			_onHideOutShowIn->OnHideOutShowInDefault((SceneBase*)m_pOutScene, (SceneBase*)m_pInScene);
			//for (long i = 0; i < 1000000000l*5l; ++i);			
		}

		_layerColor->stopAllActions();
		_layerColor->runAction((CCActionInterval *)CCSequence::create
			(
				CCFadeOut::create(m_fDuration/2),
				CCCallFunc::create(this, callfunc_selector(CCTransitionFadeFixed::Finish)),
				NULL
			));

		m_pInScene->setVisible(true);		
		m_pInScene->onEnter();

		float timeOnAfterHideOut = TotalTimeCounter::GetRealTime() - _timeOnBeforeHideOut;
		MY_LOG_F("elapsed time on loading between scenes = %.2f", timeOnAfterHideOut);
	}

	private_ void Finish()
	{
		if (_onHideOutShowIn != NULL)
		{
			_onHideOutShowIn->OnShowedIn((SceneBase*)m_pOutScene, (SceneBase*)m_pInScene);
			SAFE_DELETE(_onHideOutShowIn);
		}
		CCTransitionScene::finish();

		//восстанавливаем скейл
		InputHelper::SetSceneScale((SceneBase*)m_pInScene);
	}

	protected_ virtual void onExit()
	{
		//CCTransitionFade::onExit();
		CCScene::onExit();
		this->removeChild(_layerColor, false);

		// inScene should not receive the onExit callback
		// only the onEnterTransitionDidFinish
		m_pInScene->onEnterTransitionDidFinish();
	}
};


#endif

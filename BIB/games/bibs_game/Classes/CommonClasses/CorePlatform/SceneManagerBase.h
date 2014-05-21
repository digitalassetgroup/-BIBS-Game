#pragma once
#ifndef SceneManagerBase_h
#define SceneManagerBase_h

#include "Core/CoreAll.h"
class SceneBase;
struct IScenesHideOutShowIn;
namespace cocos2d { class CCScene; }
using namespace cocos2d;


class SceneManagerBase : NonCopyable
{
	protected_ vector<SceneBase*> _scenes;
	private_ int _currentSceneType;

	private_ static SceneManagerBase* _inst;
	public_ static SceneManagerBase* GetInstance() { return _inst; }

	protected_ SceneManagerBase();
	public_ virtual ~SceneManagerBase();

	public_ void SetCurrentSceneType(int sceneType) { _currentSceneType = sceneType; }
	public_ int GetCurrentSceneType() { return _currentSceneType; }	

	protected_ void InitLogScreen();
	protected_ void RunFirstScene(SceneBase* scene);

	protected_ virtual void ReplaceSceneInternal(SceneBase* scene, bool isNeedShading, IScenesHideOutShowIn* onHideOutShowIn);
	public_ void ReplaceScene(SceneBase* scene, bool isNeedShading, IScenesHideOutShowIn* onHideOutShowIn = NULL);
	protected_ void PushScene(SceneBase* newScene, bool isNeedShading, IScenesHideOutShowIn* onHideOutShowIn = NULL);
	protected_ SceneBase* PopScene(bool isNeedShading);
	protected_ void PopSceneAndRelease(bool isNeedShading);
	public_ void RemoveSceneFromStack(SceneBase* scene);

	public_ virtual void Destroy();
	public_ virtual void Exit();

	public_ virtual void ReplaceSceneWithDefaultTransition(SceneBase* newScene);
	public_ void ReplaceSceneWithNoTransition(SceneBase* newScene);
	public_ void PushSceneNew(SceneBase* newScene);
	public_ void PopSceneNew();
	public_ CCScene* GetCurrentScene();

	public_ void RunFirstSceneWithTransitionFadeIn(SceneBase* newScene);

	//
	public_ virtual void OnFocusedOn() {}
	public_ virtual void OnFocusedOff() {}
	public_ virtual void OnEnterForeground() {}
	public_ virtual void OnEnterBackground() {}
};

#endif

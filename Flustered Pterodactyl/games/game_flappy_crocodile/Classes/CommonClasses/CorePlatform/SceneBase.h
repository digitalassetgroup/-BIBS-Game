#pragma once
#ifndef SceneBase_h
#define SceneBase_h

#include "Core/CommonMacros.h"
#include "layers_scenes_transitions_nodes/CCScene.h"
using namespace cocos2d;
class SceneManagerBase;

//все сцены наследовать от этого класса!
class SceneBase : public CCScene
{
	private_ SceneManagerBase* _sceneManager;
	private_ int _sceneType;
	public_ int GetSceneType() { return _sceneType; }

	protected_ SceneBase(int sceneType);
	public_ virtual ~SceneBase();

	public_ virtual bool init();
	public_ virtual void onEnter();
	public_ virtual void onExit();

	//вызываются в SceneManager
	public_ virtual void OnLeave() {};
	public_ virtual void OnEnter() {};
	public_ virtual void OnSchedule() {};
};

#endif

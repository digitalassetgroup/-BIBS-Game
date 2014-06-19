#pragma once
#ifndef TouchableLayerBase_h
#define TouchableLayerBase_h

#include "Core/CommonMacros.h"
#include "CorePlatform/SceneBase.h"
#include <string>
#include <vector>
#include "layers_scenes_transitions_nodes/CCLayer.h"
using std::string;
using std::vector;
class TouchableLayerBase;
using namespace cocos2d;
namespace ig_interface { class Button; class IBtnDelegate; }

namespace TouchType
{
	enum Type
	{
		BeginTouch,
		MoveTouch,
		EndTouch
	};
}


class SceneBaseWithToucheableLayers : public SceneBase
{
	public_ static SceneBase* GetSceneWithLayer(int sceneType, TouchableLayerBase* layer);	

	protected_ vector<TouchableLayerBase*> _touchableLayers;

	public_ SceneBaseWithToucheableLayers(int sceneType);
	public_ ~SceneBaseWithToucheableLayers();
	
	public_ virtual void OnEnter();
	public_ virtual void OnLeave();
	public_ virtual void OnSchedule();
};


class TouchableLayerBase : public CCLayer
{
    private_ CCTouch* _currentTouch;
    
    public_ TouchableLayerBase();
    private_ bool GetCurrentTouch(CCSet* touches, CCPoint& location);
    
	public_ virtual void onEnter();
	public_ virtual void onExit();

	public_ virtual void OnEnter();
	public_ virtual void OnLeave();

	public_ virtual void OnSchedule() {};

	protected_ virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);
	protected_ virtual void ccTouchesMoved(CCSet* touches, CCEvent* event);
	protected_ virtual void ccTouchesEnded(CCSet* touches, CCEvent* event);
	

	protected_ virtual void OnTouch(TouchType::Type touchType, const CCPoint& point) {};
	public_ ig_interface::Button* CreateDefaultButton(ig_interface::IBtnDelegate* deleg, int z, const string& text, int tag = 0);
	protected_ int GetSceneType();
};

#endif

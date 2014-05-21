#pragma once
#ifndef ScreenLogLayer_h
#define ScreenLogLayer_h

#include "Core/CommonMacros.h"
#include "CorePlatform/CorePlatformAll.h"
#include "CorePlatform/ScreenLogListener.h"
#include <layers_scenes_transitions_nodes/CCLayer.h>
using namespace cocos2d;
namespace cocos2d { class CCMenuItemLabel; class CCLabelBMFont; class CCSet; class CCEvent; class CCObject; }


class ScreenLogLayer : NonCopyable, public CCLayer
{
	private_ static ScreenLogLayer* _inst;
	public_ static ScreenLogLayer& Inst();
	public_ static void Release();

	private_ ScreenLogListener _screenLogListener;
	private_ bool _isShowLog;	
	private_ CCLabelBMFont* _labelLog;
	private_ CCMenuItemLabel* _butClearLog;
	private_ CCMenuItemLabel* _butMemoryWarning;

	private_ ScreenLogLayer();
	private_ bool init();
    protected_ virtual void onEnter();
    protected_ virtual void onExit();
	private_ void OnShowLogClicked(CCObject* sender);
	private_ void OnClearLogClicked(CCObject* sender);
	private_ void OnMemoryWarningClicked(CCObject* sender);
	private_ void UpdateLabelLog();
	private_ virtual void update(ccTime dt);
	public_ void ClearLog();
	public_ void SetVisibleClearLogButton(bool isVisible);
	public_ void SetIsShowLog(bool isShowLog);

	protected_ virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);
	protected_ virtual void ccTouchesEnded(CCSet* touches, CCEvent* event);	
};

#endif

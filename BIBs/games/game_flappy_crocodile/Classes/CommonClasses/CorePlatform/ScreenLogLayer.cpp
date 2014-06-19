#include "ScreenLogLayer.h"
#include "InputHelper.h"
#include "SpriteHelper.h"
#include "CorePlatform/CommonProperties.h"
#include <cocos2d.h>


ScreenLogLayer* ScreenLogLayer::_inst = NULL;

void ScreenLogLayer::Release()
{
	if (_inst != NULL)
	{
		_inst->release();
		_inst = NULL;
	}
}

ScreenLogLayer& ScreenLogLayer::Inst()
{
	if (_inst == NULL)
	{
		_inst = new ScreenLogLayer();
		_inst->init();
	}

	return *_inst;
}


ScreenLogLayer::ScreenLogLayer()
	: _isShowLog(false), _butClearLog(), _butMemoryWarning()
{
}

bool ScreenLogLayer::init()
{
	if (!CCLayer::init())
		return false;

	CCSize winSize = InputHelper::GetUnscaledWindowSize();

	//_labelAtlas = CCLabelAtlas::labelWithString("TESTING RETINA\tDISPLAY\r\nsdsdfsd", "fontsEnc/larabie-16.png", 5, 10, 'A');
	_labelLog = CCLabelBMFont::create(string(3000, ' ').c_str(), SpriteHelper::GetFontPath().c_str());
	_labelLog->setColor(/*ccWHITE*/ccRED);
	_labelLog->getTexture()->setAliasTexParameters();
    _labelLog->setAnchorPoint(ccp(0, 1));
	_labelLog->setPosition(ccp(0, winSize.height)); //TODO: надо мерить от центра!!!!!
	addChild(_labelLog, 200);
	_labelLog->setVisible(_isShowLog);
	
	auto anchorPointForMenuItems = ccp(winSize.width, 0);

	auto menuShowLog = SpriteHelper::CreateLabel(this, "SHOW LOG", ccp(1, 0), menu_selector(ScreenLogLayer::OnShowLogClicked));
	menuShowLog->setPosition(InputHelper::GetPointRelativeSomePoint(
		anchorPointForMenuItems, ccp(-190, 10)));

	_butClearLog = SpriteHelper::CreateLabel(this, "CLEAR LOG", ccp(1, 0), menu_selector(ScreenLogLayer::OnClearLogClicked));
	_butClearLog->setPosition(InputHelper::GetPointRelativeSomePoint(
		anchorPointForMenuItems, ccp(-70, 10)));

	_butMemoryWarning = SpriteHelper::CreateLabel(this, "MEMORY WARN", ccp(1, 0), menu_selector(ScreenLogLayer::OnMemoryWarningClicked));
	_butMemoryWarning->setPosition(InputHelper::GetPointRelativeSomePoint(
		anchorPointForMenuItems, ccp(-310, 10)));

	auto menu = SpriteHelper::CreateMenu(this, 100);
	menu->addChild(menuShowLog, 0);
	menu->addChild(_butClearLog, 0);
	menu->addChild(_butMemoryWarning, 0);

	InputHelper::SetTextMenuScale(menuShowLog);
	InputHelper::SetTextMenuScale(_labelLog);	
	SetVisibleClearLogButton(false);

#ifdef DEBUG
	setTouchEnabled(true);	
#else
	this->setVisible(false);
#endif

	return true;
}

void ScreenLogLayer::onEnter()
{
	CCLayer::onEnter();
	scheduleUpdate();
}

void ScreenLogLayer::onExit()
{
    CCLayer::onExit();
    unscheduleUpdate();
}

void ScreenLogLayer::SetIsShowLog(bool isShowLog)
{
	_isShowLog = isShowLog;

	_labelLog->setVisible(_isShowLog);
	if (_isShowLog)
		UpdateLabelLog();
}

void ScreenLogLayer::OnShowLogClicked(CCObject* sender)
{
	SetIsShowLog(!_isShowLog);
}

void ScreenLogLayer::OnClearLogClicked(CCObject* sender)
{
	ClearLog();
}

void ScreenLogLayer::OnMemoryWarningClicked(CCObject* sender)
{
	CCDirector::sharedDirector()->purgeCachedData();
}

void ScreenLogLayer::UpdateLabelLog()
{
	string textToShow;
	_screenLogListener.GetString(textToShow);
	_labelLog->setString(textToShow.c_str());
}

void ScreenLogLayer::update(ccTime dt)
{
	if (_isShowLog && _screenLogListener.IsNeedToUpdateScreen())
	{
		UpdateLabelLog();
	}
}

void ScreenLogLayer::ClearLog()
{
	_screenLogListener.ClearLog();
}

void ScreenLogLayer::SetVisibleClearLogButton(bool isVisible)
{
	_butClearLog->setVisible(isVisible);
	_butMemoryWarning->setVisible(isVisible);
}

void ScreenLogLayer::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if (!CCEGLView::sharedOpenGLView()->IsCtrlPressed())
		return;

    CCTouch* touch = (CCTouch*)touches->anyObject();
	CCPoint location = touch->LOCATION_IN_VIEW(touch);

	auto winSize = InputHelper::GetUnscaledWindowSize();
	location.y = winSize.height - location.y;

	MY_LOG(StringHelper::format("LOC: x = %.2f(%.4f); y = %.2f(%.4f)", location.x, location.x/winSize.width,
		location.y, location.y/winSize.width));
#endif
}

void ScreenLogLayer::ccTouchesEnded(CCSet* touches, CCEvent* event)
{

}
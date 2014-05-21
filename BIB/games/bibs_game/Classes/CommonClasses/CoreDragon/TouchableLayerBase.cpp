#include "TouchableLayerBase.h"
#include "CorePlatform/CorePlatformAll.h"
#include "Dragon/effects/effect_manager.h"
#include "Dragon/ui/ui_button.h"
#include <cocos2d.h>
using namespace ig_interface;

SceneBase* SceneBaseWithToucheableLayers::GetSceneWithLayer(int sceneType, 
	TouchableLayerBase* layer)
{
	auto scene = new SceneBaseWithToucheableLayers(sceneType);
	scene->init();

	scene->addChild(layer);
	layer->init();
	layer->release();
	scene->_touchableLayers.push_back(layer);

	return scene;
}

SceneBaseWithToucheableLayers::SceneBaseWithToucheableLayers(int sceneType) 
	: SceneBase(sceneType) {}

SceneBaseWithToucheableLayers::~SceneBaseWithToucheableLayers()
{
}

void SceneBaseWithToucheableLayers::OnEnter()
{
	for_each(auto layer, _touchableLayers)
	{
		layer->OnEnter();
	}
}

void SceneBaseWithToucheableLayers::OnSchedule()
{
	for_each(auto layer, _touchableLayers)
	{
		layer->OnSchedule();
	}
}

void SceneBaseWithToucheableLayers::OnLeave()
{
	for_each(auto layer, _touchableLayers)
	{
		layer->OnLeave();
	}
}


//---------------- TouchableLayerBase ------------------

TouchableLayerBase::TouchableLayerBase()
    : _currentTouch()
{
}


void TouchableLayerBase::onEnter()
{
	CCLayer::onEnter();

	//OnEnter();
}

void TouchableLayerBase::onExit()
{
	CCLayer::onExit();

	//OnLeave();
}

void TouchableLayerBase::OnEnter()
{
	g_effectMgr->clearAllEffects(false);

	setTouchEnabled(true);
}

void TouchableLayerBase::OnLeave()
{
	setTouchEnabled(false);
	removeAllChildrenWithCleanup(true);
	Button::RegisterWithTouchDispatcherAllButtonsWithParent(false, NULL);

	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();	
	CCSpriteFrameCache::purgeSharedSpriteFrameCache();	
}

//TODO: вынести в InputHelper
static CCPoint GetAnyPoint(CCSet* touches)
{
    CCTouch* touch = (CCTouch*)touches->anyObject();
	CCPoint location = touch->LOCATION_IN_VIEW(touch);
    
    float scaleFactor = CC_CONTENT_SCALE_FACTOR();
    if (scaleFactor != 1.0f)
    {
        location.x *= scaleFactor;
        location.y *= scaleFactor;
    }
    
    return location;
}

//take first touch or only touch that was began!
//I need to do it globally, i.e. only first touch is counted on buttons, other touchable objects
bool TouchableLayerBase::GetCurrentTouch(CCSet* touches, CCPoint& location)
{
    bool isHasCurrentTouch = false;
    
    for (CCSetIterator it = touches->begin(); it != touches->end(); ++it)
    {
        if (!*it)
            continue;
        
        if (_currentTouch == nullptr)
        {
            _currentTouch = (CCTouch*)*it;
            isHasCurrentTouch = true;
            break;
        }
        else if (_currentTouch == *it)
        {
            isHasCurrentTouch = true;
            break;
        }
    }
   
    if (isHasCurrentTouch && _currentTouch != nullptr)
    {
        location = _currentTouch->LOCATION_IN_VIEW(touch);
        return true;
    }
    
    return false;
}

void TouchableLayerBase::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCPoint location;
    if (GetCurrentTouch(touches, location))
        OnTouch(TouchType::BeginTouch, location);
}

void TouchableLayerBase::ccTouchesMoved(CCSet* touches, CCEvent* event)
{
	CCPoint location;
    if (GetCurrentTouch(touches, location))
        OnTouch(TouchType::MoveTouch, location);
}

void TouchableLayerBase::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
	CCPoint location;
    if (GetCurrentTouch(touches, location))
    {
        OnTouch(TouchType::EndTouch, location);    
        _currentTouch = nullptr;
    }
}

ig_interface::Button* TouchableLayerBase::CreateDefaultButton(ig_interface::IBtnDelegate* deleg, int z, const string& text, int tag)
{
	auto button = Button::MakeButtonWithTextures("textures/fon_button.png", "textures/fon_button_push.png", deleg, text);
	button->setTag(tag);
	this->addChild(button, z);
	return button;
}

int TouchableLayerBase::GetSceneType()
{
	return static_cast<SceneBase*>(getParent())->GetSceneType();
}
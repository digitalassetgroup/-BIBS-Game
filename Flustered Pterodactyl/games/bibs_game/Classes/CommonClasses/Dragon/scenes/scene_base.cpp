#include "scene_base.h"
//#include "../../cannonboy.h"
//#include "../core/camera.h"
#include "../ui/ig_interface.h"
//#include "../objects/obj_base.h"
#include "../effects/effect_manager.h"	/* Included because CBaseScene::tick() serves as the main loop. */
//#include "SpriteHelperLoader.h"

#include "scene_assert.h"
#include "../utilities/ig_utilities.h"
using namespace ig_interface;

//#include "debug.h"

using namespace cocos2d;
using namespace DracoGames::Utils;
using namespace ig_interface;

CBaseScene::CBaseScene(void) : 
	m_backButton(nullptr),
	m_backDrop(nullptr),
	m_dbgFirstFrame(true)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBaseScene::~CBaseScene(void)
{
	removeAllChildrenWithCleanup(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::OnUpdate(float deltaTime)
{
	IG_UNUSED(deltaTime);

	/* Reset the projection matrix to avoid any possible Z-fighting/flickering issues.  -TIM 8/6/12 */
    CCDirector::sharedDirector()->setProjection(kCCDirectorProjection2D);
	
	if (false && true == m_dbgFirstFrame/* && true == IsDevBuild*/)
	{	/* Add a debug grid to each scene so we can see the differences. */
		addChild(ScrollingBackdrop::makeDebugGridBackdrop());
	}
	
	m_dbgFirstFrame = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::OnDraw(void) const
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::OnTouch(const TouchEvent& evt)
{
	IG_UNUSED(evt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::onTouch(Button* const target, const TouchEvent& evt)
{	/* Base implementation does nothing. */
	IBtnDelegate::onTouch(target, evt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::onClickAction(Button* const target)
{
	if(target == m_backButton)
	{
		printf("Back button was pressed. Going to scene: %s\n", m_previousSceneName.c_str());
		assert_if("scene_gameplay" == m_previousSceneName, "Gameplay scene can not be transitioned into.");
		//gSceneMgr->FadeTransitionToScene(m_previousSceneName, gSceneMgr->FastTransitionTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::OnEnter(CBaseScene* prevScene)
{
	printf("Entering scene: %s\n", GetSceneName().c_str());

	m_previousSceneName = "";
	if(nullptr != prevScene)
	{
		m_previousSceneName = prevScene->GetSceneName();
	}

    setTouchEnabled(true);
 	SET_IS_ACCELEROMETER_ENABLED(false);
	schedule(schedule_selector(CBaseScene::tick));	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::OnLeave(CBaseScene* nextScene)
{
	IG_UNUSED(nextScene);
	printf("Leaving scene: %s\n", GetSceneName().c_str());
	
	setTouchEnabled(false);
	SET_IS_ACCELEROMETER_ENABLED(false);
	unschedule(schedule_selector(CBaseScene::tick));	
		
	removeAllChildrenWithCleanup(true);
	ig_interface::Button::checkButtonsForAutoRelease();
	
	/* The button just auto-released itself. */
	m_backButton = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::makePreviousSceneButton(const std::string& toPrevScene)
{
	assert_if(nullptr != m_backButton, "Cannot make a back button, it already exists!\n");	
	if(false == toPrevScene.empty())
	{
		m_previousSceneName = toPrevScene;
	}

	m_backButton = NULL;//Button::makeIconButton(this, TEXTURES::IMG_COMMON_UI_ICON_BACK);
	//m_backButton->setGameID(GAME_ID_PREVIOUS_SCENE_BTN);
	m_backButton->dockToBottomLeft();
	addChild(m_backButton);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::slidePreviousButton(const ButtonSlideDirection& direction, const float slideDelay)
{
	assert_if(nullptr == m_backButton, "Must makePreviousSceneButton before we can slide it!\n");
	ig_interface::slideInFromDirection(*m_backButton, direction, slideDelay);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::makeSceneBackdrop(const std::string& imageName)
{
	assert_if(nullptr != m_backDrop, "Backdrop has already been created, cannot be created again.\n");
	assert_if("" == imageName, "Invalid value for parameter; imageName("") - expected a string with contents.\n");
	
	/* Add a pretty backdrop. */    
	m_backDrop = NULL;//spriteLoader.spriteWithUniqueName(imageName, ccp(0,0));
	assert_if(nullptr == m_backDrop, "Backdrop was not created properly, does it exist on the sprite sheet?\n");

	/* Set the anchor point to the center of the bottom, and position at the center of the bottom of the screen. */
	m_backDrop->setAnchorPoint(ccp(0.5f, 0.0f));
    m_backDrop->setPosition(ccp(gScreenSize.width/2, 0.0f));
	addChild(m_backDrop, -100);	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::draw(void)
{
	OnDraw();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::tick(cocos2d::ccTime dt)
{
	if(nullptr != dynamic_cast<CAssertScene*>(gSceneMgr->getCurrentScene()))
	{	/* No exiting required during an assert!!! */
		return;
	}
	
	if (true == gSceneMgr->safeToChangeScene())
	{	/* First check if the scene manager needed to change scenes. */
		return;
	}

	ig_interface::Button::checkButtonsForAutoRelease();
	g_effectMgr->update(dt);
	OnUpdate(dt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::onExit(void)
{
//#ifdef DEBUG
	if(nullptr != dynamic_cast<CAssertScene*>(gSceneMgr->getCurrentScene()))
	{	/* No exiting required during an assert!!! */
		return;
	}
//#endif

	/**
	 *	This IF statement is to address an issue where the gameplay scene is required to be the current scene while it
	 *	destructs itself.  OnLeave() SHOULD be called always during onExit, but in the case of the gameplay scene it
	 *	would get called twice if we called it here, since it was called prior to reaching here.
	 *
	 *	This HACKish fix also exists in CSceneManager::safeToChangeScene() where onLeave() is called for gameplay scene.
	 */	
	if("game_play_scene" != GetSceneName())
	{
		OnLeave(nullptr);
	}
	
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	
	//CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile
	//CCTextureCache::sharedTextureCache()->removeTexture();
	
	//purges all textures.		
	CCSpriteFrameCache::purgeSharedSpriteFrameCache();		
	//[CCTextureCache purgeSharedTextureCache];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* ievt)
{
	IG_UNUSED(ievt);
	cocos2d::CCTouch* touch(nullptr);
	int touchId(0);
    
	for (cocos2d::CCSetIterator it(touches->begin()); it != touches->end(); it++, touchId++)
	{
		touch = static_cast<cocos2d::CCTouch*>(*it);
		cocos2d::CCPoint location(touch->LOCATION_IN_VIEW(touch));
		location = cocos2d::CCDirector::sharedDirector()->convertToGL(location); /* What is this doing? */
        
		TouchEvent touchEvt;
		touchEvt.phase = TouchPhase::Began;
		/* touchEvt.touchId = touchId; */
		touchEvt.atScreen.x = location.x;
		touchEvt.atScreen.y = location.y;
		touchEvt.atWorld.x = touchEvt.atScreen.x;// + g_camera->getPositionX();
		touchEvt.atWorld.y = touchEvt.atScreen.y;// + g_camera->getPositionY();
		OnTouch(touchEvt);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::ccTouchesMoved(CCSet* touches, CCEvent* ievt)
{
	IG_UNUSED(ievt);
	cocos2d::CCTouch *touch(nullptr);
	int touchId(0);
    
	for(cocos2d::CCSetIterator it(touches->begin()); it != touches->end(); it++, touchId++)
	{
		touch = static_cast<cocos2d::CCTouch*>(*it);

		cocos2d::CCPoint location(touch->LOCATION_IN_VIEW(touch));
		location = cocos2d::CCDirector::sharedDirector()->convertToGL(location); /* What is this doing? */

		TouchEvent touchEvt;
		touchEvt.phase = TouchPhase::Moved;
		/* touchEvt.touchId = touchId; */
		touchEvt.atScreen.x = location.x;
		touchEvt.atScreen.y = location.y;
		touchEvt.atWorld.x = touchEvt.atScreen.x;// + g_camera->getPositionX();
		touchEvt.atWorld.y = touchEvt.atScreen.y;// + g_camera->getPositionY();
		OnTouch(touchEvt);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBaseScene::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* ievt)
{
	IG_UNUSED(ievt);
	cocos2d::CCTouch* touch(nullptr);
	int touchId(0);
    
	for (cocos2d::CCSetIterator it(touches->begin()); it != touches->end(); it++, touchId++)
	{
		touch = static_cast<cocos2d::CCTouch*>(*it);

		cocos2d::CCPoint location(touch->LOCATION_IN_VIEW(touch));
		location = cocos2d::CCDirector::sharedDirector()->convertToGL(location); /* What is this doing? */

		TouchEvent touchEvt;
		touchEvt.phase = TouchPhase::Ended;
		/* touchEvt.touchId = touchId; */
		touchEvt.atScreen.x = location.x;
		touchEvt.atScreen.y = location.y;
		touchEvt.atWorld.x = touchEvt.atScreen.x;// + g_camera->getPositionX();
		touchEvt.atWorld.y = touchEvt.atScreen.y;// + g_camera->getPositionY();
		OnTouch(touchEvt);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




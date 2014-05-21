#pragma once
#ifndef SUPER_KID_SCENE_BASE_H
#define SUPER_KID_SCENE_BASE_H "$Rev: 2422 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cocos2d.h"
#include "../ui/i_btn_delegate.h"
#include "../effects/node_effect_button_slide_in.h"
#include "CorePlatform/CorePlatformAll.h"

namespace ig_interface { class Button; struct TouchEvent; };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *	Adding a scene to the game. As of [7/10/2012]
 *
 *	Derive your new scene from this base, overriding any of the virtual methods as needed.  In
 *	CSceneManager::SafeToChangeToScene (), add a case for your scene and create it like the others.  CAREFUL to match the
 *	name here, with the GetSceneName () pure virtual function!  Use the gSceneMgr->ChangeToScene ("your_new_scene_name");
 *	whereever fits best!
 */

class CBaseScene : public cocos2d::CCLayer, public ig_interface::IBtnDelegate
{
public:
	CBaseScene(void);
	virtual ~CBaseScene(void);

	/**
	 * Return the name of the scene...
	 *
	 * Currently [7/10/2012] this is not actually used anywhere; it was added to have seemless changes using the scene
	 * manager, which currently has an if...elseif... matching the name the scene should have!
	 */
	virtual std::string GetSceneName(void) const = 0;

	/**
	 * Called once per frame, every frame except possibly the frame when a scene change takes place.  Base
	 * implementation does nothing.
	 */
	virtual void OnUpdate(float dt);

	/**
	 *	Allows the scene to handle any special effects.  Base implementation does nothing.
	 */
	virtual void OnDraw(void) const;

	/**
	 *	Allows the scene to handle touch events on the screen/layer.
	 */
	virtual void OnTouch(const ig_interface::TouchEvent& evt);

	/**
	 * Allows the scene to handle touch events from buttons on the ui.  Base implementation handles the case of the Back
	 * Button, if one exists.
	 */
	virtual void onTouch(ig_interface::Button* const target, const ig_interface::TouchEvent& evt);

	/**
	 * Allows the scene to handle click events from buttons on the ui.  Base implementation handles the case of the Back
	 * Button, if one exists.
	 */
	virtual void onClickAction(ig_interface::Button* const target);

	/**
	 * This is called when the scene is about to be shown and ran for the player.  Base implementation sets
	 * previousScene name, which is to be used if the derived scene has a Back Button.
	 *
	 * prevScene can be NULL if this is the first scene to be entered.
	 */
	virtual void OnEnter(CBaseScene* prevScene);
	
	/**
	 * This is called once the scene has finished running. It will only be called if the scene has been entered, so do
	 * not expect it to be called on a scene that never ran.
	 *
	 * Base implementation calls removesAllChildrenWithCleanup on theCCLayer, which removes any add children- CAREFUL:
	 * any Button's added must be deleted as they are not deleted automatically. DO NOT DELETE them until AFTER calling
	 * the base implementation of OnLeave! Other CCObjects seem to manage themselves, so watch this on a case by case
	 * basis for each object added to addChild () on the CCLayer.
	 *
	 * nextScene can be NULL if the game is shutting down. ??
	 */
	virtual void OnLeave(CBaseScene* nextScene);

	
protected:	
	/**
	 * By default the previous scene name is set duriong OnEnter, however if desiring a specific scene when clicked, you
	 * can specify by providing a non-empty string.
	 */
	void makePreviousSceneButton(const std::string& toPrevScene = "");
	void slidePreviousButton(const ButtonSlideDirection& direction, const float slideDelay = -1.0f);
	void makeSceneBackdrop(const std::string& imageName = "back_drop");

	cocos2d::CCSprite* m_backDrop;	
    ig_interface::Button* m_backButton;
private:
	/**
	 * The following are overridden from the CCObject structure to provide hooks in order to make all of the above work
	 * splendidly.
	 */
	virtual void draw(void);
	virtual void tick(cocos2d::ccTime dt);
	virtual void onExit(void);
	virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* evt);
	virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* evt);
	virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* evt);

	/**
	 * Disabled copy ctor and assignment operator to prevent accidental use. Scenes often have resources to clean.
	 */
	CBaseScene(const CBaseScene& obj);
	CBaseScene& operator = (const CBaseScene& obj);

	std::string m_previousSceneName;
	
	bool m_dbgFirstFrame;
};

#endif /* SUPER_KID_SCENE_BASE_H */

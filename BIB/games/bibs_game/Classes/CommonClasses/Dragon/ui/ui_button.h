#pragma once
#ifndef SUPER_KID_CANNON_UI_BUTTON_H
#define SUPER_KID_CANNON_UI_BUTTON_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../utilities/ig_utilities.h"
#include "../utilities/ig_assert.h"
#include "../utilities/ig_one_time_const.h"
#include "../utilities/ig_defines.h"
#include "../utilities/ig_easing_curves.h"

#include "cocos2d.h"
#include <string>

#include "../effects/effect_manager.h"	/* Dependent on this for the button scale up effect to complete itself. */
#include "../effects/node_effect_button_slide_in.h"

#include "Core/QFunction.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ig_interface
{
	struct TouchEvent;
	class IBtnDelegate;
	class Label;

	typedef enum ButtonStateEnum
	{
		STATE_UP,		/* The button is ready to be pressed or touched. */
		STATE_DOWN,		/* The button is touched, or waiting for it's effect to wear off. */
		STATE_CLICK,	/* The button was just clicked. */
	} ButtonState;

	typedef unsigned long ButtonId;

	class Button : public cocos2d::CCNode, public cocos2d::CCStandardTouchDelegate, public IGameEffectNotifier
	{
	public:
		/**
		 *	Prior to 9/18/12:
		 *	The Button is not automatically released or cleaned up like Label and other ig_interface elements.
		 *	You must call delete on the Button after removing it from the cocos scene.  This would be nice to change.	
		 *
		 *	After 9/18/12:
		 *
		 *	The button has a manager that can and will delete the button after it no longer has a parent.  So if the
		 *	the button is added to a scene, and the scene dies- the next call to checkButtonsForAutoRelease() will
		 *	clean-up the button, deleting the memory and resources.  However, calling delete is still OKAY as long as
		 *	you call delete AFTER the scene is destroyed AND BEFORE checkButtonsForAutoRelease().  Otherwise just set
		 *	the soon-to-be hanging pointer to nullptr.
		 */
		virtual ~Button(void);
    
		static Button* makeScalingButton(IBtnDelegate* handle,
										 const std::string& btnSprite, 
										 const std::string& labelText = "");
		

		/**
		 *	Creates a button from a supplied up and down image and sprite sheet to pull them from.
		 *
		 *	@param handle is the delegate that will be called when this button is clicked.
		 *	@param upSprite is the name of the sprite on the sprite sheet that will be displayed during "up" state.
		 *	@param downSprite is the name of the sprite on the sprite sheet that will be displayed during "down" state,
		 *	if left as "" it will be upSprite + "_down"	
		 *  @param labelText is the text dislayed on button, it can be empty for icon buttons
		 *  @param labelOffset is the offset of label center relative to button center
		 *
 		 *	@note that button cleanup is different than other ui elements.  See the Button desctructor comment.
		 */
		static Button* makeCustomButton(IBtnDelegate* handle, const std::string& upSprite, const std::string& downSprite,
										 const std::string& labelText = "",
										const cocos2d::CCPoint& labelOffset = ccp(0.0f, 0.0f));
		
		static Button* MakeButtonWithTextures(const std::string& upName, 
			const std::string& downName, IBtnDelegate* handle, const std::string &description);

		static Button* MakeButtonWithSpriteFrames(CCSpriteFrame* frmUp, CCSpriteFrame* frmDown, 
			IBtnDelegate* handle, const std::string &description);

		static Button* MakeButtonWithNodes(CCNode* nodeUp, CCNode* nodeDown, IBtnDelegate* handle, const std::string &description);

		/**
		 *	Creates a button from a template to keep all 'large' buttons the same look and everything with ease.
		 *
		 *	@note that button cleanup is different than other ui elements.  See the Button desctructor comment.
		 */
		static Button* makeLargeButton(IBtnDelegate* handle, const std::string& labelText);

		/**
		 *	Creates a button from a template to keep all 'large' buttons the same look and everything with ease.
		 *
		 *	@note that button cleanup is different than other ui elements.  See the Button desctructor comment.
		 */
		static Button* makeExtraWideButton(IBtnDelegate* handle, const std::string& labelText);
		
		/**
		 *	Creates a button from a template to keep all 'normal' buttons the same look and everything with ease.
		 *
		 *	@note that button cleanup is different than other ui elements.  See the Button desctructor comment.
		 */
		static Button* makeNormalButton(IBtnDelegate* handle, const std::string& labelText);
		
		/**
		 *	Creates a large button that looks like it has more attention than other large buttons.  Typically this will
		 *	be used for "Play".  There should only be one per screen to grab the users attention, and is positioned at
		 *	the bottom of the ui by default.
		 */
		static Button* makePlayButton(IBtnDelegate* handle, const std::string& labelText);		
		
		/**
		 *	Creates a button from a template to keep all 'icon' buttons the same.  Icon buttons display an icon instead
		 *	of a label.
		 *
		 *	@param iconImageName is the up state of the icon, the down state will attempt to use iconImageName + "_down"
		 *	as an icon.  Defaults to iconImageName if it failed to create a sprite using the _down variation.
		 *
		 *	@warning iconImageName must exist on the "a_sheet_ui.pshs" sprite sheet.
		 *
		 *	@note that button cleanup is different than other ui elements.  See the Button desctructor comment.
		 */
		static Button* makeIconButton(IBtnDelegate* handle, const std::string& iconImageName, bool green = false);
		
		/*
		 *	Docking Functions:
		 *
		 *	The following functions will place a button to an edge of the screen, accounting for the ui padding that
		 *	DracoGames desires.  Use of these functions, with a small offset if needed, is greatly encouraged over
		 *	setPosition.  Most buttons should be using these docking functions.
		 *
		 *	@note converting/scaling of offsets for the different devices is AUTOMAGICALLY handled in the docking
		 *	functions.  DO NOT scale the offsetX or offsetY variables that are sent it or things will look odd.
		 */

		void dockToBottomLeft(const float offsetX = 0.0f, const float offsetY = 0.0f);
		void dockToBottomRight(const float offsetX = 0.0f, const float offsetY = 0.0f);
		void dockToBottomCenter(const float offsetX = 0.0f, const float offsetY = 0.0f);
		void dockToTopLeft(const float offsetX = 0.0f, const float offsetY = 0.0f);
		void dockToTopRight(const float offsetX = 0.0f, const float offsetY = 0.0f);
		void dockToTopCenter(const float offsetX = 0.0f, const float offsetY = 0.0f);
	
		/**
		 *	Run through all the buttons that have not been deleted, checks if they belong to a scene, and deletes any
		 *	that do not belong somewhere.
		 */
		static void checkButtonsForAutoRelease(void);
	
		static void RegisterWithTouchDispatcherAllButtonsWithParent(bool toRegister, CCNode* parentNode, bool isHandleOnlyVisibleButtons = false);

		void registerWithTouchDispatcher(void);
		void unregisterWithTouchDispatcher(void);
		
		void ChangeSpriteImages(CCSpriteFrame* frmUp, CCSpriteFrame* frmDown);
		void ChangeSpriteNodes(CCNode* nodeUp, CCNode* nodeDown);

		/* IGameEffectNotifier overrides - documented in effect_manager.cpp */
		virtual void OnEffectFinish(IGameEffect* effect);
		
		inline ButtonState getState(void) const { return m_state; }
		inline float getHeight(void) const { return m_height; }
		inline float getWidth(void) const { return m_width; }
		inline ButtonId getId(void) const { return m_buttonId; }
		
		inline bool isEnabled(void) const { return m_enabled; }
		void setEnabled(const bool enabled, bool visuallyDisable = true);
		
		/**
		 *	Make the both up and down sprites of the button change the opacity for fading and other effects.
		 *
		 *	@param opacity how opaque/translucent the button should become. TRANSPARENT (0.0f <= opacity <= 1.0f) OPAQUE.
		 */
		virtual void setOpactity(GLubyte opacity);
		
		void setText(const std::string &text);
		
		/* Game ID getter and setter */
		void setGameID(int gameId) { m_gameId = gameId; }
		int getGameID(void) { return m_gameId; }
		
		std::string getText();
		void SetScale(float scale);

		void SetButtonDownScale(float buttonDownScale);
		void SetEasingType(DracoGames::EasingCurveType easingType, float easingBackOvershoot);
		void SetButtonEffectScaleDuration(float duration);

		CCNode* GetUpSprite() { return m_upSprite; }
		CCNode* GetDownSprite() { return m_downSprite; }
		void SetIsTouchedByBoundingBox(bool value) { _isTouchedByBoundingBox = value; }	
		void SetIsPerformClickImmediately(bool value) { _isPerformClickImmediately = value; } 
		void SetIsPerformScalyOnlyOnY(bool value) { _isPerformScalyOnlyOnY = value; }
		void SetIsTouchEndNotInBoundingBox(bool value) { _isTouchEndNotInBoundingBox = value; }
		template<typename TFunc> void SetFuncOnClicked(const TFunc& func) { _funcOnClicked = func; }

	protected:	
		
		Button();

		void InitWithTextureNames(IBtnDelegate* handle, const std::string &description, 
			const std::string& upName, const std::string& downName);
		void InitWithSpriteFrames(IBtnDelegate* handle, const std::string &description,
			CCSpriteFrame* frmUp, CCSpriteFrame* frmDown);		
		void InitWithNodes(IBtnDelegate* handle, const std::string &description, CCNode* nodeUp, CCNode* nodeDown);

		virtual void onGrabbed(void);
		virtual void onRelease(void);
		virtual void onClicked(void);
		
		/**
		 *	Called whenever a touch is dispatched to the button.  began, moved or ended.
		 */
		virtual void onTouch(const TouchEvent& evt);
		
		/**
		 *	Called only when the button was pressed, and released while the touch was within bounds- this is when the
		 *	button should handle the click action.  It may be delayed by button feedback effects.
		 */
		virtual void onClickAction(void);		
		
		static const float ButtonDownScale;
		void setState(const ButtonState& state);
		
		cocos2d::CCNode* m_upSprite;
		cocos2d::CCNode* m_downSprite;
		ig_interface::Label* m_upLabel;
		ig_interface::Label* m_downLabel;
		
	private:
		/* Prevent accidental use: If you need to create a button, use the static factory methods above "Make" */
		Button(const Button &obj);
		Button& operator=(const Button &obj);		

		inline void registerListener(IBtnDelegate* handle)
		{	/* This function will fail if it is called more than once. */
			m_handler.setValue(handle);
		}

		virtual void ccTouchesBegan(cocos2d::CCSet* pTouches, cocos2d::CCEvent* pEvent);
		virtual void ccTouchesMoved(cocos2d::CCSet* pTouches, cocos2d::CCEvent* pEvent);
		virtual void ccTouchesEnded(cocos2d::CCSet* pTouches, cocos2d::CCEvent* pEvent);

		bool touchWithinBounds(const float touchX, const float touchY);

		void handleTouch(const TouchEvent& evt);
		void Init(IBtnDelegate* handle, const std::string &description);
		void AddScaleEffect(float desiredScaleX, float desiredScaleY, int tag);
		static Button* ms_activeButton;
		static std::vector<Button*> ms_existingButtons;
		static ButtonId ms_lastButtonId;		
		const ButtonId m_buttonId;
		
		DracoGames::igOneTimeConst<IBtnDelegate*> m_handler;
		ButtonState m_state;
		/* these are calculated based on HxW of the upstate */
		float m_width;
		float m_height;
		
		bool m_enabled;
        
        std::string m_buttonDescription;
		
		int m_gameId;

		float _initedScale;
		
		float _buttonDownScale;
		DracoGames::EasingCurveType _easingType;
		float _easingBackOvershoot;
		float _buttonEffectScaleDuration;
		bool _isRegisteredWithTouchDispatcher;
		bool _isTouchedByBoundingBox;	
		bool _isPerformClickImmediately; //вызвать OnClickAction сразу после STATE_CLICK
		bool _isPerformScalyOnlyOnY;
		bool _isTouchEndNotInBoundingBox;  //если TouchEnd не в bounding box кнопки, то все равно вызывать OnClickAction
		QFunction<void()> _funcOnClicked;
		IGameEffect* _currentScaleEffect;
	};
}; /* namespace ig_interface */

#endif /* SUPER_KID_CANNON_UI_BUTTON_H */

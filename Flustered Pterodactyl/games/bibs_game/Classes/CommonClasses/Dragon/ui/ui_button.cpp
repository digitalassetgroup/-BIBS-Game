#include "ui_button.h"
#include "i_btn_delegate.h"
#include "ui_label.h"
#include "ig_interface.h"

//#include "obj_base.h"				/* TouchEvent definition lives here. */
//#include "../core/camera.h"			/* Needed so we can use the camera to set the TouchEvents atWorld. */

//#include "SpriteHelperLoader.h"

//#include "sounds.h"
#include "../utilities/ig_utilities.h"
#include "CorePlatform/CCPointMath.h"

#include "../effects/effect_manager.h"
#include "../effects/node_effect_button_scale.h"

//#include "analytics.h"
#include "../scenes/scene_base.h"
#include "Core/CoreAll.h"
#include "CorePlatform/SpriteHelper.h"
#include "CorePlatform/LogHelper.h"

using namespace cocos2d;
using namespace DracoGames::Constants;

namespace ig_interface
{
	const float Button::ButtonDownScale(0.85f);
    unsigned long Button::ms_lastButtonId(0);
	std::vector<Button*> Button::ms_existingButtons;
	Button* Button::ms_activeButton(nullptr);

	//toRegister = false - unregister
	void Button::RegisterWithTouchDispatcherAllButtonsWithParent(bool toRegister, CCNode* parentNode, bool isHandleOnlyVisibleButtons)
	{
		std::vector<Button*> toUnregister;
		for_each(auto btn, ms_existingButtons)
		{
			bool isVisible = !isHandleOnlyVisibleButtons || btn->isVisible();
			if (btn->getParent() == parentNode && isVisible)
				toUnregister.push_back(btn);
		}

		for_each(Button* btn, toUnregister)
		{
			toRegister 
				? btn->registerWithTouchDispatcher()
				: btn->unregisterWithTouchDispatcher();
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Button::checkButtonsForAutoRelease(void)
	{
		std::vector<Button*> toDelete;
		for_each(Button* const btn, ms_existingButtons)
		{	/* Find the buttons that are hanging around when they shouldn't be. */
			if(nullptr == btn->getParent())
			{
				toDelete.push_back(btn);
			}
		}
		
		while(false == toDelete.empty())
		{	/* Delete the buttons that were hanging around. */
			delete toDelete.back();
			toDelete.pop_back();
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::Init(IBtnDelegate* handle, const std::string &description)
	{
		if(nullptr != handle)
		{
			registerListener(handle);
		}

		registerWithTouchDispatcher();
		
		m_width = (m_upSprite->boundingBox()).size.width;
		m_height = (m_upSprite->boundingBox()).size.height;

		
		auto sizeInPixels = CCSize(m_upSprite->boundingBox().size.width+2, 
										m_upSprite->boundingBox().size.height+2);
		//m_tContentSizeInPixels = sizeInPixels
		this->setContentSize(sizeInPixels);
		
        /* Set button description for logging to analytics */
        if (description.length())
        {
            m_buttonDescription = description;
        }
        else
        {
            //m_buttonDescription = upName;
        }
        
        setText(description);	/* m_width must be set prior to this call. */
		
		ms_existingButtons.push_back(this);
		setState(STATE_UP);
	}

    Button::Button() :
		m_handler(nullptr), 
		m_state(STATE_UP),
		m_buttonId(++ms_lastButtonId),
		m_upSprite(nullptr),
		m_downSprite(nullptr),
		m_upLabel(nullptr),
		m_downLabel(nullptr),
		m_enabled(true),
		_initedScale(1),
		_buttonDownScale(ButtonDownScale), 
		_buttonEffectScaleDuration(0.25f),
		_easingType(DracoGames::EasingCurveNone), 
		_easingBackOvershoot(0),
		_isTouchedByBoundingBox(),
		_isRegisteredWithTouchDispatcher(),
		_isPerformClickImmediately(),
		_isPerformScalyOnlyOnY(),
		_currentScaleEffect(),
		_isTouchEndNotInBoundingBox()
	{
	}

	void Button::InitWithTextureNames(IBtnDelegate* handle, const std::string &description, 
		const std::string& upName, const std::string& downName)
	{
		m_upSprite = SpriteHelper::CreateSpriteWithTextureName(this, 0, upName, ccp(0.5f, 0.5f));
		//m_upSprite = spriteLoader->spriteWithUniqueName(upName, OriginPoint);
		assert_if(nullptr == m_upSprite, "A sprite named %s does not exist on provided sprite sheet.", upName.c_str());
		m_upSprite->setVisible(true);
		//addChild(m_upSprite);
		
		if("" != downName)
		{	
			/* If specified, then use a sprite for the down state. */
			m_downSprite = SpriteHelper::CreateSpriteWithTextureName(this, 0, downName, ccp(0.5f, 0.5f));
			//m_downSprite = spriteLoader->spriteWithUniqueName(downName, OriginPoint);
			assert_if(nullptr == m_downSprite, "A sprite named %s does not exist on provided sprite sheet.", downName.c_str());
			m_downSprite->setVisible(false);
			//addChild(m_downSprite);			
		}

		Init(handle, description);
	}

	void Button::InitWithSpriteFrames(IBtnDelegate* handle, const std::string &description,
		CCSpriteFrame* frmUp, CCSpriteFrame* frmDown)
	{
		ChangeSpriteImages(frmUp, frmDown);

		Init(handle, description);
	}

	void Button::ChangeSpriteImages(CCSpriteFrame* frmUp, CCSpriteFrame* frmDown)
	{
		SAFE_REMOVEFROMPARENT(m_upSprite);
		SAFE_REMOVEFROMPARENT(m_downSprite);

		m_upSprite = SpriteHelper::CreateSpriteWithFrame(this, 0, frmUp, ccp(0.5f, 0.5f));
		m_upSprite->setVisible(true);
		if (frmDown != NULL)
		{
			m_downSprite = SpriteHelper::CreateSpriteWithFrame(this, 0, frmDown, ccp(0.5f, 0.5f));
			m_downSprite->setVisible(false);
		}
	}

	void Button::ChangeSpriteNodes(CCNode* nodeUp, CCNode* nodeDown)
	{
		SAFE_REMOVEFROMPARENT(m_upSprite);
		SAFE_REMOVEFROMPARENT(m_downSprite);

		addChild(nodeUp, 0);
		m_upSprite = nodeUp;
		m_upSprite->setVisible(true);
		if (nodeDown != NULL)
		{
			addChild(nodeDown, 0);
			m_downSprite = nodeDown;
			m_downSprite->setVisible(false);
		}
	}

	void Button::InitWithNodes(IBtnDelegate* handle, const std::string &description, 
		CCNode* nodeUp, CCNode* nodeDown)
	{
		ChangeSpriteNodes(nodeUp, nodeDown);

		Init(handle, description);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Button::~Button(void)
	{
        if (g_effectMgr != NULL)
            g_effectMgr->RemoveEffectsWithParent(this, false);

		//wrong! CCTouchDispatcher retained button
		//CCTouchDispatcher::sharedDispatcher()->removeDelegate(this);

		/** 
		 *	This assert has fired off.
		 *	It seems to happen if you tap the button ever so quickly that it seems not to get an up event?? 
		 */
		warning_if(ms_activeButton == this, "Expected this button not to be active / down currently!\n");		
		if(ms_activeButton == this)
		{
			ms_activeButton = nullptr;
		}
		
		/* Keep this at the bottom of the dtor.  Notice the return in the loop, and the assert if not returned! */
		for(size_t idx(0); idx < ms_existingButtons.size(); ++idx)
		{
			if(this == ms_existingButtons[idx])
			{
				ms_existingButtons[idx] = ms_existingButtons.back();
				ms_existingButtons.pop_back();
				return;
			}
		}
		assert_if(true, "Button was not in the existingButtons!!!!\n");		
	}	
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Button* Button::MakeButtonWithTextures(const std::string& upName, const std::string& downName,
				   IBtnDelegate* handle, const std::string &description)
	{
		auto but = new Button();
		but->InitWithTextureNames(handle, description, upName, downName);
		but->autorelease();
		return but;
	}

	Button* Button::MakeButtonWithSpriteFrames(CCSpriteFrame* frmUp, CCSpriteFrame* frmDown,
				   IBtnDelegate* handle, const std::string &description)
	{
		auto but = new Button();
		but->InitWithSpriteFrames(handle, description, frmUp, frmDown);
		but->autorelease();
		return but;
	}

	Button* Button::MakeButtonWithNodes(CCNode* nodeUp, CCNode* nodeDown, 
		 IBtnDelegate* handle, const std::string &description)
	{
		auto but = new Button();
		but->InitWithNodes(handle, description, nodeUp, nodeDown);
		but->autorelease();
		return but;
	}

	Button* Button::makeScalingButton(IBtnDelegate* handle,
									  const std::string& btnSprite, 
									  const std::string& labelText)
	{
		auto but = new Button();
		but->InitWithTextureNames(handle, labelText, btnSprite, "");
		but->autorelease();
		return but;
	}
	
	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Button* Button::makeCustomButton(IBtnDelegate* handle, const std::string& upSprite, const std::string& downSprite, 
									 const std::string& labelText,
									 const CCPoint& labelOffset)
	{
		IG_UNUSED(downSprite);

		Button* const button(makeScalingButton(handle, upSprite, labelText));
		//Button* const button(new Button(spriteLoader, upSprite, downSprite, handle, labelText));
		
		if (nullptr != button->m_upLabel)
		{
			button->m_upLabel->setPosition(labelOffset + button->m_upLabel->getPosition());
		}
		
		if (nullptr != button->m_downLabel)
		{
			button->m_downLabel->setPosition(labelOffset + button->m_downLabel->getPosition());
		}

		button->autorelease();
		return button;
	}
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Button* Button::makeLargeButton(IBtnDelegate* handle, const std::string& labelText)
	{
		return NULL; //makeScalingButton(handle, TEXTURES::IMG_COMMON_UI_LARGE_BTN_UP, g_uiSpriteLoader, labelText);		
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Button* Button::makeExtraWideButton(IBtnDelegate* handle, const std::string& labelText)
	{
		return NULL; //makeScalingButton(handle, TEXTURES::IMG_EXTRA_WIDE_BUTTON_UP, g_uiSpriteLoader, labelText);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Button* Button::makeNormalButton(IBtnDelegate* handle, const std::string& labelText)
	{
		return NULL; //makeScalingButton(handle, TEXTURES::IMG_COMMON_UI_MEDIUM_BTN_UP, g_uiSpriteLoader, labelText);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Button* Button::makePlayButton(IBtnDelegate* handle, const std::string& labelText)
	{
		return NULL; //makeScalingButton(handle, TEXTURES::IMG_COMMON_UI_PLAY_BUTTON, g_uiSpriteLoader, labelText);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Button* Button::makeIconButton(IBtnDelegate* handle, const std::string& iconSprite, bool green)
	{
		Button* newBtn = NULL;
		
		/*
		if(false == green)
		{
			newBtn = (makeScalingButton(handle, TEXTURES::IMG_COMMON_UI_ICON_BTN_UP));
		}
		else
		{
			newBtn = (makeScalingButton(handle, TEXTURES::IMG_COMMON_UI_ICON_GREEN_BTN_UP));			
		}

		
		const CCPoint centerBtnPos(newBtn->boundingBox().size.width * 0.5f, newBtn->boundingBox().size.height * 0.5f);
		
		CCNode* iconUp(g_uiSpriteLoader->spriteWithUniqueName(iconSprite, centerBtnPos));
		assert_if(nullptr == iconUp, "No button with the name %s was found on a_sheet_ui.", iconSprite.c_str());
		newBtn->m_upSprite->addChild(iconUp);

		if(nullptr != newBtn->m_downSprite)
		{
			CCNode* iconDown(g_uiSpriteLoader->spriteWithUniqueName(iconSprite + "_down", centerBtnPos));
			if (nullptr == iconDown)
			{
				iconDown = (g_uiSpriteLoader->spriteWithUniqueName(iconSprite, centerBtnPos));
			}
			newBtn->m_downSprite->addChild(iconDown);		
		}
		*/
		
		return newBtn;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::dockToBottomLeft(const float offsetX, const float offsetY)
	{
		ig_interface::dockToBottomLeft(*this, offsetX, offsetY);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::dockToBottomRight(const float offsetX, const float offsetY)
	{
		ig_interface::dockToBottomRight(*this, offsetX, offsetY);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::dockToBottomCenter(const float offsetX, const float offsetY)
	{
		ig_interface::dockToBottomCenter(*this, offsetX, offsetY);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::dockToTopLeft(const float offsetX, const float offsetY)
	{
		ig_interface::dockToTopLeft(*this, offsetX, offsetY);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::dockToTopRight(const float offsetX, const float offsetY)
	{
		ig_interface::dockToTopRight(*this, offsetX, offsetY);		
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::dockToTopCenter(const float offsetX, const float offsetY)
	{
		ig_interface::dockToTopCenter(*this, offsetX, offsetY);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static string GetStateString(ButtonState state)
	{
		switch(state)
		{
			case STATE_CLICK: return "STATE_CLICK";
			case STATE_DOWN: return "STATE_DOWN";
			case STATE_UP: return "STATE_UP";
		}
		return "";
	}

	void Button::handleTouch(const TouchEvent& evt)
	{
		if(TouchPhase::Began == evt.phase)
		{
			if(true == touchWithinBounds(evt.atScreen.x, evt.atScreen.y) && nullptr == ms_activeButton)
			{
				if (getState() != STATE_UP)
					return;

				onGrabbed();
				onTouch(evt);
			}
		}
		else if(TouchPhase::Moved == evt.phase)
		{
			if(STATE_DOWN == m_state)
			{
				assert_if(this != ms_activeButton, "Expected this to be the active button if it is down!\n");

				if(_isTouchEndNotInBoundingBox || true == touchWithinBounds(evt.atScreen.x, evt.atScreen.y))
				{
					onTouch(evt);
				}
				else
				{
					onRelease();
				}
			}
		}
		else if(TouchPhase::Ended == evt.phase)
		{			
            if (m_state == STATE_DOWN)
            {
				assert_if(this != ms_activeButton, "Expected this to be the active button if it is down!\n");
                if(_isTouchEndNotInBoundingBox || true == touchWithinBounds(evt.atScreen.x, evt.atScreen.y))
                {
					onClicked();
                    onTouch(evt);
                }
                else
                {
					warning_if(true, "This is a bit unexpected, a touch should have moved off the button?\n");
					
					/**
					 *	This assert has fired off.
					 *	A solution to the issue is to simply follow the code below; but how is this a problem?
					 *	The problem is that the touch can 'move' between "began" and "ended" without receiving a
					 *	"moved" event as would be expected.
					 */

					onRelease();
                }
            }
//			else
//			{
//				assert_if(STATE_UP != m_state, "Invalid state for the button.  Must be STATE_UP or STATE_DOWN.\n");
//				if(this == ms_activeButton)
//				{	/* This really should not be happening, ever. */
//					setState(STATE_UP);
//				}
//			}
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	bool Button::touchWithinBounds(const float touchX, const float touchY)
	{
		if(false == isEnabled() || false == isVisible())
		{
			return false;
		}

		auto pointTouch = this->getParent()->convertToNodeSpace(ccp(touchX, touchY));

		if (_isTouchedByBoundingBox) //костыль
		{
			bool isContains = boundingBox().containsPoint(pointTouch);
			return isContains;
		}
		else
		{
			const float halfx((m_width * _initedScale)  / 2.0f);
			const float halfy((m_height * _initedScale) / 2.0f);  		
			CCRect box = CCRect(getPositionX() - halfx, getPositionY() - halfy, 2*halfx, 2*halfy);
			bool isContains = box.containsPoint(pointTouch);
			return isContains;
		}
	}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::ccTouchesBegan(CCSet* touches, CCEvent* pEvent)
    {
		IG_UNUSED(pEvent);
		cocos2d::CCTouch *touch(nullptr);
		int touchId(0);
		for(cocos2d::CCSetIterator it(touches->begin()); it != touches->end(); it++, touchId++)
		{
			touch = (cocos2d::CCTouch*) (*it);

			cocos2d::CCPoint location(touch->LOCATION_IN_VIEW(touch));
			location = cocos2d::CCDirector::sharedDirector()->convertToGL(location); /* What is this doing? */
            
			TouchEvent evt;
			evt.phase = TouchPhase::Began; /* evt.touchId = touchId; */
			evt.atScreen.x = location.x;
			evt.atScreen.y = location.y;
			evt.atWorld.x = evt.atScreen.x;// + g_camera->getPositionX();
			evt.atWorld.y = evt.atScreen.y;// + g_camera->getPositionY();

			handleTouch(evt);
		}
	}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::ccTouchesMoved(CCSet* touches, CCEvent* pEvent)
	{
		IG_UNUSED(pEvent);
		cocos2d::CCTouch *touch(nullptr);
		int touchId = 0;
		for (cocos2d::CCSetIterator it = touches->begin(); it != touches->end(); it++, touchId++)
		{            
			touch = (cocos2d::CCTouch*) (*it);

			cocos2d::CCPoint location(touch->LOCATION_IN_VIEW(touch));
			location = cocos2d::CCDirector::sharedDirector()->convertToGL(location); /* What is this doing? */
            
			TouchEvent evt;
			evt.phase = TouchPhase::Moved; /* evt.touchId = touchId; */
			evt.atScreen.x = location.x;
			evt.atScreen.y = location.y;
			evt.atWorld.x = evt.atScreen.x;// + g_camera->getPositionX();
			evt.atWorld.y = evt.atScreen.y;// + g_camera->getPositionY();
			
			handleTouch(evt);
		}
	}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::ccTouchesEnded(CCSet* touches, CCEvent* pEvent)
    {
		IG_UNUSED(pEvent);
		cocos2d::CCTouch *touch = nullptr;
		int touchId = 0;
		for (cocos2d::CCSetIterator it(touches->begin()); it != touches->end(); it++, touchId++)
		{
			touch = (cocos2d::CCTouch*) (*it);
            
			cocos2d::CCPoint location(touch->LOCATION_IN_VIEW(touch));
			location = cocos2d::CCDirector::sharedDirector()->convertToGL(location); /* What is this doing? */
            
			TouchEvent evt;
			evt.phase = TouchPhase::Ended; /* evt.touchId = touchId; */
			evt.atScreen.x = location.x;
			evt.atScreen.y = location.y;
			evt.atWorld.x = evt.atScreen.x;// + g_camera->getPositionX();
			evt.atWorld.y = evt.atScreen.y;// + g_camera->getPositionY();
            
			handleTouch(evt);
		}
	}	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Button::onGrabbed(void)
	{
		setState(STATE_DOWN);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::onRelease(void)
	{
		setState(STATE_UP);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::onClicked(void)
	{
		//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SOUNDFX::CLICK);
		setState(STATE_CLICK);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::onTouch(const TouchEvent& evt)
	{
		if (m_handler.hasBeenSet())
		{
			m_handler->onTouch(this, evt);
		}		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::onClickAction(void)
	{
		//Analytics::logGameId(m_gameId);
		if (!_funcOnClicked.IsNull())
			_funcOnClicked();

		if (m_handler.hasBeenSet())
		{
			m_handler->onClickAction(this);
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
//	void Button::playUpAnimation(const std::string& animName, const SpriteHelperLoader* const spriteLoader)
//	{
//		const SpriteHelperLoader* realSpriteLoader = g_uiSpriteLoader;
//		if(nullptr != spriteLoader)
//			realSpriteLoader = spriteLoader;
//		
//		realSpriteLoader->runAnimationWithUniqueName(animName, m_upSprite);
//	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	void Button::playDownAnimation(const std::string& animName, const SpriteHelperLoader* const spriteLoader)
//	{
//		const SpriteHelperLoader* realSpriteLoader = g_uiSpriteLoader;
//		if(nullptr != spriteLoader)
//			realSpriteLoader = spriteLoader;
//		
//		realSpriteLoader->runAnimationWithUniqueName(animName, m_downSprite);		
//	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::registerWithTouchDispatcher(void)
	{
		if (_isRegisteredWithTouchDispatcher)
			return;

		_isRegisteredWithTouchDispatcher = true;

		/* CCTouchDispatcher::sharedDispatcher ()->addTargetedDelegate (this,10,false); */
		//CCTouchDispatcher_SHARED->removeDelegate(this);
		CCTouchDispatcher_SHARED->addStandardDelegate(this, -1001); //NOTE: баттоны всегда тачатся первыми
        if (g_effectMgr != NULL)
            g_effectMgr->ResumeEffectsWithParent(this);		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::unregisterWithTouchDispatcher(void)
	{
		if (!_isRegisteredWithTouchDispatcher)
			return;

		_isRegisteredWithTouchDispatcher = false;
		
		//когда кнопка нажата, то отжимаем ее
		if (m_state != STATE_UP)
		{
			m_state = STATE_UP;
			setScale(_initedScale);
			ms_activeButton = nullptr;
		}

		//NOTE: при removeDelegate(this); происходит release! т.е. Button может освободиться из памяти. 
		//т.о. обращение к полям Buttonа (к _isRegisteredWithTouchDispatcher, например) нужно делать до removeDelegate(this);
		CCTouchDispatcher_SHARED->removeDelegate(this);
        if (g_effectMgr != NULL)
            g_effectMgr->PauseEffectsWithParent(this);		
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static const int TagOnEndTouchEffect = 3;

	void Button::OnEffectFinish(IGameEffect* effect)
	{
		_currentScaleEffect = nullptr;

		if(STATE_CLICK == m_state && effect->GetTag() == TagOnEndTouchEffect)
		{
			setState(STATE_UP);
			if (!_isPerformClickImmediately)
			{
				//assert_if(ms_activeButton != this, "Expected this to be the active button.\n");
				onClickAction(); //here button can be deleted
			}			
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::setEnabled(const bool enabled, bool visuallyDisable)
	{
		m_enabled = enabled;
		
		if(false == visuallyDisable)
		{
			return;
		}
		
		auto upSprite = dynamic_cast<CCSprite*>(m_upSprite);
		if (false == m_enabled)
		{
			if (upSprite != NULL)
				upSprite->setColor(Colors::ButtonSpriteDisabled);
			if(nullptr != m_upLabel)
			{
				m_upLabel->setColor(Colors::ButtonSpriteDisabled);
			}
		}
		else 
		{
			if (upSprite != NULL)
				upSprite->setColor(Colors::White);
			if(nullptr != m_upLabel)
			{
				m_upLabel->setColor(Colors::White);
			}			
		}
	}	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	void Button::AddScaleEffect(float desiredScaleX, float desiredScaleY, int tag)
	{
		if (_currentScaleEffect != nullptr)
		{
			g_effectMgr->RemoveEffect(_currentScaleEffect, true);
			_currentScaleEffect = nullptr;
		}

		auto scaleEffect = (new NodeEffectButtonScale(*this, 
			desiredScaleX, desiredScaleY, _buttonEffectScaleDuration))
			->SetEasingType(_easingType, _easingBackOvershoot);
		scaleEffect->SetTag(tag);

		scaleEffect->setEffectNotifier(this);
		_currentScaleEffect = g_effectMgr->addGameEffect(this, scaleEffect);
	}

	void Button::setState(const ButtonState& state)
	{
		const ButtonState prevState(m_state);
		m_state = state;
		
		if(STATE_DOWN == m_state)
		{
			if(nullptr != m_downSprite)
			{
				m_upSprite->setVisible(false);
				m_downSprite->cocos2d::CCNode::setVisible(true);
			}
			else
			{
				m_upSprite->setVisible(true);
			}
						
			if(prevState != m_state)
			{	/* Scaling Effect Button */
				assert_if(ms_activeButton != nullptr, "Expected no active button at this time.\n");
				ms_activeButton = this;
				setScale(_initedScale);
				const float finalScale = _buttonDownScale * _initedScale;
				AddScaleEffect(_isPerformScalyOnlyOnY ? _initedScale : finalScale, finalScale, 0);
			}
		}
		else if(STATE_UP == m_state)
		{
			m_upSprite->setVisible(true);
			if(nullptr != m_downSprite)
			{
				m_downSprite->setVisible(false);
			}

			if (STATE_UP != prevState)
			{
				if (!_isPerformClickImmediately) 
				{
					assert_if(ms_activeButton != this, "Expected this to be the active button.\n");
				}

				ms_activeButton = nullptr;
			}
			
			if(STATE_DOWN == prevState)
			{	/* Scaling Effect Button */
				const float finalScale = _buttonDownScale * _initedScale;
				setScaleX(_isPerformScalyOnlyOnY ? _initedScale : finalScale);
				setScaleY(finalScale);

				AddScaleEffect(1.0f * _initedScale, 1.0f * _initedScale, 0);
			}
		}
		else if(STATE_CLICK == m_state)
		{
			m_upSprite->setVisible(true);
			if(nullptr != m_downSprite)
			{
				m_downSprite->setVisible(false);
			}
			
			assert_if(STATE_DOWN != prevState, "How is this button being clicked, if the state was not previously down?");
			
			AddScaleEffect(1.0f * _initedScale, 1.0f * _initedScale, TagOnEndTouchEffect);

			if (_isPerformClickImmediately)
			{
				onClickAction();
				ms_activeButton = nullptr;
			}
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::setOpactity(GLubyte opacity)
	{
		auto upSprite = dynamic_cast<CCSprite*>(m_upSprite);
		if (upSprite != NULL)
			upSprite->setOpacity(opacity);
		
		if(nullptr != m_downSprite)
		{
			auto downSprite = dynamic_cast<CCSprite*>(m_downSprite);
			if (downSprite != NULL)
				downSprite->setOpacity(opacity);
		}
	}

	void Button::SetScale(float scale)
	{
		CCNode::setScale(scale);

		_initedScale = scale;
	}

	void Button::SetButtonDownScale(float buttonDownScale)
	{
		_buttonDownScale = buttonDownScale;
	}

	void Button::SetEasingType(DracoGames::EasingCurveType easingType, float easingBackOvershoot)
	{
		_easingType = easingType;
		_easingBackOvershoot = easingBackOvershoot;
	}

	void Button::SetButtonEffectScaleDuration(float duration)
	{
		_buttonEffectScaleDuration = duration;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Button::setText(const std::string &description)
	{
		CCPoint labelPosition(m_width * 0.5f, m_height * 0.5f);
		
		if (nullptr != m_upLabel)
		{
			labelPosition = m_upLabel->getPosition();
			m_upSprite->removeChild(m_upLabel, true);
			m_upLabel = nullptr;
		}
		if (nullptr != m_downLabel)
		{
			labelPosition = m_downLabel->getPosition();
			m_downSprite->removeChild(m_downLabel, true);
			m_downLabel = nullptr;
		}
		
		if(false == description.empty())
		{
			const float labelWidth(0.75f * m_width);
//			const float labelWidth(-1.0f);
			m_upLabel = Label::makeLabelWithText(description, labelPosition, Label::buttonFontSize(), labelWidth);
			m_upLabel->setColor(DracoGames::Constants::Colors::ButtonUpLabelColor);
			m_upSprite->addChild(m_upLabel);
			m_upLabel->release();

			if (nullptr != m_downSprite)
			{
				m_downLabel = Label::makeLabelWithText(description, labelPosition, Label::buttonFontSize(), labelWidth);
				m_downLabel->setColor(DracoGames::Constants::Colors::ButtonDownLabelColor);
				m_downSprite->addChild(m_downLabel);
				m_downLabel->release();
			}
		}
		
		setEnabled(m_enabled); /* Reset button color */
	}

	std::string Button::getText()
	{
		return m_buttonDescription;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
}; /* namespace ig_interface */

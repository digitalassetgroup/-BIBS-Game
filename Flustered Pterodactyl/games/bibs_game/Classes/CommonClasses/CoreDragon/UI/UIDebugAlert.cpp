#include "UIDebugAlert.h"
#include "CCLabelHelper.h"
#include "CorePlatform/InputHelper.h"
#include "CorePlatform/SpriteHelper.h"
#include "CorePlatform/NodeScheduleLambda.h"
#include <cocos2d.h>


static const int _tagAlertYes = 1001;
static const int _tagAlertNo = 1002;

UIDebugAlert::UIDebugAlert()
{
}

void UIDebugAlert::ShowAlert(CCNode* parent, const string& message, CCNode* handler, AlertHandler onAlertDone)
{
	ShowAlert(parent, message, [=](bool value) 
	{
		(handler->*onAlertDone)(value);
	});
}

void UIDebugAlert::ShowAlert(CCNode* parent, const string& message, const QFunction<void(bool)>& onAlertDone)
{
	auto winSize = InputHelper::GetUnscaledWindowSize();
	auto alertNode = new UIDebugAlert();
	alertNode->autorelease();
	parent->addChild(alertNode, 100);

	alertNode->_onAlertDone = onAlertDone;

	CCLabelHelper::CreateLabel(alertNode, message, ccp(winSize.width/2, winSize.height/2 + 50));

	auto itemYes = CCLabelHelper::CreateMenuLabel("YES", _tagAlertYes, ccp(winSize.width/2 - 50, winSize.height/2), 
		alertNode, menu_selector(UIDebugAlert::OnAlertButtonClick));
	auto itemNo = CCLabelHelper::CreateMenuLabel("NO", _tagAlertNo, ccp(winSize.width/2 + 50, winSize.height/2),
		alertNode, menu_selector(UIDebugAlert::OnAlertButtonClick));

	auto menu = CCMenu::create(itemYes, itemNo, NULL);
	menu->setPosition(0, 0);
	alertNode->addChild(menu, 100);	
}

void UIDebugAlert::OnAlertButtonClick(CCObject* sender)
{
	auto item = dynamic_cast<CCNode*>(sender);
	if (item->getTag() == _tagAlertYes)
	{
		_onAlertDone(true);
	}
	else if (item->getTag() == _tagAlertNo)
	{
		_onAlertDone(false);
	}

	NodeScheduleLambda(this, 0.01f, [=]
	{
		this->removeFromParentAndCleanup(true);
	});
}


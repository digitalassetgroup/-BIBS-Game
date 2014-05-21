#include "CCLabelHelper.h"
#include <cocos2d.h>


CCLabelBMFont* CCLabelHelper::CreateLabel(CCNode* parent, const string& caption, const CCPoint& position)
{
	auto label = CCLabelBMFont::create(caption.c_str(), "fonts/font.fnt", 0, kCCTextAlignmentCenter);
	label->setScale(0.8f);
	label->setPosition(position);
	parent->addChild(label, 5);
	return label;
}

CCMenuItemLabel* CCLabelHelper::CreateMenuLabel(const string& caption, int tag, const CCPoint& position, CCNode* _this, SEL_MenuHandler handler)
{
	auto label = CCLabelBMFont::create(caption.c_str(), "fonts/font.fnt", 0, kCCTextAlignmentCenter);
	auto menuItem = CCMenuItemLabel::create(label, _this, handler);
	menuItem->setPosition(position);
	menuItem->setTag(tag);
	return menuItem;
}

CCLabelBMFont* CCLabelHelper::CreateLabelRegulator(CCNode* parent, CCMenu* menu, const CCPoint& position, float xFromLabel, SEL_MenuHandler handler)
{
	auto menuNegative = CCLabelHelper::CreateMenuLabel("[-]", -1, position + ccp(xFromLabel, 0), parent, handler);
	auto menuAdd = CCLabelHelper::CreateMenuLabel("[+]", 1, position + ccp(xFromLabel + 40, 0), parent, handler);
	menu->addChild(menuNegative);
	menu->addChild(menuAdd);

	auto label = CCLabelHelper::CreateLabel(parent, "", position);
	return label;
}


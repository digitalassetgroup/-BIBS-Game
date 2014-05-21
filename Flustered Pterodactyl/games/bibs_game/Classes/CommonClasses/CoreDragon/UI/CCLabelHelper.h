#pragma once
#ifndef CCLabelHelper_h
#define CCLabelHelper_h

#include <string>
using std::string;
#include "base_nodes/CCNode.h"

namespace cocos2d
{ 
	class CCNode; class CCPoint;
	class CCMenu; class CCLabelBMFont; class CCMenuItemLabel;
}
using namespace cocos2d;

namespace CCLabelHelper
{
	CCLabelBMFont* CreateLabel(CCNode* parent, const string& caption, const CCPoint& position);
	CCMenuItemLabel* CreateMenuLabel(const string& caption, int tag, const CCPoint& position, 
		CCNode* _this, SEL_MenuHandler handler);
	CCLabelBMFont* CreateLabelRegulator(CCNode* parent, CCMenu* menu, const CCPoint& position, float xFromLabel, SEL_MenuHandler handler);
}


#endif
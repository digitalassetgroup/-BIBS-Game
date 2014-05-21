#pragma once
#ifndef UIDebugAlert_h
#define UIDebugAlert_h

#include "base_nodes/CCNode.h"
#include "Core/QFunction.h"
#include <string>
using std::string;
using namespace cocos2d;


class UIDebugAlert : public CCNode
{
private:
	QFunction<void(bool)> _onAlertDone;

public:
	typedef void (CCObject::*AlertHandler)(bool);

	static void ShowAlert(CCNode* parent, const string& message, CCNode* handler, AlertHandler onAlertDone);
	static void ShowAlert(CCNode* parent, const string& message, const QFunction<void(bool)>& onAlertDone);

private:
	UIDebugAlert();
	void OnAlertButtonClick(CCObject* sender);
};

#define uialert_selector(sel) (UIDebugAlert::AlertHandler)(&sel)

#endif
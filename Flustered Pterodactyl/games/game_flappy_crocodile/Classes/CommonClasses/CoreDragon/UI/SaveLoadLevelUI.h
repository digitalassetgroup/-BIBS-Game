#pragma once
#ifndef SaveLoadLevelUI_h
#define SaveLoadLevelUI_h

#include "text_input_node/CCTextFieldTTF.h"
#include "Core/QFunction.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

namespace cocos2d
{
	class CCMenu;
}
using namespace cocos2d;


class SaveLoadLevelUI : public CCNode, public CCTextFieldDelegate
{
private:
	enum { _tagMenuExitFromLevels = -1, _tagMenuPrevLevels = -2, _tagMenuNextLevels = -3 };
	CCNode* _parent;
	CCMenu* _menuLevels;
	vector<string> _allLevels;
	int _currentPage;	
	CCTextFieldTTF* _inputField;
	string _currentLevelName;
	const int _maxLevelCountOnPage;
	string _fileExtension;
	bool _isMenuVisible;

public:
	QFunction<bool(const string&)> OnSaveToFile;
	QFunction<bool(const string&)> OnLoadFromFile;

public:
	SaveLoadLevelUI(CCNode* parent);
	void SetFileExtension(const string& fileExtension);
	bool IsMenuVisible() const;
	void Init();
	void ButLoadLevelClick(CCObject* sender);
	void ButSaveLevelClick(CCObject* sender);

private:
	void BuildMenuLevels();
	string GetMenuLevelSelectedItem(CCMenu* menuLevels, CCObject* sender);
	void ButChooseLevelClicked(CCObject* sender);
	void ButShowNextLevelsClicked(CCObject* sender);
	bool onTextFieldDetachWithIME(CCTextFieldTTF* sender);
};


#endif
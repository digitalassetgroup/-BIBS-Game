#include "SaveLoadLevelUI.h"
#include "CCLabelHelper.h"
#include "Core/StringHelper.h"
#include "lib/Boolinq/boolinq-all.h"
#include "CorePlatform/FileHelper.h"
#include "CorePlatform/InputHelper.h"
#include "CorePlatform/NodeScheduleLambda.h"
#include "CorePlatform/LogHelper.h"


SaveLoadLevelUI::SaveLoadLevelUI(CCNode* parent) : _parent(parent), 
	_menuLevels(),
	_currentPage(),
	_maxLevelCountOnPage(10),
	_inputField(),
	_isMenuVisible()
{
	SetFileExtension(".lvl");
}

void SaveLoadLevelUI::SetFileExtension(const string& fileExtension)
{
	_fileExtension = fileExtension;
}

bool SaveLoadLevelUI::IsMenuVisible() const
{
	return _isMenuVisible;
}

void SaveLoadLevelUI::Init()
{
	auto winSize = InputHelper::GetUnscaledWindowSize();

	_menuLevels = CCMenu::create();
	_menuLevels->setPosition(winSize/2);
	_menuLevels->setVisible(false);
	_parent->addChild(_menuLevels, 100);

	_inputField = CCTextFieldTTF::textFieldWithPlaceHolder("<input file name>", "Thonburi", 46);
	_inputField->setPosition(ccp(winSize.width/2, winSize.height/2));
	_inputField->setVisible(false);
	_inputField->setDelegate(this);
	_parent->addChild(_inputField, 60);
}

void SaveLoadLevelUI::ButLoadLevelClick(CCObject* sender)
{
	_allLevels.clear();
	_currentPage = 0;
	FileHelper::GetAllLevelsFromUserPath(_allLevels, "", _fileExtension);
	//if (levels.empty())
	//	return;

	_isMenuVisible = true;
	ButShowNextLevelsClicked(nullptr);
}

void SaveLoadLevelUI::ButSaveLevelClick(CCObject* sender)
{
	_isMenuVisible = true;

	_inputField->setVisible(true);
	_inputField->attachWithIME();

	_inputField->setString(_currentLevelName.c_str());
}

void SaveLoadLevelUI::BuildMenuLevels()
{
	_menuLevels->removeAllChildrenWithCleanup(true);
	auto levels = boolinq::from(_allLevels)
		.skip(_currentPage * _maxLevelCountOnPage)
		.take(_maxLevelCountOnPage);

	for (const auto& levelString : levels.toVector())
	{
		auto item = CCLabelHelper::CreateMenuLabel(levelString, 0, ccp(0, 0), this, 
			menu_selector(SaveLoadLevelUI::ButChooseLevelClicked));
		_menuLevels->addChild(item);
	}

	if (_currentPage > 0)
	{
		auto item = CCLabelHelper::CreateMenuLabel("-----PREV-----", _tagMenuPrevLevels, ccp(0, 0), 
			this, menu_selector(SaveLoadLevelUI::ButShowNextLevelsClicked));
		_menuLevels->addChild(item);
	}

	if (_currentPage < ((int)_allLevels.size() / _maxLevelCountOnPage))
	{
		auto item = CCLabelHelper::CreateMenuLabel("-----NEXT-----", _tagMenuNextLevels, ccp(0, 0), 
			this, menu_selector(SaveLoadLevelUI::ButShowNextLevelsClicked));
		_menuLevels->addChild(item);
	}

	auto item = CCLabelHelper::CreateMenuLabel("-----EXIT-----", _tagMenuExitFromLevels, ccp(0, 0), 
		this, menu_selector(SaveLoadLevelUI::ButChooseLevelClicked));
	_menuLevels->addChild(item);

	_menuLevels->alignItemsVertically();
	_menuLevels->setVisible(true);
}

string SaveLoadLevelUI::GetMenuLevelSelectedItem(CCMenu* menuLevels, CCObject* sender)
{
	menuLevels->setVisible(false);
	auto menuItem = dynamic_cast<CCMenuItemLabel*>(sender);
	if (menuItem->getTag() == _tagMenuExitFromLevels)
		return "";

	auto label = (CCLabelBMFont*)menuItem->getChildren()->objectAtIndex(0);
	return label->getString();
}

void SaveLoadLevelUI::ButChooseLevelClicked(CCObject* sender)
{
	_isMenuVisible = false;
	auto selectedLevel = GetMenuLevelSelectedItem(_menuLevels, sender);
	if (selectedLevel.empty())
		return;

	bool isLoaded = OnLoadFromFile(selectedLevel);
	if (!isLoaded)
	{
		MY_LOG("Error loading file " << selectedLevel);
		return;
	}

	MY_LOG("Success loading file " << selectedLevel);
	_currentLevelName = selectedLevel;
	StringHelper::Replace(_currentLevelName, _fileExtension, "");
}

void SaveLoadLevelUI::ButShowNextLevelsClicked(CCObject* sender)
{
	int dPage = 0;
	if (sender != nullptr)
	{
		auto item = dynamic_cast<CCNode*>(sender);
		if (item->getTag() == _tagMenuPrevLevels)
			dPage = -1;
		else if (item->getTag() == _tagMenuNextLevels)
			dPage = 1;
	}
	_currentPage += dPage;
	NodeScheduleLambda(_parent, 0.01f, [=] { BuildMenuLevels(); });
}

bool SaveLoadLevelUI::onTextFieldDetachWithIME(CCTextFieldTTF* sender)
{
	_isMenuVisible = false;
	_inputField->setVisible(false);
	_currentLevelName = sender->getString();

	bool isSaved = OnSaveToFile(_currentLevelName + _fileExtension);
	if (isSaved) {
		MY_LOG("Success saving file " << _currentLevelName);
	} else {
		MY_LOG("Error saving file " << _currentLevelName);
	}
	return false;
}


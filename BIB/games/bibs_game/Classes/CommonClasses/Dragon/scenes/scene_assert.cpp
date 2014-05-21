#include "scene_assert.h"

#include "scene_manager.h"
//#include "scene_game_play.h"

//#include "../core/data/game_data.h"
#include "../utilities/ig_utilities.h"

//#include "AutoGenConst.h"
#include "../ui/ui_label.h"

using namespace DracoGames;
using namespace DracoGames::Utils;
using namespace ig_interface;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAssertScene::CAssertScene(const std::string& assertMessage, const std::string &fileName, const unsigned int line) :
	m_assertMessage(assertMessage),
	m_fileName(fileName.substr(fileName.find_last_of("/"))),
	m_lineNumber(line)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAssertScene::~CAssertScene(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAssertScene::OnEnter(CBaseScene* prevScene)
{	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	const float centerGuideX(gScreenSize.width/2);
	const float assertGuideY(gScreenSize.height/2 + (40.0f * g_convertRatio->y));
	const float spacingY(50.0f * g_convertRatio->y);
	
	std::string infoString("Build: ");
	//infoString += igAutoGen::REV_NUM;
	infoString += "\n" + m_fileName + " : " + ToString(m_lineNumber);
	
	CBaseScene* const currentScene = NULL;//gSceneMgr->getCurrentScene();
	if(nullptr != currentScene)
	{
		infoString += "\nScene: " + currentScene->GetSceneName();
		/*
		if(nullptr != dynamic_cast<CGameplayScene*>(currentScene))
		{
			infoString += "\nWorld-Level: " + ToString(gGameData->getCurrentWorld()) + " - " + ToString(gGameData->getCurrentLevel());
		}
		*/
	}
	else
	{
		infoString += "\nNo current scene.";
	}
	
	ig_interface::Label* const generalInfo = ig_interface::Label::makeLabelWithText(infoString, ccp(0,0), Label::descriptionFontSize());
	generalInfo->setPosition(ccp((10.0f * g_convertRatio->x) + (generalInfo->boundingBox().size.width / 2.0f), 
								 gScreenSize.height - (10.0f * g_convertRatio->y) - (generalInfo->boundingBox().size.height / 2.0f)));
	generalInfo->CCNode::setScale(0.75f);	/* Take that private label scaling... */
	addChild(generalInfo);
	
	ig_interface::Label* const assertFailed = ig_interface::Label::makeLabelWithText("Assertion Failed!", ccp(0,0), Label::descriptionFontSize());
	assertFailed->setPosition(ccp(centerGuideX, assertGuideY));
	assertFailed->setColor(cocos2d::ccc3(255, 0, 0));
	addChild(assertFailed);
	
	m_assertMessage = fixStringToLength(m_assertMessage, 32);
	addChild(ig_interface::Label::makeLabelWithText(m_assertMessage, ccp(centerGuideX, assertGuideY - spacingY), Label::descriptionFontSize()));

	CBaseScene::OnEnter(prevScene);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAssertScene::OnLeave(CBaseScene* nextScene)
{
	CBaseScene::OnLeave(nextScene);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string CAssertScene::fixStringToLength(const std::string& msg, const unsigned int length)
{		
	if(msg.length() <= length)
	{
		return msg;
	}
	
	std::string result;
	size_t idx(0);
	while(msg.length() - idx > length)
	{
		std::string lineOfText = msg.substr(idx, length);
		size_t spaceIdx = lineOfText.find_last_of(" ");
		if(std::string::npos != spaceIdx)
		{
			result += lineOfText.substr(0, spaceIdx);
			result += '\n';
			idx += spaceIdx + 1;
		}
		else
		{
			result += lineOfText;
			result += '\n';
			idx += length;
		}
	}
	
	result += msg.substr(idx);
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

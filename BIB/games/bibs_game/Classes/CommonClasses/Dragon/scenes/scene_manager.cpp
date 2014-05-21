#include "scene_manager.h"
#include "scene_base.h"

#include "scene_assert.h"

using namespace cocos2d;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSceneManager* gSceneMgr(nullptr);

const float CSceneManager::SlowTransitionTime(0.75f);
const float CSceneManager::FastTransitionTime(0.35f);

CSceneManager::CSceneManager(void) :
	m_director(CCDirector::sharedDirector()),
	m_doSceneChange(false),
	m_desiredScene()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSceneManager::~CSceneManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSceneManager::ChangeToScene(CCScene* newScene)
{
	//assert_if(true == m_doSceneChange, "Cannot change scene \"%s\", already changing to scene \"%s\".",
	//		  sceneName.c_str(), m_desiredScene.c_str());
	
	m_doSceneChange = true;
	m_fadingTransition = -1.0f;
	m_desiredScene = newScene;
	
	if(nullptr == m_director->getRunningScene())
	{	/**
		 *	If this is the first scene the manager is changing to, it is safe. However for any other scene, the only safe
		 *  time to change is before OnUpdate() is called. (Within CBaseScene::tick())
		 */
		safeToChangeScene();
	}
}

void CSceneManager::FadeTransitionToScene(CCScene* newScene, const float fadeTimer)
{
	m_doSceneChange = true;
	m_fadingTransition = fadeTimer;
	m_desiredScene = newScene;
}

void CSceneManager::gotoAssertScene(const char* const fileName, const unsigned int line, const char* const assertMessage, ...)
{
	char buffer[4096];
	va_list variableList;
	va_start(variableList, assertMessage);
	vsprintf(buffer, assertMessage, variableList);
	va_end(variableList);
	
	CAssertScene* newScene = new CAssertScene(buffer, fileName, line);
	newScene->OnEnter(nullptr);
	
	CCScene *ccscene(CCScene::create());
	ccscene->addChild(newScene);
	newScene->release(); /* TB: What is this doing? It was within the initial Cocos helloworld?! Obj-C madness? */
	
	CCDirector* const director = CCDirector::sharedDirector();
	//director->pushScene(ccscene);
	

	
	director->replaceScene(ccscene);
	
	//gSceneMgr->m_currentscene = newScene;
	
	while(true)
	{
		director->update(0.0f);
		director->mainLoop();
	}	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CSceneManager::safeToChangeScene(void)
{
	if(false == m_doSceneChange)
	{
		return false;
	}
		
	m_doSceneChange = false;
	
	/* We are assuming that replaceScene deletes/cleans-up after the previous scene... */
	//CBaseScene *newScene(/*m_desiredScene*/NULL);
	CCScene* newScene = m_desiredScene;
	m_desiredScene = NULL;

	CCScene* oldccscene = m_director->getRunningScene();
	//CBaseScene* oldScene((nullptr == oldccscene) ? nullptr : static_cast<CBaseScene*>(oldccscene->getChildren()->objectAtIndex(0)));
    CCScene* oldScene = oldccscene;

	g_effectMgr->clearAllEffects(false);
	
	if (nullptr != oldScene)
	{
//		oldScene->OnLeave(newScene);	/* Scenes now clean up during the onExit called by Cocos magic. -TIM < 8/1/12 */
		
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
		CCTextureCache::sharedTextureCache()->removeUnusedTextures();
		
		//CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile
		//CCTextureCache::sharedTextureCache()->removeTexture();
				
		//purges all textures.		
		CCSpriteFrameCache::purgeSharedSpriteFrameCache();		
		//[CCTextureCache purgeSharedTextureCache];
	}

	/* Now we jump into the new scene. */
	m_currentscene = newScene;
	
	if (nullptr != newScene)
	{
		//newScene->OnEnter(oldScene);

		//CCScene *ccscene(CCScene::node());
		//ccscene->addChild(newScene);
		CCScene* ccscene = newScene;

		//newScene->release(); /* TB: What is this doing? It was within the initial Cocos helloworld?! Obj-C madness? */

		if(nullptr == oldScene)
		{
			m_director->runWithScene(ccscene);
		}
		else if (m_fadingTransition > 0.0f)
		{
			m_director->replaceScene(CCTransitionFade::create(m_fadingTransition, ccscene));
		}
		else
		{
			m_director->replaceScene(ccscene);
		}
	}
	else
	{
		assert_if(true, "Expected shutdown case."); /* Shut down time. */	
	}

	return true;
}


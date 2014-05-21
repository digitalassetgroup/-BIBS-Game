#pragma once
#ifndef DracoGames_CSCENEMANAGER_H
#define DracoGames_CSCENEMANAGER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <map>

#include <cocos2d.h>
using namespace cocos2d;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ig_interface { struct TouchEvent; }
namespace cocos2d { class CCDirector; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSceneManager
{
public:
    CSceneManager(void);
    ~CSceneManager(void);

    /**
	 *	This sets flags to change the scene the next time SafeToChangeScene is called.  Note: Does not actually change
     *  the scene until SafeToChangeScene is called on the next loop.
	 */
    void ChangeToScene(CCScene* newScene);

	void FadeTransitionToScene(CCScene* newScene, const float fadeTimer = FastTransitionTime);
	
    /* The scene will actually change here if it is marked to do so. Returns true if the scene was changed. */
    bool safeToChangeScene(void);
	
	static const float FastTransitionTime;
	static const float SlowTransitionTime;
	
	inline CCScene* getCurrentScene()
	{
		return m_currentscene;
	}
	
	static void gotoAssertScene(const char* const fileName, const unsigned int line, const char* const assertMessage, ...);
	
protected:

private:
    /* Prevent accidental use */
    CSceneManager(const CSceneManager& obj);
    CSceneManager& operator = (const CSceneManager& obj);
	
    cocos2d::CCDirector* const m_director;
    CCScene* m_desiredScene;
    bool m_doSceneChange;
	float m_fadingTransition;
	
	CCScene* m_currentscene;
};

extern CSceneManager* gSceneMgr; /* Created/Destroyed in AppDelegate.cpp */

#endif /* DracoGames_CSCENEMANAGER_H */

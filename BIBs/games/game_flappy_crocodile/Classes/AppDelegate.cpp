#include "AppDelegate.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Utility/SceneManager.h"
#include "CorePlatform/ScreenViewHelper.h"
#include "CorePlatform/CommonProperties.h"
#include "CorePlatform/InputHelper.h"
#include "SKGameMetadata.h"

using namespace cocos2d;
using namespace CocosDenshion;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
	InitAchievements();

	auto director = CCDirector::sharedDirector();
    director->setOpenGLView(CCEGLView::sharedOpenGLView());

#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
    CCFileUtils::sharedFileUtils()->setSearchPaths(std::vector<std::string>());
	CCFileUtils::sharedFileUtils()->addSearchPath("assets");
#endif

	CommonProperties::SetIsLoadHDImages(true);
	ScreenViewHelper::SetupScreenSizesOnAllPlatforms();
    
    //---if vertical orientation, scale down a little---
    auto winSize = InputHelper::GetUnscaledWindowSize();
    if (winSize.height > winSize.width)
    {
        winSize = winSize * 1.2f;
        InputHelper::SetUnscaledWindowSize(winSize);
        CCEGLView::sharedOpenGLView()->setDesignResolutionSize(winSize.width, winSize.height, kResolutionNoBorder);
    }
    //---
    
    director->setDisplayStats(false);
    director->setAnimationInterval(1.0 / 60);
	
	SceneManager::Inst().RunAtFirst();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->pause();

    //CCDirector::sharedDirector()->stopAnimation();
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->resume();

    //CCDirector::sharedDirector()->startAnimation();
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}

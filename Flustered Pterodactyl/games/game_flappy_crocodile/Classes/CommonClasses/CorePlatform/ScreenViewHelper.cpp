#include "ScreenViewHelper.h"
#include "SceneManagerBase.h"
#include "InputHelper.h"
#include "LogHelper.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && COCOS2D_VERSION >= 0x00020000

//#include "CCEGLViewInternal.h"
//typedef CCEGLViewInternal CCEGLViewBase;
typedef CCEGLView CCEGLViewBase;

namespace ScreenViewHelper
{
	class CustomGLView : public CCEGLViewBase
	{
		protected_ virtual void OnActive(bool isActive)
		{
			//MY_LOG("On Active " << isActive);
			OnChangeFocus(isActive);
		}

		protected_ virtual void OnSetFocus(bool isFocus)
		{
		}

		private_ void OnChangeFocus(bool isFocus)
		{
			#ifdef DEBUG_GAME
				return; //for testing
			#endif

			auto sceneManager = SceneManagerBase::GetInstance();
			if (sceneManager == NULL)
				return;

			if (isFocus)
				sceneManager->OnFocusedOn();
			else
				sceneManager->OnFocusedOff();
		}
	};

	void CreateWindowsView(const char* viewName)
	{
		CCEGLViewBase::SetSharedOpenGLView(new CustomGLView());
		auto eglView = CCEGLViewBase::sharedOpenGLView();
		eglView->setViewName(viewName);
		eglView->Create();  //cocos2.0.4 addition
		eglView->SetWindowBackgroundColor(0, 0, 0);
		eglView->SetIsFullScreen(false);
	}

	void ToggleFullScreen()
	{
		auto desktopSize = InputHelper::GetDesktopWindowSize();
		auto screenSize = InputHelper::GetUnscaledWindowSize();

		auto eglView = CCEGLViewBase::sharedOpenGLView();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		eglView->swapBuffers(); //black color

		eglView->SetIsFullScreen(!eglView->IsFullScreen());
		eglView->UpdateWindowStyle(desktopSize.width, desktopSize.height);
		eglView->setFrameSize(desktopSize.width, desktopSize.height);
		eglView->setDesignResolutionSize(screenSize.width, screenSize.height, kResolutionShowAll);
	}
}

#endif

namespace ScreenViewHelper
{
	static void SetupScreenSizeCommon(CCSize screenSize)
	{		
		InputHelper::SetDesktopWindowSize(IntSize(screenSize.width, screenSize.height));
   
		if (screenSize.width < 700)
		{
			screenSize.width *= 2;
			screenSize.height *= 2;

			//sd
			InputHelper::SetNodeScale(768.0f/320.0f);
			InputHelper::SetIsLoadSD(true);
		}

		auto referenceSize = InputHelper::GetReferenceScreenSize();
	
		//--скейлим весь экран--
		screenSize.width = referenceSize.height * (screenSize.width * 1.0f/screenSize.height);
		screenSize.height = referenceSize.height;
		//-------

		CCEGLView* eglView = CCEGLView::sharedOpenGLView();
		InputHelper::SetUnscaledWindowSize(screenSize);
		eglView->setDesignResolutionSize(screenSize.width, screenSize.height, kResolutionShowAll);	
	}

	static void SetupScreenSizeOnIos()
	{
	#if COCOS2D_VERSION < 0x00020000
        bool isIpad = pDirector->getWinSizeInPixels().height == 1024 || pDirector->getWinSizeInPixels().width == 1024;    
        if (!isIpad)
            pDirector->enableRetinaDisplay(true);
    #else
        CCSize screenSize = CCEGLView::sharedOpenGLView()->getFrameSize();
        //if (screenSize.height > 320) //мы всегда используем ретина-картинки
        {
            CCSize designSize;
            designSize = CCSizeMake(480, 320); //design size делаем с учетом ретины
            CCSize resourceSize;
            resourceSize = CCSizeMake(960, 640);
            //CCFileUtils::sharedFileUtils()->setResourceDirectory("hd");
            //pDirector->setContentScaleFactor(resourceSize.height/designSize.height);
        }
    
        if (screenSize.height <= 480) //if iPhone-sd
        {
            screenSize.width *= 2;
            screenSize.height *= 2;
            
            //sd
			InputHelper::SetNodeScale(768.0f/320.0f);
			InputHelper::SetIsLoadSD(true);
        }
        else if (screenSize.height >= 1536) //if iPad-retina
        {
            screenSize.width /= 2;
            screenSize.height /= 2;
        }

		InputHelper::SetDesktopWindowSize(IntSize(screenSize.width, screenSize.height));
    
        auto referenceSize = InputHelper::GetReferenceScreenSize();
    
        //--скейлим весь экран--
        screenSize.width = referenceSize.height * (screenSize.width * 1.0f/screenSize.height);
        screenSize.height = referenceSize.height;
        //-------

		InputHelper::SetUnscaledWindowSize(screenSize);
        CCEGLView::sharedOpenGLView()->setDesignResolutionSize(screenSize.width, screenSize.height, kResolutionNoBorder);
        
    #endif
	}

	CCSize GetWindowSizeByDesktopSize(const IntSize& desktopSize)
	{
		//NOTE: при пропорции 1.25 (1280x1024), сцена паузы не влезает по ширине
		//поэтому выставляем минимальную пропорцию 1.33 (как при 1024x768)
		float initialProportion = Math::Max(1.333f, desktopSize.width * 1.0f/desktopSize.height);
		CCSize screenSize;
		screenSize.height = Math::Min(desktopSize.height * 0.8f, 1024.0f);
		screenSize.width = screenSize.height * initialProportion;
		return screenSize;
	}

	void SetupScreenSizesOnAllPlatforms()
	{
		#if ((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && COCOS2D_VERSION >= 0x00020000) || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
			ScreenViewHelper::SetupScreenSizeCommon(InputHelper::GetUnscaledWindowSize());
		#endif

		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && COCOS2D_VERSION >= 0x00020000) || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN
			CCSize screenSize = CCEGLView::sharedOpenGLView()->getFrameSize();
			InputHelper::SetUnscaledWindowSize(screenSize);

			MY_LOG_WARNING_F("screen size = %dx%d", screenSize.width, screenSize.height);

			ScreenViewHelper::SetupScreenSizeCommon(InputHelper::GetUnscaledWindowSize());
		#endif

		#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			ScreenViewHelper::SetupScreenSizeOnIos();

		//  pDirector->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);
		#endif
	}
}
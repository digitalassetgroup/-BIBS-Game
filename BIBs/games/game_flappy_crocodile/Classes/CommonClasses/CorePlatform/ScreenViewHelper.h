#pragma once
#ifndef ScreenViewHelper_h
#define ScreenViewHelper_h

#include "Core/CoreAll.h"
#include <cocos2d.h>
using namespace cocos2d;


namespace ScreenViewHelper
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && COCOS2D_VERSION >= 0x00020000

	void CreateWindowsView(const char* viewName);
	void ToggleFullScreen();

#endif

	CCSize GetWindowSizeByDesktopSize(const IntSize& desktopSize);
	void SetupScreenSizesOnAllPlatforms();
}

#endif

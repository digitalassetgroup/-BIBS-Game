//
//  MacScreenViewHelper.mm
//  CatsCloneMac
//
//  Created by Slava Arhipov on 19.03.13.
//  Copyright (c) 2013 Bullets in a Burning Box, Inc. All rights reserved.
//

#include "MacScreenViewHelper.h"
#include <cocos2d.h>
#import "EAGLView.h"
#include "CorePlatform/InputHelper.h"
using namespace cocos2d;


namespace MacScreenViewHelper
{
    void ToggleFullscreen()
    {
        EAGLView* pView = [EAGLView sharedEGLView];
        
        bool isFullscreen = !pView.isFullScreen;
        
        CCEGLView* eglView = CCEGLView::sharedOpenGLView();
        if (isFullscreen)
        {
            NSSize screenSize = [NSScreen mainScreen].frame.size;
            eglView->setFrameSize(screenSize.width, screenSize.height);
        }
        else
        {
            auto desktopSize = InputHelper::GetDesktopWindowSize();
            eglView->setFrameSize(desktopSize.width, desktopSize.height);
        }
        
        //NOTE: при включении фулскрина на 1 кадр показывается растянутое изображения. далее идет фикс этого бага
        pView->NotToDrawWhileReshape = true;
   		[pView setFullScreen:isFullscreen];
        pView->NotToDrawWhileReshape = false;
        //update projection
        
        auto winSize = InputHelper::GetUnscaledWindowSize();
        eglView->setDesignResolutionSize(winSize.width, winSize.height, kResolutionShowAll);
    }
}

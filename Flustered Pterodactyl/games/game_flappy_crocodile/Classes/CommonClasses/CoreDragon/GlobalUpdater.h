#pragma once
#ifndef GlobalUpdater_h
#define GlobalUpdater_h

#include "CorePlatform/CorePlatformAll.h"
#include <cocoa/CCObject.h>
using namespace cocos2d;

class GlobalUpdater : NonCopyable, public CCObject
{
	public_ static GlobalUpdater& Inst();

	private_ virtual void update(ccTime deltaTime);
};

#endif

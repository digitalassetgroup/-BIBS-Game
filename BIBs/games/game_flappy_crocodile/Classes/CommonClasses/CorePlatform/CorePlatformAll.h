#pragma once
#ifndef CorePlatformAll_H
#define CorePlatformAll_H

#include "Core/CoreAll.h"
#include "platform/CCPlatformMacros.h"
#include <CCDirector.h>
#include <CCEGLView.h>
#include <CCScheduler.h>


#define COCOS2D_VERSION 0x00020100   //TODO: temporarily - need to set up this paramter in compiler options

#define SAFE_REMOVEFROMPARENT(s) \
	if (s != NULL) \
	{ \
		s->removeFromParentAndCleanup(true); \
		s = NULL; \
	}

#if COCOS2D_VERSION >= 0x00020000
	namespace cocos2d
	{
		typedef float ccTime;
	}

	#define ARCH_OPTIMAL_PARTICLE_SYSTEM CCParticleSystemQuad
	#define kCCMenuTouchPriority -128  //* priority used by the menu

	#define CCApplication_SHARED (*CCApplication::sharedApplication())
	#define CCEGLView_SHARED (*CCEGLView::sharedOpenGLView())
	#define CCScheduler_SHARED CCDirector::sharedDirector()->getScheduler()
	#define CCTouchDispatcher_SHARED CCDirector::sharedDirector()->getTouchDispatcher()	

	#define CCMenuItemSprite_FROMSPRITE CCMenuItemSprite::itemWithNormalSprite
	#define INIT_FROM_NORMAL_SPRITE initWithNormalSprite
	#define RENDERTEXTURE_END end()
	#define SET_DISPLAY_FPS setDisplayStats

	#define PREVIOUS_LOCATION_IN_VIEW(touch) getPreviousLocationInView()
	#define LOCATION_IN_VIEW(touch) getLocationInView()

	#define SET_IS_ACCELEROMETER_ENABLED setAccelerometerEnabled
	#define SET_IS_AUTOREMOVE_ONFINISH setAutoRemoveOnFinish
	#define SET_IS_POPUP_NOTIFY setPopupNotify
	#define SET_IS_OPACITYMODIFY_RGB setOpacityModifyRGB
	#define IS_SELECTED isSelected
	
	namespace CCFileUtilsShared
	{
		string getWritablePath();
		string fullPathForFilename(const char* pszRelativePath);
		void SET_IS_POPUP_NOTIFY(bool bNotify);
        unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize);
	}
	#define CCFileUtils_SHARED CCFileUtilsShared

	#define CCFileData_IS_USE_DECRYPTION CCFileUtils::IsUseDecryption
	#define CCFileData_GET_DECRYPTFUNC CCFileUtils::GetDecryptFunc

	struct CCFileData
	{
		CCFileData(const char* pszFileName, const char* pszMode, bool needToEncrypt = true);
		~CCFileData() { CC_SAFE_DELETE_ARRAY(m_pBuffer); }
		CC_SYNTHESIZE_READONLY(unsigned char *, m_pBuffer, Buffer);
		CC_SYNTHESIZE_READONLY(unsigned long ,  m_uSize,   Size);
	};

#else
	#define CCApplication_SHARED CCApplication::sharedApplication()
	#define CCEGLView_SHARED CCEGLView::sharedOpenGLView()
	#define CCScheduler_SHARED CCScheduler::sharedScheduler()
	#define CCTouchDispatcher_SHARED CCTouchDispatcher::sharedDispatcher()
	#define CCFileUtils_SHARED CCFileUtils

	#define CCMenuItemSprite_FROMSPRITE CCMenuItemSprite::itemFromNormalSprite
	#define INIT_FROM_NORMAL_SPRITE initFromNormalSprite
	#define RENDERTEXTURE_END end(false)
	#define SET_DISPLAY_FPS setDisplayFPS

	#define PREVIOUS_LOCATION_IN_VIEW(touch) previousLocationInView(touch->view())
	#define LOCATION_IN_VIEW(touch) locationInView(touch->view())

	#define CCFileData_IS_USE_DECRYPTION CCFileData::IsUseDecryption
	#define CCFileData_GET_DECRYPTFUNC CCFileData::GetDecryptFunc

	#define SET_IS_ACCELEROMETER_ENABLED setIsAccelerometerEnabled
	#define SET_IS_AUTOREMOVE_ONFINISH setIsAutoRemoveOnFinish
	#define SET_IS_POPUP_NOTIFY setIsPopupNotify
	#define SET_IS_OPACITYMODIFY_RGB setIsOpacityModifyRGB
	#define IS_SELECTED getIsSelected

#endif

#endif

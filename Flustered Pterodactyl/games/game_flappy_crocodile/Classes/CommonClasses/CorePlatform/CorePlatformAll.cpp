#include "CorePlatformAll.h"
#include "platform/CCFileUtils.h"
using namespace cocos2d;

#if COCOS2D_VERSION >= 0x00020000

	CCFileData::CCFileData(const char* pszFileName, const char* pszMode, bool needToEncrypt)
		: m_pBuffer(0), m_uSize(0)
	{
		m_pBuffer = CCFileUtils::sharedFileUtils()->getFileData(pszFileName, pszMode, &m_uSize);
	}

	string CCFileUtilsShared::getWritablePath()
	{ 
		return CCFileUtils::sharedFileUtils()->getWritablePath(); 
	}

	string CCFileUtilsShared::fullPathForFilename(const char* pszRelativePath) 
	{
		return CCFileUtils::sharedFileUtils()->fullPathForFilename(pszRelativePath); 
	}

	void CCFileUtilsShared::SET_IS_POPUP_NOTIFY(bool bNotify)
	{
		CCFileUtils::sharedFileUtils()->SET_IS_POPUP_NOTIFY(bNotify);
	}

    unsigned char* CCFileUtilsShared::getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize)
    {
        return CCFileUtils::sharedFileUtils()->getFileData(pszFileName, pszMode, pSize);
    }

#endif

#pragma once

#ifndef __GAF_COCOA_OBJECT_H__
#define __GAF_COCOA_OBJECT_H__

#include "GAFCocos2dx.h"

namespace GAF
{
	
/**
 @class CCObject
 Internal class of GAF Marmalade Library.
 It is used for emulation of Cocos2dx API and envirmonment
 for the Library. The purpose of the class is to have API
 that match API of Cocos2dx object, but the class itself has minimal
 set  of features. It does not depend on Cocos2dx. We use it to
 unify the code of GAF Libraries across the frameworks.
 All support classes are located in namespace GAF.
 In future GAF Marmalade can support new rendering backend
 (other than Cocos2dx), and the effort is made to minimize
 code differences between libraries. If you are going to use
 only Cocos2dx as rendering backend you do not have to use theese classes or even know about them.
 If you are planning to switch to new backend that GAF Marmalade
 will support you should not use any Cocos2dx class and instead
 use your own or classes from cocoagaf. If you need reference
 counting compatible with any of future releases you should use
 GAF::CCObject as well.
 */	
	class CCObject
	{
	public:
		CCObject();
		virtual ~CCObject();
		void retain();
		void release();
		CCObject* autorelease();
		void setUserData(void * userData)
		{
			_userData = userData;
		}
		void * userData()
		{
			return _userData;
		}
		void setTag(int tag)
		{
			_tag = tag;
		}
		int getTag() const
		{
			return _tag;
		}
	public:
		static void process_autoreleased_objects();
	private:
		unsigned int _autoreleaseCount;
		unsigned int _retainCount;
		unsigned int _tag;
		void *       _userData;
		
	};
}

#endif // __GAF_COCOA_OBJECT_H__

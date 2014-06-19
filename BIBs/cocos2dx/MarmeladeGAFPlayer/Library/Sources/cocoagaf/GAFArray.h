#pragma once

#ifndef __GAF_COCOA_CCARRAY_H__
#define __GAF_COCOA_CCARRAY_H__

#include "GAFObject.h"
#include <vector>

namespace GAF
{
	class CCObject;
	
	/**
	 @class CCArray
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
	class CCArray : public CCObject
	{
	public:
		
		static CCArray* create();
		static CCArray* createWithArray(CCArray * array);
		static CCArray* createWithCapacity(int size);
		
		inline unsigned int count() const
		{
			return _vect.size();
		}
		
		CCObject* lastObject();
		
		CCObject* objectAtIndex(unsigned int index)
		{
			return _vect[index];
		}
		
		void addObject(CCObject* object)
		{
			object->retain();
			_vect.push_back(object);
		}
		
		void removeAllObjects();
		
		void removeLastObject();
		
		void fastRemoveObjectAtIndex(int index);
		
		std::vector<CCObject*> & vect()
		{
			return _vect;
		}
	private:
		std::vector<CCObject*> _vect;
	};
}

#endif // __GAF_COCOA_CCARRAY_H__

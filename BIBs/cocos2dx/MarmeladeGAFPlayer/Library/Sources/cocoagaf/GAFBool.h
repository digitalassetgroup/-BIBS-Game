#pragma once

#ifndef __GAF_COOCA_CCBOOL__
#define __GAF_COOCA_CCBOOL__

#include "GAFObject.h"

namespace GAF
{

/**
@class CCBool
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
	
class CCBool : public CCObject
{
public:

	static CCBool * create(bool v);
	CCBool(bool value)
	:
	_value(value)
	{
		
	}
	inline bool getValue() const
	{
		return _value;
	}
	void setValue(bool value)
	{
		_value = value;
	}
private:
    bool _value;
};

}


#endif // __GAF_COOCA_CCBOOL__
#pragma once

#ifndef __GAF_COCOA_CCAFFINE_TRANSFORM__H___
#define __GAF_COCOA_CCAFFINE_TRANSFORM__H___

#include <string>

#define GAFAffineTransformMake(a, b, c, d, tx, ty) GAF::CCAffineTransform(a, b, c, d, tx, ty)

#define GAFCCAffineTransformMakeIdentity() GAF::CCAffineTransform(1.0, 0.0, 0.0, 1.0, 0.0, 0.0)


namespace GAF
{
	
/**
 @class CCAffineTransform
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
class CCAffineTransform
{
	public:
	inline CCAffineTransform()
	:
	a(1.0f), b(0.0f), c(0.0f), d(1.0f), tx(0.0f), ty(1.0f)
	{
	}		
	inline CCAffineTransform(float _a, float _b, float _c, float _d, float _tx, float _ty)
	:
	a(_a), b(_b), c(_c), d(_d), tx(_tx), ty(_ty)
	{
	}
	float a, b, c, d, tx, ty;
};
}

#endif // __GAF_COCOA_CCAFFINE_TRANSFORM__H___

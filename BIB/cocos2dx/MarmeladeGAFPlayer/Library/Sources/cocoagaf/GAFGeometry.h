#pragma once 

#ifndef __GAF_CC_GEOMETRY_H__
#define __GAF_CC_GEOMETRY_H__

#define GAFCCPointMake(x, y) CCPoint((float)(x), (float)(y))
#define GAFCCSizeMake(width, height) CCSize((float)(width), (float)(height))
#define GAFCCRectMake(x, y, width, height) CCRect((float)(x), (float)(y), (float)(width), (float)(height))

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
class CCPoint
{
public:
	CCPoint();
	CCPoint(float _x, float _y);
	float x, y;
};
	
	
/**
 @class CCSize
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
class CCSize
{
public:
	CCSize();
	CCSize(float w, float h);
	float width, height;
};
	
	
/**
 @class CCRect
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
class CCRect
{
public:
	CCRect();
    CCRect(float x, float y, float width, float height);
    CCPoint origin;
    CCSize  size;
};
	

}

#endif // __GAF_CC_GEOMETRY_H__
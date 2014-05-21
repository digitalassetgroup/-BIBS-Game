#pragma once

#ifndef __GAF_ANIMATED_OBJECT_H__
#define __GAF_ANIMATED_OBJECT_H__

#include <vector>
#include <string>

#include "GAFAnimation.h"
#include "cocoagaf/GAFObject.h"
#include "cocoagaf/GAFGeometry.h"

namespace GAF
{
	
class GAFAnimation;
class GAFSprite;
class GAFAnimator;
class GAFAnimatedObject;
class GAFAsset;
class CCDictionary;
class CCArray;
class CCString;


enum PCAnimationFPSType
{
    PCAnimationFPSType_15 = 15,
    PCAnimationFPSType_30 = 30,
    PCAnimationFPSType_60 = 60
};

enum GAFAnimatedObjectControlFlags
{
    kGAFAnimatedObjectControl_None = 0,
    // If specified, state of controlled object will be changed every frame (like it is by default) and then
    // animatedObject:didDisplayFrameWithSubobject: will be called
    kGAFAnimatedObjectControl_ApplyState = 1 << 0
};
/// @class GAFFramePlayedDelegate
/// You can get notification when particular frame of any GAFAnimatedObject is played.
/// To do this you have to inherit GAFFramePlayedDelegate and call setFramePlayedDelegate
/// method of your GAFAnimatedObject
class GAFFramePlayedDelegate
{
public:
/// Callback funtion, called by GAF.
/// @param object - selected animated object
/// @param frame - frame number that is just played
	virtual void onFramePlayed(GAFAnimatedObject * object, int frame);
};

class GAFAnimatedObjectControlDelegate
{
public:
	virtual void onFrameDisplayed(GAFAnimatedObject * object, GAFSprite * subobject);
};

/// @class GAFAnimatedObject
class GAFAnimatedObject : public GAF::CCObject, public GAFAnimation
{
public:
	~GAFAnimatedObject();
	static GAFAnimatedObject * create(GAFAsset * anAsset);
	static GAFAnimatedObject * create(const char * jsonPath);	
	static GAFAnimatedObject * createAndRun(const char * jsonPath, bool looped = false);

	bool init(GAFAsset * anAsset);	
	void processAnimations(float dt);
	CCPoint pupilCoordinatesWithXSemiaxis(float anXSemiaxis, float anYSemiaxis, CCPoint aCenter, CCPoint anExternalPoint);
	GAFSprite * subObjectForInnerObjectId(CCString * anInnerObjectId);
	void removeAllSubObjects();
	void addSubObjectsUsingAnimationObjectsDictionary(CCDictionary * anAnimationObjects, CCDictionary * anAnimationMasks, CCArray * anAnimationFrames);
	void setSubobjectsVisible(bool visible);
	CCDictionary * subObjects();
	CCDictionary * masks();
	void animatorDidPlayedFrame(GAFAnimator * anAnimator, int aFrameNo);
/// Returns subobject which has specified name assinged to it ("namedParts" section in config)	
	GAFSprite * subobjectByName(const char * name);
/// Returns subobject by raw name (usually "Zn", where n is natural number (1,2,3,...))
	GAFSprite * subobjectByRawName(const char * name);
	virtual void start();
	virtual void stop();
	virtual void processAnimation();
/// Takes control over subobject, which means that every frame control delegate will be notified to decide
/// what to do with captured external object
/// @note it supports only objects for now, DOES NOT SUPPORT MASKS
/// @param name subobject name taken from "namedParts" section in config
/// @param controlFlags flags specifying what played will do with subobjects controlled externally
/// @returns YES if control was successfully taken and all future
	bool captureControlOverSubobjectNamed(const char * name, GAFAnimatedObjectControlFlags controlFlags);
/// Releases control over subobject captured earlier	
	void releaseControlOverSubobjectNamed(const char * name);
	
	void setFramePlayedDelegate(GAFFramePlayedDelegate * delegate);
	void setControlDelegate(GAFAnimatedObjectControlDelegate * delegate);
/// @return external pointer to GAF rendering backend object		
	void * getExternalPointer()
	{
		return _externalSpriteContainer;
	}
	void setScale(float scale);
	void setPosition(float x, float y);
protected:
	GAFAnimatedObject();
	GAFSprite * subObjectForInnerObjectId(const char * anInnerObjectId);
	std::string objectIdByObjectName(const char * aName);
private:
	GAFAsset * _asset;
	CCDictionary   * _subObjects;
	CCDictionary   * _masks;
	CCDictionary   * _capturedObjects;
	bool _animationsSelectorScheduled;
	GAFFramePlayedDelegate * _framePlayedDelegate;
	GAFAnimatedObjectControlDelegate * _controlDelegate;
	
	int numberOfGlobalFramesForOneAnimationFrame();
	int _extraFramesCounter;
	PCAnimationFPSType _FPSType;
	void * _externalSpriteContainer;

}; // GAFAnimatedObject
	
}; // GAF

#endif // __GAF_ANIMATED_OBJECT_H__

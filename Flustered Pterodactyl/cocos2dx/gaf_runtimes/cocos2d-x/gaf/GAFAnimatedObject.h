#pragma once

#ifndef __GAF_ANIMATED_OBJECT_H__
#define __GAF_ANIMATED_OBJECT_H__

#include "layers_scenes_transitions_nodes/CCLayer.h"
#include "GAFAnimation.h"
#include <vector>
#include <string>

class GAFAnimation;
class GAFSprite;
class GAFAnimator;
class GAFAnimatedObject;
class GAFAsset;

namespace cocos2d
{
	class CCDictionary;
	class CCArray;
	class CCString;
}

using namespace cocos2d;

enum PCAnimationFPSType
{
    PCAnimationFPSType_15 = 15,
    PCAnimationFPSType_30 = 30,
    PCAnimationFPSType_60 = 60
};

class GAFFramePlayedDelegate
{
public:
	virtual void onFramePlayed(GAFAnimatedObject * object, int frame);
};

class GAFAnimatedObject : public CCLayer, public GAFAnimation
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
	GAFSprite * getNamedSubobject(const char * name);
	// call this function only if you really know what you do
	GAFSprite * getSubobjectByRawName(const char * name);
	virtual void start();
	virtual void stop();
	virtual void processAnimation();
	
protected:
	GAFAnimatedObject();
private:
	CCDictionary * masksAssociationStructureFromAnimationObjectIds(CCArray * anAnimationObjectIds, CCArray * anAnimationFrames, CCArray **aMaskedObjectIds);
private:
	GAFAsset * _asset;
	CCDictionary   * _subObjects;
	CCDictionary   * _masks;
	bool _animationsSelectorScheduled;
	GAFFramePlayedDelegate * _framePlayedDelegate;
	
	int numberOfGlobalFramesForOneAnimationFrame();
	int _extraFramesCounter;
	PCAnimationFPSType _FPSType;

}; // GAFAnimatedObject

#endif // __GAF_ANIMATED_OBJECT_H__

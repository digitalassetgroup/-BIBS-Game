#pragma once

#ifndef __GAF_ANIMATION_FRAME__
#define __GAF_ANIMATION_FRAME__

#include "cocoagaf/GAFObject.h"

namespace GAF
{

class CCDictionary;
class CCArray;
class GAFTextureAtlas;

class GAFAnimationFrame : public CCObject
{
public:		
	GAFAnimationFrame();
	~GAFAnimationFrame();
	CCArray * objectStates();
	void setObjectStates(CCArray * states);
private:
	CCArray * _initialInstructions;
	CCArray * _objectsStates;
};
	
} // namespace GAF

#endif // __GAF_ANIMATION_FRAME__

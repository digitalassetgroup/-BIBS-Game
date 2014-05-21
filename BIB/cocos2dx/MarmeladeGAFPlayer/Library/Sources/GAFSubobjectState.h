#pragma once

#ifndef __GAF_SUBOBJECT_STATE__
#define __GAF_SUBOBJECT_STATE__

#include "cocoagaf/GAFObject.h"
#include "cocoagaf/GAFAffineTransform.h"

#include <string>

namespace GAF
{
	
class CCDictionary;


enum GAFColorTransformIndex
{
	GAFCTI_R,
	GAFCTI_G,
	GAFCTI_B,
	GAFCTI_A,
};

class GAFSubobjectState : public CCObject
{
public:
	std::string objectId;
	int zIndex;
	std::string maskObjectId;
	CCAffineTransform affineTransform;
	
	static GAFSubobjectState * createWithStateDictionary(CCDictionary * dict, const char * objectId);
	static GAFSubobjectState * createEmptyWithObjectId(const char * objectId);
	
	bool initWithStateDictionary(CCDictionary * dict, const char * objectId);
	bool initEmptyWinthObjectId(const char * objectId);
	~GAFSubobjectState();
	inline CCDictionary * filters()
	{
		return _filters;
	}
	inline float * colorMults()
	{
		return &_colorMults[0];
	}
	inline float * colorOffsets()
	{
		return &_colorOffsets[0];
	}
	inline bool isVisisble() const
	{
		return _colorMults[GAFCTI_A] != 0;
	}
protected:
	GAFSubobjectState();
private:
	CCDictionary * _filters;
	float   _colorMults[4];
	float _colorOffsets[4];
}; // GAFSubobjectState
	
} // namespace GAF

#endif // __GAF_SUBOBJECT_STATE__
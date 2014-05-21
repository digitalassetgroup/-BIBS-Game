#pragma once

#ifndef __GAF_TEXTURE_ATLAS_ELEMENT__
#define __GAF_TEXTURE_ATLAS_ELEMENT__

#include <string>

#include "cocoagaf/GAFObject.h"
#include "cocoagaf/GAFGeometry.h"

namespace GAF
{
	
class CCDictionary;

class GAFTextureAtlasElement : public CCObject
{
public:
	std::string name;	
	CCPoint     pivotPoint;
	CCRect      bounds;
	float       scale;
	int         atlasIdx;
	static GAFTextureAtlasElement * create(CCDictionary * aDictionary);
	bool initWithDictionary(CCDictionary * aDictionary);
private:
	GAFTextureAtlasElement();
}; // GAFTextureAtlasElement

}

#endif // __GAF_TEXTURE_ATLAS_ELEMENT__

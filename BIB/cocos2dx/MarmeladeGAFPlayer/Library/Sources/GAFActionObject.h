#pragma once

#ifndef __GAF_ACTION_OBJECT__
#define __GAF_ACTION_OBJECT__

#include <string>
#include "cocoagaf/GAFObject.h"
#include "cocoagaf/GAFGeometry.h"

namespace GAF
{
	
class CCDictionary;

class GAFActionObject : public CCObject
{
friend class GAFAsset;
public:
///
/// Name of the object/subobject. You can query object/suboject by name in your application.
///
	std::string name;
///
/// Pivot point of the object/subobject with respect to object hierarchy. For advanced users only.
///
	CCPoint     pivotPoint;
///
/// Bounds of the object/subobject with respect to texture atlas. For advanced users only.
///
	CCRect      bounds;
private:
	static GAFActionObject * create(CCDictionary * aDictionary);
	bool initWithDictionary(CCDictionary * aDictionary);
	GAFActionObject();

}; // GAFInteractionObject

} // namespace GAF

#endif // __GAF_ACTION_OBJECT__

#include "GAFInteractionObject.h"

#include "cocoa/CCInteger.h"
#include "cocoa/CCDictionary.h"

static const char * kNameKey        = "name";
static const char * kXKey           = "x";
static const char * kYKey           = "y";
static const char * kRectXKey       = "rectX";
static const char * kRectYKey       = "rectY";
static const char * kRectWidthKey   = "rectWidth";
static const char * kRectHeightKey  = "rectHeight";

GAFInteractionObject::GAFInteractionObject()
{
	
}

GAFInteractionObject * GAFInteractionObject::create(CCDictionary * aDictionary)
{
	GAFInteractionObject * ret = new GAFInteractionObject();
	if (ret && ret->initWithDictionary(aDictionary))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;	
}

bool GAFInteractionObject::initWithDictionary(CCDictionary * aDictionary)
{
	if (!aDictionary)
	{
		CCLOGERROR("Error while creating GAFInteractionObject from NSDictionary. aDictionary must not be NULL.");
		return false;
	}
	CCString * jName = (CCString*)aDictionary->objectForKey(kNameKey);
	if (jName)
	{
		name = std::string(jName->getCString());
	}
	
	CCInteger * rectX      = (CCInteger*)aDictionary->objectForKey(kRectXKey);
	CCInteger * rectY      = (CCInteger*)aDictionary->objectForKey(kRectYKey);
	CCInteger * rectWidth = (CCInteger*)aDictionary->objectForKey(kRectWidthKey);
	CCInteger * rectHeight  = (CCInteger*)aDictionary->objectForKey(kRectHeightKey);
	
	if (rectX && rectY && rectWidth && rectHeight)
	{
		bounds = CCRectMake(rectX->getValue(), rectY->getValue(), rectWidth->getValue(), rectHeight->getValue());
	}
	else
	{
		CCLOGERROR("Error while creating GAFInteractionObject from NSDictionary. One of rect* attributes missing.");
		return false;
	}
	
	CCInteger * x      = (CCInteger*)aDictionary->objectForKey(kXKey);
	CCInteger * y      = (CCInteger*)aDictionary->objectForKey(kYKey);
	
	if (x && y)
	{
		pivotPoint = CCPointMake(x->getValue(), y->getValue());
	}
	else
	{
		CCLOGERROR("Error while creating GAFInteractionObject from NSDictionary. One of x, y attributes missing.");
		return false;
	}
	return true;
}


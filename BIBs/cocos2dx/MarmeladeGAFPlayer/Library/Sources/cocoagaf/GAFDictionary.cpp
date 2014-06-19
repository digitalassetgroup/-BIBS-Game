#include "GAFDictionary.h"
#include "GAFArray.h"


namespace GAF
{

CCDictionary* CCDictionary::create()
{
	CCDictionary* pRet = new CCDictionary();
	if (pRet != NULL)
	{
		pRet->autorelease();
	}
	return pRet;
}
	
void CCDictionary::setObject(CCObject * object, const std::string& key)
{
	object->retain();
	(*this)[key] = object;
}

void CCDictionary::removeAllObjects()
{
	GAFDICT_FOREACH(this, pElement)
    {
		CC_SAFE_RELEASE(pElement->second);
	}
	this->clear();
}
	
	
void CCDictionary::removeObjectForKey(const std::string & key)
{
	CCDictionaryIt it = this->find(key);
	if (it != this->end())
	{
		CC_SAFE_RELEASE(it->second);
		this->erase(it);
	}
}
	
CCObject * CCDictionary::objectForKey(const std::string& key)
{
	CCDictionaryIt it = this->find(key);
	if (it != this->end())
	{
		return it->second;
	}
	return NULL;
}
	
CCArray * CCDictionary::allKeys()
{
	CCArray * res = CCArray::create();
	if (res)
	{
		GAFDICT_FOREACH(this, pElement)
		{
			res->addObject(pElement->second);
		}
	}

	return res;
}
	
}


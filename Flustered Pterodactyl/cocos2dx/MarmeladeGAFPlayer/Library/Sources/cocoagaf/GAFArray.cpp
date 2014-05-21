#include "GAFArray.h"

namespace GAF
{

CCArray* CCArray::create()
{
    CCArray* res = new CCArray();
    if (res)
    {
        return (CCArray*)res->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(res);
		return NULL;
    }
}
	
CCArray* CCArray::createWithArray(CCArray * array)
{
	CCArray* res = create();
	if (res)
	{
		for (unsigned int i = 0; i < array->count(); ++i)
		{
			res->addObject(array->objectAtIndex(i));
		}
	}
	return res;
}
	
CCArray* CCArray::createWithCapacity(int size)
{
	CCArray* res = new CCArray();
    if (res)
    {
		res->_vect.reserve(size);
        return (CCArray*)res->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(res);
		return NULL;
    }
}
	
void CCArray::removeAllObjects()
{
	for (unsigned int i = 0; i < _vect.size(); ++i)
	{
		CC_SAFE_RELEASE(_vect[i]);
	}
	_vect.clear();
}
	
void CCArray::removeLastObject()
{
	if (_vect.size())
	{
		CC_SAFE_RELEASE(_vect.back());
		_vect.pop_back();
	}
}
	
CCObject * CCArray::lastObject()
{
	if (!_vect.size())
	{
		return NULL;
	}
	return _vect[_vect.size() - 1];
}
	
void CCArray::fastRemoveObjectAtIndex(int index)
{
	CC_SAFE_RELEASE(_vect[index]);
	if (index != int(_vect.size() - 1))
	{
		_vect[index] = _vect.back();
	}
	_vect.pop_back();
}

} // namespace GAF
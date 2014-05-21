#include "GAFObject.h"
#include "IwDebug.h"
#include <vector>

namespace GAF
{

static std::vector<CCObject *> _autoreleasedPool;
	
CCObject::CCObject()
	:
_autoreleaseCount(0),
_retainCount(1),
_userData(NULL),
_tag(0)
{
	
}
	
CCObject::~CCObject()
{
	if (_autoreleaseCount > 0)
	{
		for (unsigned int i = 0; i < _autoreleasedPool.size(); ++i)
		{
			if (_autoreleasedPool[i] == this)
			{
				_autoreleasedPool[i] = 0;
				break;
			}
		}
	}	
}
	
void CCObject::retain()
{
	GAFAssert(_retainCount > 0, "reference count should greater than 0");
    ++_retainCount;
}
	
void CCObject::release()
{	
    GAFAssert(_retainCount > 0, "reference count should greater than 0");
    --_retainCount;	
    if (0 == _retainCount)
    {
        delete this;
    }
}

// GAF on marmalade use simple minimalistic autorelease pool emulation
// we do not allow nested autoreleased pools
	

	
CCObject* CCObject::autorelease()
{
	GAFAssert(_autoreleaseCount == 0, "must be autoreleased only once");
	++_autoreleaseCount;
	_autoreleasedPool.push_back(this);
	return this;
}
	
void CCObject::process_autoreleased_objects()
{
	for (unsigned int i = 0; i < _autoreleasedPool.size(); ++i)
	{
		if (_autoreleasedPool[i])
		{
			_autoreleasedPool[i]->release();
		}		
	}
	_autoreleasedPool.clear();
}
	

}


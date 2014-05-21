#pragma once
#ifndef MemoryManagement_h
#define MemoryManagement_h

#include "CommonMacros.h"
#include "QPointer.h"

/*
// Ideally pointers should be wrapped in a class for protection and their destruction usually happens in the destructor.
// This isn't really 'safe' for various reasons
template<class T> void SafeDelete(T*& pVal)
{
    if (nullptr != pVal) 
    {
        delete pVal;
        pVal = nullptr;
    }
}

// Ideally pointers should be wrapped in a class for protection and their destruction usually happens in the destructor.
// This isn't really 'safe' for various reasons
template<class T> void SafeDeleteArray(T*& pVal)
{
    if(nullptr != pVal) 
    { 
        delete[] pVal;
        pVal = nullptr;
    }
}
*/

#define SAFE_DELETE(o) if (o != NULL) { delete o; o = NULL; }
#define SAFE_RELEASE(o) if (o != NULL) { o->release(); o = NULL; }

/*
   Some classes do not permit copies to be made of an object. These
   classes contains a private copy constructor and assignment
   operator to disable copying (the compiler gives an error message).
*/
#define Q_DISABLE_COPY(Class) \
    Class(const Class &); \
    Class &operator=(const Class &);


class StaticHolder
{
	typedef void (*CustomDeleter)();
	private_ CustomDeleter _deleter;

	public_ StaticHolder(CustomDeleter deleter)
		: _deleter(deleter) {}

	public_ ~StaticHolder()
	{
		if (_deleter)
			_deleter();
	}
};

template<typename T>
class StaticHolderWithResource
{
	typedef void (*CustomDeleter)(T obj);
	private_ CustomDeleter _deleter;
	private_ T* _obj;

	public_ StaticHolderWithResource(CustomDeleter deleter, T* obj)
		: _deleter(deleter), _obj(obj) {}

	public_ ~StaticHolderWithResource()
	{
		if (_deleter != NULL)
			_deleter(*_obj);
	}
};

#endif

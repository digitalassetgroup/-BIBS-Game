#pragma once
#ifndef ObjectPool_h
#define ObjectPool_h

#include "NonCopyable.h"
#include "CollectionHelper.h"
#include <assert.h>

template<typename T>
class ObjectPool : NonCopyable
{
	private_ std::vector<T*> _objects;
	private_ int _startedCount; //начальное число объектов

	public_ ObjectPool(int startedCount)
		: _startedCount(startedCount)
	{
		_objects.reserve(startedCount);
	}

	~ObjectPool()
	{
		for_each(auto obj, _objects)
			delete obj;
	}

	private_ T* Create()
	{
		return new T();
	}

	public_ void Init()
	{
		while (_startedCount--)
		{
			T* obj = Create();
			_objects.push_back(obj);
		}
	}

	public_ T* Get()
	{
		if (_objects.empty())
		{
			assert(false);
			//увеличиваем размер			
		}

		T* obj = _objects.back();
		_objects.pop_back();
		return obj;
	}

	//возвратить объект в пул
	public_ void Return(T* obj)
	{
		_objects.push_back(obj);
	}
};

#endif

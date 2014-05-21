#pragma once
#ifndef ScopedObjectiveObject_h
#define ScopedObjectiveObject_h

#include "QPointer.h"
#include "NonCopyable.h"

namespace MemoryManagement
{
	//RAII для объектов, отнаследованных от CCObject (в стиле Objective-C)
	//просто scope
	template<typename T>
	class ObjectScope
	{
		private_ T* _data;
		public_ ObjectScope(T* data) : _data(data)
		{ 
			_data->retain(); 
		}

		~ObjectScope() 
		{ 
			_data->release(); 
		}
	};

	template<typename T>
	class CCScopedObject : public Qt::QPointerBase<T>
	{
		public_ CCScopedObject(T* data) : Qt::QPointerBase<T>(data) 
		{ 
			this->_data->retain();
		}
		
		~CCScopedObject() 
		{ 
			this->_data->release();
		}

		public_ inline operator T*() const { return this->_data; }
	};
}

#endif

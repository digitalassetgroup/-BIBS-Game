#pragma once
#ifndef QPointer_h
#define QPointer_h

#include "CommonMacros.h"
#include <assert.h>

//Смарт-поинтер, по котором можно определить, удален ли внутренний объект, или нет
//т.о. преимущества такие:
//1. Можно вызывать delete для этого объекта много раз  (но это антипаттерн, конечно)
//2. Можно проверить (через isNull), удален ли этот объект (но это тоже антипаттерн, т.е. в логике программы какой-то косяк)
//3. По крайней мере, с помощью этого класса можно проверять на ошибки доступа к висячему поинтеру

/*
QPointer<T>, behaves like a normal C++
pointer \c{T *}, except that it is automatically set to 0 when the
referenced object is destroyed (unlike normal C++ pointers, which
become "dangling pointers" in such cases)

Guarded pointers are useful whenever you need to store a pointer
to a QObject that is owned by someone else, and therefore might be
destroyed while you still hold a reference to it. You can safely
test the pointer for validity.

Qt also provides QSharedPointer, an implementation of a reference-counted
shared pointer object, which can be used to maintain a collection of
references to an individual pointer.
*/

namespace Qt
{
	//Базовый класс для всех смарт-поинтеров
	//Позволяет хранить указать на объект и обращаться к нему
	template<class T>
	class QPointerBase
	{
		protected_ T* /*const*/ _data; //const to pointer! констанстный указатель
		protected_ inline QPointerBase(T* p = 0) : _data(p) {}

		public_ inline bool isNull() const 
		{
			return (_data == 0); 
		}

		public_ inline T* get() const
		{ 
			return _data; 
		}

		//Overloaded arrow operator; implements pointer semantics. Just use
		//this operator as you would with a normal C++ pointer.
		public_ inline T* operator->() const { assert(_data); return _data; }

		//Dereference operator; implements pointer semantics. Just use this
		//operator as you would with a normal C++ pointer.
		public_ inline T& operator*() const { assert(_data); return *_data; }

		//public_ inline operator bool() const { return isNull() ? NULL : &_data; }
		//public_ inline bool operator!() const { return isNull(); }

		//Cast operator; implements pointer semantics. Because of this
		//function you can pass a QPointer\<T\> to a function where a T*
		//is required.
		public_ inline operator T*() const { return this->_data; }
	};


	#pragma region equality operator

	template <class T>
	inline bool operator==(const T* o, const QPointerBase<T>& p)
	{ 
		return o == p.get(); 
	}

	template<class T>
	inline bool operator==(const QPointerBase<T>& p, const T* o)
	{ 
		return p.get() == o; 
	}

	template <class T>
	inline bool operator==(T *o, const QPointerBase<T>& p)
	{ 
		return o == p.get(); 
	}

	template<class T>
	inline bool operator==(const QPointerBase<T>& p, T* o)
	{ 
		return p.get() == o; 
	}

	template<class T>
	inline bool operator==(const QPointerBase<T>& p1, const QPointerBase<T>& p2)
	{ 
		return p1.get() == p2.get(); 
	}

	template <class T>
	inline bool operator!=(const T* o, const QPointerBase<T>& p)
	{ 
		return o != p.get(); 
	}

	template<class T>
	inline bool operator!=(const QPointerBase<T>& p, const T* o)
	{ 
		return p.get() != o; 
	}

	template <class T>
	inline bool operator!=(T* o, const QPointerBase<T>& p)
	{ 
		return o != p.get(); 
	}

	template<class T>
	inline bool operator!=(const QPointerBase<T>& p, T* o)
	{ 
		return p.get() != o; 
	}

	template<class T>
	inline bool operator!=(const QPointerBase<T>& p1, const QPointerBase<T>& p2)
	{ 
		//return p1.operator->() != p2.operator->(); 
		return p1.get() != p2.get();
	}

	#pragma endregion


	//это типа 	unique_ptr
	template<class T> 
	class QPointer : public QPointerBase<T>
	{
		public_ inline QPointer(T* p = 0) : QPointerBase<T>(p) {}

		//комментим копирующий конструктор
		//низяяяя!!!!
		//public_ inline QPointer(const QPointer<T>& p) : _data(p._data) {}	

		public_	inline ~QPointer()
		{
			if (this->_data)
			{
				delete this->_data;
				this->_data = 0;
			}
		}

		#pragma region operators

		//Assignment operator. This guarded pointer will now point to the
		//same object that \a p points to.
		//public_ inline QPointer<T>& operator=(const QPointer<T>& p) { return *this; }

		//Assignment operator. This guarded pointer will now point to the
		//same object that \a p points to.
		//public_ inline QPointer<T>& operator=(T* p) { return *this; }

		#pragma endregion
	};

	
	template<class T>
	class QSharedPointer : public QPointerBase<T>
	{
		public_ QSharedPointer() : QPointerBase<T>()
		{
		}

		public_ QSharedPointer(T* data) : QPointerBase<T>(data) 
		{
			assert(this->_data);
			IncrementRef();
		}

		public_ QSharedPointer(QSharedPointer<T>& p) : QPointerBase<T>(p._data)
		{
			assert(this->_data);
			IncrementRef();
		}

		public_	QSharedPointer<T>& operator=(QSharedPointer<T>& right)
		{
			this->DecrementRef();
		    this->_data = right._data;
			this->IncrementRef();
			return *this;
		}

		private_ void IncrementRef()
		{
			if (this->_data)
				++this->_data->RefCount;
		}

		private_ void DecrementRef()
		{
			if (this->_data == 0)
				return;	

			--this->_data->RefCount;
			assert(this->_data->RefCount >= 0);
			if (this->_data->RefCount == 0)
			{
				delete this->_data;
				this->_data = 0;
			}
		}

		public_ ~QSharedPointer()
		{
			DecrementRef();
		}

		public_ int use_count()
		{
			return (this->_data) ? this->_data->RefCount : 0;
		}
	};
}

#endif

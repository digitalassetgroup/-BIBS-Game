#pragma once
#ifndef QScopedPointer_h
#define QScopedPointer_h

#include "NonCopyable.h"
#include "MemoryManagement.h"
#include "QPointer.h"

namespace Qt
{
	template <typename T>
	struct QScopedPointerArrayDeleter
	{
		static inline void cleanup(T *pointer)
		{
			// Enforce a complete type.
			// If you get a compile error here, read the secion on forward declared
			// classes in the QScopedPointer documentation.
			typedef char IsIncompleteType[ sizeof(T) ? 1 : -1 ];
			(void) sizeof(IsIncompleteType);

			delete[] pointer;
		}
	};

	template<typename T>
	struct QScopedPointerDeleter
	{
		static inline void cleanup(T *pointer)
		{
			// Enforce a complete type.
			// If you get a compile error here, read the secion on forward declared
			// classes in the QScopedPointer documentation.
			// http://doc.qt.nokia.com/4.7-snapshot/qscopedpointer.html
			typedef char IsIncompleteType[ sizeof(T) ? 1 : -1 ];
			(void) sizeof(IsIncompleteType);

			delete pointer;
		}
	};

	template<typename T>
	struct QScopedPointerNoDeleter
	{
		static inline void cleanup(T *pointer) {}
	};


	template<typename T>
	struct QScopedPointerReleaseDeleter
	{
		static inline void cleanup(T* pointer)
		{		
			typedef char IsIncompleteType[ sizeof(T) ? 1 : -1 ];
			(void) sizeof(IsIncompleteType);

			if (pointer != NULL)
				pointer->release();
		}
	};

	template <typename T, typename Cleanup = QScopedPointerDeleter<T> >
	class QScopedPointer : private NonCopyable, public QPointerBase<T>
	{
		public_ explicit inline QScopedPointer(T *p = 0) : QPointerBase<T>(p) 
		{
		}

		public_ inline ~QScopedPointer()
		{
			T *oldD = this->_data;
			Cleanup::cleanup(oldD);
			this->_data = 0;
		}

		public_ inline void reset(T* other = NULL)
		{
			if (this->_data == other)
				return;
			T *oldD = this-> _data;
			this->_data = other;
			Cleanup::cleanup(oldD);
		}

		public_ inline T* take()
		{
			T *oldD = this->_data;
			this->_data = NULL;
			return oldD;
		}

		public_ inline void swap(QScopedPointer<T, Cleanup>& other)
		{
			qSwap(this->_data, other._data);
		}
		
		//Q_DISABLE_COPY(QScopedPointer)
	};


	template <typename T, typename Cleanup = QScopedPointerArrayDeleter<T> >
	class QScopedArrayPointer : public QScopedPointer<T, Cleanup>
	{
	public:
		explicit inline QScopedArrayPointer(T *p = 0)
			: QScopedPointer<T, Cleanup>(p)
		{
		}

		inline T &operator[](int i) { return this->d[i]; }
		inline const T &operator[](int i) const { return this->d[i]; }

	private:
		Q_DISABLE_COPY(QScopedArrayPointer)
	};


	template<typename T, typename Cleanup = QScopedPointerReleaseDeleter<T> >
	class QScopedPointerRelease : public QScopedPointer<T, Cleanup>
	{
	public:
		explicit inline QScopedPointerRelease(T *p = 0)
			: QScopedPointer<T, Cleanup>(p)
		{
		}
	};


	template<typename T, typename Cleanup = QScopedPointerNoDeleter<T> >
	class QScopedPointerNoDeletion : public QScopedPointer<T, Cleanup>
	{
	public:
		explicit inline QScopedPointerNoDeletion(T *p = 0)
			: QScopedPointer<T, Cleanup>(p)
		{
		}
	};
}

#endif

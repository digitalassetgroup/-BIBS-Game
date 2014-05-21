#pragma once
#ifndef shared_ptr_my_h
#define shared_ptr_my_h

namespace MemoryManagement
{
		// CLASS _Ref_count_base
class _Ref_count_base
	{	// common code for reference counting
private:
	virtual void _Destroy() = 0;
	virtual void _Delete_this() = 0;

	long _Uses;
	long _Weaks;

protected:
	_Ref_count_base()
		: _Uses(1), _Weaks(1)
		{	// construct
		}

public:
	virtual ~_Ref_count_base()
		{	// ensure that derived classes can be destroyed properly
		}

	bool _Incref_nz()
		{	// increment use count if not zero, return true if successful
		for (; ; )
			{	// loop until state is known
			if (_Uses == 0)
				return (false);
			++_Uses;
			return (true);
			}
		}

	void _Incref()
		{	// increment use count
			++_Uses;
		}

	void _Incwref()
		{	// increment weak reference count
		++_Weaks;
		}

	void _Decref()
		{	// decrement use count
		--_Uses;
		if (_Uses == 0)
			{	// destroy managed resource, decrement weak reference count
			_Destroy();
			_Decwref();
			}
		}

	void _Decwref()
		{	// decrement weak reference count
		--_Weaks;
		if (_Weaks == 0)
			_Delete_this();
		}

	long _Use_count() const
		{	// return use count
		return (_Uses);
		}

	bool _Expired() const
		{	// return true if _Uses == 0
		return (_Uses == 0);
		}
	};

	// TEMPLATE CLASS _Ref_count, _Ref_count_del, _Ref_count_del_alloc
template<class _Ty>
	class _Ref_count
	: public _Ref_count_base
	{	// handle reference counting for object without deleter
public:
	_Ref_count(_Ty *_Px)
		: _Ref_count_base(), _Ptr(_Px)
		{	// construct
		}

private:
	virtual void _Destroy()
		{	// destroy managed resource
		delete _Ptr;
		}

	virtual void _Delete_this()
		{	// destroy self
		delete this;
		}

	_Ty * _Ptr;
	};

template<class _Ty,
	class _Dx>
	class _Ref_count_del
	: public _Ref_count_base
	{	// handle reference counting for object with deleter
public:
	_Ref_count_del(_Ty *_Px, _Dx _Dt)
		: _Ref_count_base(), _Ptr(_Px), _Dtor(_Dt)
		{	// construct
		}

private:
	virtual void _Destroy()
		{	// destroy managed resource
		_Dtor(_Ptr);
		}

	virtual void _Delete_this()
		{	// destroy self
		delete this;
		}

	_Ty * _Ptr;
	_Dx _Dtor;	// the stored destructor for the controlled object
	};

template<class _Ty,
	class _Dx,
	class _Alloc>
	class _Ref_count_del_alloc
	: public _Ref_count_base
	{	// handle reference counting for object with deleter and allocator
public:
	typedef _Ref_count_del_alloc<_Ty, _Dx, _Alloc> _Myty;
	typedef typename _Alloc::template rebind<_Myty>::other _Myalty;

	_Ref_count_del_alloc(_Ty *_Px, _Dx _Dt, _Myalty _Al)
		: _Ref_count_base(), _Ptr(_Px), _Dtor(_Dt), _Myal(_Al)
		{	// construct
		}

private:
	virtual void _Destroy()
		{	// destroy managed resource
		_Dtor(_Ptr);
		}

	virtual void _Delete_this()
		{	// destroy self
		_Myalty _Al = _Myal;
		_Dest_val(_Al, this);
		_Al.deallocate(this, 1);
		}

	_Ty * _Ptr;
	_Dx _Dtor;	// the stored destructor for the controlled object
	_Myalty _Myal;	// the stored allocator for this
	};

// DECLARATIONS
template<class _Ty>
	class shared_ptr;
template<class _Ty>
	class enable_shared_from_this;
struct _Static_tag {};
struct _Const_tag {};
struct _Dynamic_tag {};

template<class _Ty1,
	class _Ty2>
	void _Do_enable(_Ty1 *, enable_shared_from_this<_Ty2> *,
		_Ref_count_base *);

template<class _Ty>
	inline void _Enable_shared(_Ty *_Ptr, _Ref_count_base *_Refptr,
		typename _Ty::_EStype * = 0)
	{	// reset internal weak pointer
	if (_Ptr)
		_Do_enable(_Ptr,
			(enable_shared_from_this<typename _Ty::_EStype>*)_Ptr, _Refptr);
	}

inline void _Enable_shared(const volatile void *, const volatile void *)
{	// not derived from enable_shared_from_this; do nothing
}

		// TEMPLATE CLASS _Ptr_base
template<class _Ty>
	class _Ptr_base
	{	// base class for shared_ptr and weak_ptr
public:
	typedef _Ptr_base<_Ty> _Myt;
	typedef _Ty _Elem;
	typedef _Elem element_type;

	_Ptr_base()
		: _Ptr(0), _Rep(0)
		{	// construct
		}

	template<class _Ty2>
		_Ptr_base(_Ptr_base<_Ty2>&& _Right)
		: _Ptr(_Right._Ptr), _Rep(_Right._Rep)
		{	// construct _Ptr_base object that takes resource from _Right
		_Right._Ptr = 0;
		_Right._Rep = 0;
		}

	void _Assign_rv(_Myt&& _Right)
		{	// assign by moving _Right
		if (this != &_Right)
			_Swap(_Right);
		}

	long use_count() const
		{	// return use count
		return (_Rep ? _Rep->_Use_count() : 0);
		}

	template<class _Ty2>
		bool owner_before(const _Ptr_base<_Ty2>& _Right) const
		{	// compare addresses of manager objects
		return (_Rep < _Right._Rep);
		}

	_Ty *_Get() const
		{	// return pointer to resource
		return (_Ptr);
		}

	bool _Expired() const
		{	// test if expired
		return (!_Rep || _Rep->_Expired());
		}

	void _Decref()
		{	// decrement reference count
		if (_Rep != 0)
			_Rep->_Decref();
		}

	void _Reset()
		{	// release resource
		_Reset(0, 0);
		}

	template<class _Ty2>
		void _Reset(const _Ptr_base<_Ty2>& _Other)
		{	// release resource and take ownership of _Other._Ptr
		_Reset(_Other._Ptr, _Other._Rep, false);
		}

	template<class _Ty2>
		void _Reset(const _Ptr_base<_Ty2>& _Other, bool _Throw)
		{	// release resource and take ownership from weak_ptr _Other._Ptr
		_Reset(_Other._Ptr, _Other._Rep, _Throw);
		}

	template<class _Ty2>
		void _Reset(const _Ptr_base<_Ty2>& _Other, const _Static_tag&)
		{	// release resource and take ownership of _Other._Ptr
		_Reset(static_cast<_Elem *>(_Other._Ptr), _Other._Rep);
		}

	template<class _Ty2>
		void _Reset(const _Ptr_base<_Ty2>& _Other, const _Const_tag&)
		{	// release resource and take ownership of _Other._Ptr
		_Reset(const_cast<_Elem *>(_Other._Ptr), _Other._Rep);
		}

	template<class _Ty2>
		void _Reset(const _Ptr_base<_Ty2>& _Other, const _Dynamic_tag&)
		{	// release resource and take ownership of _Other._Ptr
		_Elem *_Ptr = dynamic_cast<_Elem *>(_Other._Ptr);
		if (_Ptr)
			_Reset(_Ptr, _Other._Rep);
		else
			_Reset();
		}

 #if _HAS_CPP0X
	template<class _Ty2>
		void _Reset(_Ty *_Ptr, const _Ptr_base<_Ty2>& _Other)
		{	// release resource and alias _Ptr with _Other_rep
		_Reset(_Ptr, _Other._Rep);
		}
 #endif /* _HAS_CPP0X */

	void _Reset(_Ty *_Other_ptr, _Ref_count_base *_Other_rep)
		{	// release resource and take _Other_ptr through _Other_rep
		if (_Other_rep)
			_Other_rep->_Incref();
		_Reset0(_Other_ptr, _Other_rep);
		}

	void _Reset(_Ty *_Other_ptr, _Ref_count_base *_Other_rep, bool _Throw)
		{	// take _Other_ptr through _Other_rep from weak_ptr if not expired
			// otherwise, leave in default state if !_Throw,
			// otherwise throw exception
		if (_Other_rep && _Other_rep->_Incref_nz())
			_Reset0(_Other_ptr, _Other_rep);
		/*else if (_Throw)
			_THROW_NCEE(bad_weak_ptr, 0);*/
		}

	void _Reset0(_Ty *_Other_ptr, _Ref_count_base *_Other_rep)
		{	// release resource and take new resource
		if (_Rep != 0)
			_Rep->_Decref();
		_Rep = _Other_rep;
		_Ptr = _Other_ptr;
		}

	void _Decwref()
		{	// decrement weak reference count
		if (_Rep != 0)
			_Rep->_Decwref();
		}

	void _Resetw()
		{	// release weak reference to resource
		_Resetw((_Elem *)0, 0);
		}

	template<class _Ty2>
		void _Resetw(const _Ptr_base<_Ty2>& _Other)
		{	// release weak reference to resource and take _Other._Ptr
		_Resetw(_Other._Ptr, _Other._Rep);
		}

	template<class _Ty2>
		void _Resetw(const _Ty2 *_Other_ptr, _Ref_count_base *_Other_rep)
		{	// point to _Other_ptr through _Other_rep
		_Resetw(const_cast<_Ty2*>(_Other_ptr), _Other_rep);
		}

	template<class _Ty2>
		void _Resetw(_Ty2 *_Other_ptr, _Ref_count_base *_Other_rep)
		{	// point to _Other_ptr through _Other_rep
		if (_Other_rep)
			_Other_rep->_Incwref();
		if (_Rep != 0)
			_Rep->_Decwref();
		_Rep = _Other_rep;
		_Ptr = _Other_ptr;
		}

	operator bool() const
	{	// test if shared_ptr object owns no resource
	return (this->_Get() != 0 ? true : false);
	}

private:
	_Ty *_Ptr;
	_Ref_count_base *_Rep;
	template<class _Ty0>
		friend class _Ptr_base;
	};

		// TEMPLATE CLASS shared_ptr
template<class _Ty>
	class shared_ptr
		: public _Ptr_base<_Ty>
	{	// class for reference counted resource management
public:
	typedef shared_ptr<_Ty> _Myt;
	typedef _Ptr_base<_Ty> _Mybase;

	shared_ptr()
		{	// construct empty shared_ptr object
		}

	template<class _Ux>
		explicit shared_ptr(_Ux *_Px)
		{	// construct shared_ptr object that owns _Px
		_Resetp(_Px);
		}

	template<class _Ux,
		class _Dx>
		shared_ptr(_Ux *_Px, _Dx _Dt)
		{	// construct with _Px, deleter
		_Resetp(_Px, _Dt);
		}

//#if _HAS_CPP0X

	template<class _Ux,
		class _Dx,
		class _Alloc>
		shared_ptr(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
		{	// construct with _Px, deleter, allocator
		_Resetp(_Px, _Dt, _Ax);
		}
//#endif /* _HAS_CPP0X */

 #if _HAS_CPP0X
	template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Right, _Ty *_Px)
		{	// construct shared_ptr object that aliases _Right
		this->_Reset(_Px, _Right);
		}
 #endif /* _HAS_CPP0X */

	shared_ptr(const _Myt& _Other)
		{	// construct shared_ptr object that owns same resource as _Other
		this->_Reset(_Other);
		}

#ifndef NOT_USE_STL11
	//----in c++11 vector use moving constructor and moving assignment!----
	shared_ptr(_Myt&& _Other)
	{
		this->_Reset(_Other);
	}

	template<class _Ty2>
	_Myt& operator=(shared_ptr<_Ty2>&& _Other)
	{
		this->_Reset(_Other);
		return (*this);
	}
	//----------------------------------------------------------------------
#endif
        
	template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Other, const _Static_tag& _Tag)
		{	// construct shared_ptr object for static_pointer_cast
		this->_Reset(_Other, _Tag);
		}

	template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Other, const _Const_tag& _Tag)
		{	// construct shared_ptr object for const_pointer_cast
		this->_Reset(_Other, _Tag);
		}

	template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Other, const _Dynamic_tag& _Tag)
		{	// construct shared_ptr object for dynamic_pointer_cast
		this->_Reset(_Other, _Tag);
		}

	~shared_ptr()
		{	// release resource
		this->_Decref();
		}
        
    //strange... vector.erase demand copy-constructor with nonconst argument
    _Myt& operator=(_Myt& _Right)
    {
        this->_Reset(_Right);
        return *this;
    }

    _Myt& operator=(const _Myt& _Right)
    {
        this->_Reset(_Right);
        return *this;
    }

    //TODO: find original source and add method swap
	template<class _Ty2>
		_Myt& operator=(const shared_ptr<_Ty2>& _Right)
		{	// assign shared ownership of resource owned by _Right
		shared_ptr(_Right).swap(*this);
		return (*this);
		}

	void reset()
		{	// release resource and convert to empty shared_ptr object
		shared_ptr().swap(*this);
		}

	template<class _Ux>
		void reset(_Ux *_Px)
		{	// release, take ownership of _Px
		shared_ptr(_Px).swap(*this);
		}

	template<class _Ux,
		class _Dx>
		void reset(_Ux *_Px, _Dx _Dt)
		{	// release, take ownership of _Px, with deleter _Dt
		shared_ptr(_Px, _Dt).swap(*this);
		}

//#if _HAS_CPP0X
	template<class _Ux,
		class _Dx,
		class _Alloc>
		void reset(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
		{	// release, take ownership of _Px, with deleter _Dt, allocator _Ax
		shared_ptr(_Px, _Dt, _Ax).swap(*this);
		}
//#endif /* _HAS_CPP0X */

	_Ty *get() const
		{	// return pointer to resource
		return (this->_Get());
		}

	_Ty *operator->() const
		{	// return pointer to resource
		return (this->_Get());
		}

	bool unique() const
		{	// return true if no other shared_ptr object owns this resource
		return (this->use_count() == 1);
		}

private:
	template<class _Ux>
	void _Resetp(_Ux *_Px)
	{	// release, take ownership of _Px
	_Resetp0(_Px, new _Ref_count<_Ux>(_Px));
	}

        //!!!errors!!!
        
template<class _Ux,
	class _Dx>
	void _Resetp(_Ux *_Px, _Dx _Dt)
	{	// release, take ownership of _Px, deleter _Dt
	_Resetp0(_Px, new _Ref_count_del<_Ux, _Dx>(_Px, _Dt));
	}


//#if _HAS_CPP0X
	template<class _Ux,
		class _Dx,
		class _Alloc>
		void _Resetp(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
		{	// release, take ownership of _Px, deleter _Dt, allocator _Ax
		typedef _Ref_count_del_alloc<_Ux, _Dx, _Alloc> _Refd;
		typename _Alloc::template rebind<_Refd>::other _Al = _Ax;

		_Refd *_Ptr = _Al.allocate(1);
		new (_Ptr) _Refd(_Px, _Dt, _Al);
		_Resetp0(_Px, _Ptr);
		}
//#endif /* _HAS_CPP0X */

        //!!!errors!!!
        
public:
	template<class _Ux>
		void _Resetp0(_Ux *_Px, _Ref_count_base *_Rx)
		{	// release resource and take ownership of _Px
		this->_Reset0(_Px, _Rx);
		_Enable_shared(_Px, _Rx);
		}
	};

template<class _Ty1,
class _Ty2>
bool operator==(const shared_ptr<_Ty1>& _S1,
	const shared_ptr<_Ty2>& _S2)
{	// test if shared_ptr == shared_ptr
return (_S1.get() == _S2.get());
}

template<class _Ty1,
	class _Ty2>
	bool operator!=(const shared_ptr<_Ty1>& _S1,
		const shared_ptr<_Ty2>& _S2)
	{	// test if shared_ptr != shared_ptr
	return (!(_S1 == _S2));
	}
}

#endif

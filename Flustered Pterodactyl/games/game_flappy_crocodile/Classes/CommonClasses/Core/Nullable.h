#pragma once
#ifndef Nullable_h
#define Nullable_h

#include <new>

class NullableEmpty {};

class NullableBase
{
protected:
    // Default - invalid.
    NullableBase() : _isValid(false) {}
    NullableBase& operator=(const NullableBase& t)
    {
		_isValid = t._isValid;
		return *this;
    }

	//Copy constructor
    NullableBase(const NullableBase& other)
		: _isValid(other._isValid) {}

public:
	//utility functions
	bool const valid() const { return _isValid; }
	bool const invalid() const	{ return !_isValid; }

protected:
    bool _isValid;
};


template<class T>
class NullableBaseArray : public NullableBase
{
protected:
    NullableBaseArray() {}
    NullableBaseArray(const NullableBaseArray<T>& other)
        : NullableBase(other) {}
        
    T const * const GetT() const { return reinterpret_cast<T const * const>(_data); }
    T * const GetT() { return reinterpret_cast<T * const>(_data); }    
    void construct(T const & t) { new (GetT()) T(t); }
    void destroy() { GetT()->~T(); }
    
private:
    char _data[sizeof(T)];  // storage space for T
};


template<class T>
class NullableBaseValue : public NullableBase
{
protected:
    NullableBaseValue() {}
    NullableBaseValue(const NullableBaseValue<T>& other)
        : NullableBaseValue(other) {}
    
    T const * const GetT() const { return &_value; }
    T * const GetT() { return &_value; }
    void construct(T const & t) { _value = t; }
    void destroy() { _value = T(); }
    
private:
    T _value;
};


//because of EXC_ARM_DA_ALIGN on iPhone (http://www.cocos2d-iphone.org/forum/topic/271326)
#if defined(CC_TARGET_OS_IPHONE)
    #define NULLABLE_BASE_PARENT NullableBaseValue<T>
#else
    #define NULLABLE_BASE_PARENT NullableBaseArray<T>
#endif


template <class T>
class Nullable : public NULLABLE_BASE_PARENT
{
    typedef NULLABLE_BASE_PARENT super;
    
public:
    // Default - invalid.
    Nullable() {}
    Nullable(const T& t)  { super::construct(t); NullableBase::_isValid = (true);  }
	Nullable(const NullableEmpty&) {}

    Nullable& operator=(const T& t)
    {
        if (NullableBase::_isValid)
        {
            *super::GetT() = t;
        }
        else
        {
            super::construct(t);
			NullableBase::_isValid = true;	// order important for exception safety.
        }

        return *this;
    }

	//Copy constructor
    Nullable(const Nullable& other)
    {
		if (other._isValid)
		{
			super::construct(*other);
            NullableBase::_isValid = true;	// order important for exception safety.
		}
    }

    Nullable& operator=(const Nullable& other)
    {
		assert(!(this == &other));	// don't copy over self!
		if (NullableBase::_isValid)
		{						// first, have to destroy our original.
			NullableBase::_isValid = false;	// for exception safety if destroy() throws.
								// (big trouble if destroy() throws, though)
			super::destroy();
		}

		if (other._isValid)
		{
			super::construct(* other);
			NullableBase::_isValid = true;	// order vital.

		}
		return * this;
    }


	bool const operator == (Nullable const & other) const
	{
		if ((! valid()) && (! other.valid())) { return true; }
		if (valid() ^ other.valid()) { return false; }
		return ((* * this) == (* other));
	}

	bool const operator < (Nullable const & other) const
	{
		// equally invalid - not smaller.
		if ((! valid()) && (! other.valid()))   { return false; }

		// I'm not valid, other must be, smaller.
		if (! valid())	{ return true; }

		// I'm valid, other is not valid, I'm larger
		if (! other.valid()) { return false; }

		return ((* * this) < (* other));
	}

    ~Nullable() { if (NullableBase::_isValid) super::destroy(); }

	// Accessors.
	T& get() { return *super::GetT(); }

	T const & operator * () const			{ assert(NullableBase::_isValid); return * super::GetT(); }
	T & operator * ()						{ assert(NullableBase::_isValid); return * super::GetT(); }
	T const * const operator -> () const	{ assert(NullableBase::_isValid); return super::GetT(); }
	T		* const operator -> ()			{ assert(NullableBase::_isValid); return super::GetT(); }

	//This clears the value of this optional variable and makes it invalid once again.
	void clear()
	{
		if (NullableBase::_isValid)
		{
			NullableBase::_isValid = false;
			super::destroy();
		}
	}

	//utility functions
	bool const valid() const		{ return NullableBase::_isValid; }
	bool const invalid() const		{ return !NullableBase::_isValid; }

private:

};

#endif

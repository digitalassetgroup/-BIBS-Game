#pragma once
#ifndef QTypeTraits_h
#define QTypeTraits_h

//some stuff from <type_traits> (c++11)
namespace QTypeTraits
{
	//convenient template for integral constant types
	template<class T, T Value>
	struct integral_constant
	{
		static const T value = Value;

		typedef T value_type;
		typedef integral_constant<T, Value> type;
		
		operator value_type() const //return stored value
		{
			return value;
		}
	};

	typedef integral_constant<bool, true> true_type;
	typedef integral_constant<bool, false> false_type;

	//determine whether T is an lvalue reference
	template<class T> struct is_lvalue_reference : false_type {};
	template<class T> struct is_lvalue_reference<T&> : true_type {};


	template<class T>
	struct remove_reference
	{
		typedef T type;
	};

	template<class T>
	struct remove_reference<T&>
	{
		typedef T type;
	};

	template<class T>
	struct remove_reference<T&&>
	{
		typedef T type;
	};


	template<class T> inline
	typename remove_reference<T>::type&& move(T&& arg)
	{
		return ((typename remove_reference<T>::type&&)arg);
	}

	// forward an lvalue
	template<class T> inline
	T&& forward(typename remove_reference<T>::type& arg)
	{
		return static_cast<T&&>(arg);
	}
	
	//forward anything
	template<class T> inline
	T&& forward(typename remove_reference<T>::type&& arg)
	{	
		static_assert(!is_lvalue_reference<T>::value, "bad forward call");
		return static_cast<T&&>(arg);
	}
}

#endif
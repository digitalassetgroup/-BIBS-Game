#pragma once
#ifndef QFunction_h
#define QFunction_h

#include "Core/QScopedPointer.h"

//like std::function in c++11
//without variadic templates, because some compilers dose not support them yet
//from http://stackoverflow.com/questions/15175734/is-there-something-similar-to-stdfunction-before-c11

template<typename T>
struct QFunction {};

//TODO: need to add copy and move constructor!
template<typename R>
struct QFunction<R()>
{
public:
	QFunction() {}
	virtual ~QFunction() {}

    template<typename F>
	QFunction(const F& f)
		: _holder(new holder<F>(f)) {}

	//NOTE: heavy copy constructor - using heap!
	QFunction(const QFunction<R()>& other) {
		_holder.reset(other._holder->clone());
	}

	QFunction(QFunction<R()>&& other) {
		_holder.reset(other._holder.take());
	}

	template<typename F>
	void operator=(const F& f)
	{
		_holder.reset(new holder<F>(f));
	}

	//heavy copy assignment
	QFunction<R()>& operator=(const QFunction<R()>& other)
	{
		_holder.reset(other._holder->clone());
		return *this;
	}

	QFunction<R()>& operator=(QFunction<R()>&& other)
	{
		if (this != &other)
			_holder.reset(other._holder.take());

		return *this;
	}

	bool IsNull() const
	{ 
		return _holder.isNull();
	}

	void Clear()
	{
		_holder.reset(NULL);
	}

	R operator()() const
	{
		if (IsNull())
			return R();

		return _holder->call();
	}

private:
    struct holder_base
    {
		virtual R call() const = 0;
		virtual holder_base* clone() const = 0;
        virtual ~holder_base() {}
    };

    template<typename F>
    struct holder : holder_base
    {
        holder(const F& f) : _f(f) {}
        virtual R call() const { return _f(); }
		virtual holder_base* clone() const { return new holder<F>(_f); }
        const F _f;
    };

    Qt::QScopedPointer<holder_base> _holder;
};


template<typename R, typename Arg>
struct QFunction<R(Arg)>
{
private:
	struct holder_base
    {
		virtual R call(Arg arg) const = 0;
		virtual holder_base* clone() const = 0;
        virtual ~holder_base() {}
	};

    template<typename F>
    struct holder : holder_base
    {
        holder(const F& f) : _f(f) {}
        virtual R call(Arg arg) const { return _f(arg); }
		virtual holder_base* clone() const { return new holder<F>(_f); }
        const F _f;
    };

public:
	QFunction() {}
	virtual ~QFunction() {}

    template<typename F> 
    QFunction(const F& f) 
		: _holder(new holder<F>(f)) 
	{
	}

	//NOTE: heavy copy constructor - using heap!
	QFunction(const QFunction<R(Arg)>& other) {
		_holder.reset(other._holder->clone());
	}

	QFunction(QFunction<R(Arg)>&& other) {
		_holder.reset(other._holder.take());
	}

	template<typename F>
	void operator=(const F& f)
	{ 
		_holder.reset(new holder<F>(f)); 
	}

	//heavy copy assignment
	QFunction<R(Arg)>& operator=(const QFunction<R(Arg)>& other)
	{
		_holder.reset(other._holder->clone());
		return *this;
	}

	QFunction<R(Arg)>& operator=(QFunction<R(Arg)>&& other)
	{
		if (this != &other)
			_holder.reset(other._holder.take());

		return *this;
	}

	bool IsNull() const
	{
		return _holder.isNull();
	}

	void Clear()
	{
		_holder.reset(NULL);
	}

	template<typename Arg1>
    R operator()(Arg1 arg) const
	{
		if (IsNull())
			return R();

		return _holder->call(arg);
	}

private:
    Qt::QScopedPointer<holder_base> _holder;
};

/*
samples:

QFunction<int(int)> f;
f = [](int x) { return x + 3; };
int s = f(2);

QFunction<int()> f1;
f1 = []{ return 4; };
int s1 = f1();

*/

#endif

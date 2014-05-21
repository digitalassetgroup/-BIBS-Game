#pragma once
#ifndef QArray_h
#define QArray_h

template<typename T, int N>
class QArray
{
protected:
	T _arr[N];

public:
	QArray() : _arr() {}

	int GetCapacity() const
	{ 
		return N; 
	}	

	T& operator[](int i)
	{
		return _arr[i];
	}

	const T& operator[](int i) const
	{
		return _arr[i];
	}

	const T* begin() const
	{
		return _arr;
	}

	const T* end() const
	{
		return _arr + N;
	}

	void Clear(T defaultValue = T())
	{
		for (int i = 0; i < N; ++i)
			_arr[i] = defaultValue;
	}
};


template<typename T, int N>
class QStackVector : public QArray<T, N>
{
private:
	int _count;

public:
	QStackVector() 
		: QArray<T, N>(), _count()
	{
	}

	int size() const
	{
		return _count;
	}

	int GetCount() const
	{
		return _count;
	}

	const T* end() const
	{
		return this->_arr + _count;
	}

	void push_back(const T& element)
	{
		if (_count >= N)
			return; //assert

		this->_arr[_count] = element;
		++_count;
	}
};


template<typename T, int N1, int N2>
class QMatrix
{
private:
	T _arr[N1][N2];

private:
	void ValidateIndexes(int i, int j)
	{
		if (i < 0 || j < 0 || i >= N1 || j >= N2)
			assert(false);
	}

public:
	QMatrix()
	{
		Clear();
	}

	T& operator()(int i, int j)
	{
		ValidateIndexes(i, j);
		return _arr[i][j];
	}

	const T& operator()(int i, int j) const
	{
		ValidateIndexes(i, j);
		return _arr[i][j];
	}

	void Clear(T defaultValue = T())
	{
		for (int i = 0; i < N1; ++i) {
			for (int j = 0; j < N2; ++j) {
				_arr[i][j] = defaultValue;
			}
		}
	}
};

#endif

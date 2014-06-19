#pragma once
#ifndef CollectionHelper_h
#define CollectionHelper_h

#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include "CommonMacros.h"
using std::vector;
using std::list;

#ifdef min
	#undef min
#endif

#ifdef max
	#undef max
#endif


#pragma region functors

//пример:
//DEFINE_FUNCTOR(dictionary_deleter2, (dictionary* d), { iniparser_freedict(d); });
#define DEFINE_FUNCTOR(functorName, parameters, body) \
	struct functorName { void operator() parameters const body };

//пример:
//DEFINE_FUNCTOR_2((dictionary* d), { iniparser_freedict(d); }) dictionary_deleter3;
#define DEFINE_FUNCTOR_2(parameters, body) \
	typedef struct { void operator() parameters const body }

#define DEFINE_FUNCTOR_RETURN(returnType, parameters, body) \
	typedef struct { returnType operator() parameters const body }

#define DEFINE_ONELINE_FUNCTOR(parameters, body) \
	typedef struct { void operator()(parameters) const { return body; } }

#define UNIQUE_NAME_INTERNAL(name, line) name##line
#define UNIQUE_NAME_BY_LINE(name, line) UNIQUE_NAME_INTERNAL(name, line)
#define UNIQUE_NAME_(name) UNIQUE_NAME_BY_LINE(name, __LINE__)

#pragma endregion

#define for_each_iterator(collection) \
	for (auto it = collection.begin(), itEnd = collection.end(); it != itEnd; ++it)

#define for_each_ref(elementName, collection) \
	for (auto UNIQUE_NAME_(it) = collection.begin(), UNIQUE_NAME_(itEnd) = collection.end(); UNIQUE_NAME_(it) != UNIQUE_NAME_(itEnd); ++UNIQUE_NAME_(it)) \
	{ elementName = *UNIQUE_NAME_(it);

#define for_each_ref_end }

//TODO: rename for_each because of name confliction with STL!
#define for_each(elementName, collection) \
	for (auto UNIQUE_NAME_(it) = collection.begin(), UNIQUE_NAME_(itEnd) = collection.end(); \
		elementName = (UNIQUE_NAME_(it) != UNIQUE_NAME_(itEnd)) ? *UNIQUE_NAME_(it) : NULL; ++UNIQUE_NAME_(it))

#define for_each_(elementName, collection) \
	for (auto UNIQUE_NAME_(it) = collection->begin(), UNIQUE_NAME_(itEnd) = collection->end(); \
		elementName = (UNIQUE_NAME_(it) != UNIQUE_NAME_(itEnd)) ? *UNIQUE_NAME_(it) : NULL; ++UNIQUE_NAME_(it))

namespace CollectionHelper
{
	template<typename T, int N>
	int GetArraySize(T (&arr)[N]) { return N; }

	template<typename T>
	void AddRange(T& sourceCollection, const T& newElements)
	{
		for_each_iterator(newElements)
			sourceCollection.push_back(*it);
	}

	template<typename TCollection, typename TElem>
	bool Contains(const TCollection& collection, const TElem& elementToFind)
	{
		for_each_iterator(collection)
		{
			if (*it == elementToFind)
				return true;
		}

		return false;
	}

	template<typename T>
	void Remove(vector<T>& collection, const T& value)
	{
		auto newIteratorEnd = std::remove(collection.begin(), collection.end(), value);
		if (newIteratorEnd != collection.end())
			collection.erase(newIteratorEnd, collection.end());
	}

	template<typename T, typename TPredicate>
	void RemoveIf(vector<T>& collection, const TPredicate& predicate)
	{
		auto newIteratorEnd = std::remove_if(collection.begin(), collection.end(), predicate);
		if (newIteratorEnd != collection.end())
			collection.erase(newIteratorEnd, collection.end());
	}

	template<typename T>
	void ClearArrayWithCount(T* array, int count, const T& defaultValue = T())
	{
		for (int i = 0; i < count; ++i)
			array[i] = defaultValue;
	}

	template<typename T, int N>
	inline void ClearArray(T (&array)[N], const T& defaultValue = T())
	{
		ClearArrayWithCount(array, N, defaultValue);
	}

	template<typename T, int ROW_COUNT, int COLUMN_COUNT>
	void CleanMatrixTwoDimension(T (&matrix)[ROW_COUNT][COLUMN_COUNT], T defaultValue = T())
	{
		for (int i = 0; i < ROW_COUNT; ++i) {
			for (int j = 0; j < COLUMN_COUNT; ++j) {
				matrix[i][j] = defaultValue;
			}
		}
	}

	template<typename T, int N>
	bool IsArrayEmpty(T (&array)[N], const T& defaultValue = T())
	{
		for (int i = 0; i < N; ++i)
			if (array[i] != defaultValue)
				return false;
		return true;
	}

	template<class TIt, typename TFunc>
	void IterForEach(TIt first, TIt last, TFunc func)
	{
		for (auto it = first; it != last; ++it)
		{
			func(*it);
		}
	}

	template<typename T>
	int GetIndexOfElement(const std::vector<T>& vec, const T& elem)
	{
		int index = distance(vec.begin(), find(vec.begin(), vec.end(), elem));
		if (index >= (int)vec.size())
			return -1;
		return index;
	}

    //like in c++11 std
    template<typename TIter, typename TPredicate>
    bool IsAllOf(TIter first, TIter last, TPredicate pred)
    {
        for (auto it = first; it != last; ++it)
        {
            if (!pred(*it))
                return false;
        }
        
        return true;
    }

	template<typename TIter, typename TPredicate> 
	bool IsAnyOf(TIter itBeg, TIter itEnd, TPredicate pred)
	{
		for (auto it = itBeg; it != itEnd; ++it)
		{
			if (pred(*it))
				return true;
		}

		return false;
	}

	template<typename T, typename TPred> 
	T* GetOneByPredicate(const vector<T*> source, const TPred& predicate)
	{
		for (auto it = source.begin(), itEnd = source.end(); it != itEnd; ++it)
		{
			if (predicate(*it))
				return *it;
		}

		return nullptr;
	}

	template<typename TScalar, typename TCollection, typename TFunc>
	TScalar MaxValue(const TCollection& source, const TFunc& func)
	{
		TScalar maxValue = std::numeric_limits<TScalar>::min();
		for (const auto& element : source)
		{
			TScalar value = func(element);
			if (value > maxValue)
				maxValue = value;
		}

		return maxValue;
	}

	template<typename TScalar, typename TCollection, typename TFunc>
	TScalar MinValue(const TCollection& source, const TFunc& func)
	{
		TScalar minValue = std::numeric_limits<TScalar>::max();
		for (const auto& element : source)
		{
			TScalar value = func(element);
			if (value < minValue)
				minValue = value;
		}

		return minValue;
	}

	//sample: std::map mymap = CreateMap<int, int>(1,2)(3,4)(5,6);
	template <typename T, typename U> 
	class CreateMap
	{ 
		private_ std::map<T, U> _map;

		public_ CreateMap(const T& key, const U& val)
		{
			_map[key] = val;
		}
			
		public_ CreateMap<T, U>& operator()(const T& key, const U& val)
		{
			_map[key] = val;
			return *this; 
		} 
			
		public_ operator std::map<T, U>() { return _map; }
	};

	//sample: MapAddValues<int,int>(my_map)(1,2)(3,4)(5,6);
	template<typename T, typename U> 
	class MapAddValues 
	{ 
		private_ std::map<T,U>& _map; 
		public_	MapAddValues(std::map<T, U>& _map)
			: _map(_map) {}
		
		public_ MapAddValues& operator()(const T& key, const U& val)
		{
			_map[key] = val;
			return *this;
		}
	};

	//
	template<typename T> 
	class CreateVectorClass
	{ 
		private_ std::vector<T> _vec;

		public_ CreateVectorClass(const T& value) {
			_vec.push_back(value);
		}
			
		public_ CreateVectorClass<T>& operator()(const T& value) {
			_vec.push_back(value);
			return *this;
		} 
			
		public_ std::vector<T> toVec() { return _vec; }
		public_ operator std::vector<T>() { return toVec(); }
	};

	//CreateVector(1)(2)(3)
	template<typename T>
	CreateVectorClass<T> CreateVector(const T& value) {
		return CreateVectorClass<T>(value); //hope on RVO 
	}


	//example: MAKE_VECTOR(1, 2, 3)
	#define MAKE_VECTOR(first,...) \
		([&]() -> std::vector<decltype(first)> { \
			static const decltype(first) arr[] = { first,__VA_ARGS__ }; \
			std::vector<decltype(first)> ret(arr, arr + sizeof(arr) / sizeof(*arr)); \
			return ret; \
		})()
}


template<typename T, typename TElement>
vector<T>& operator<<(vector<T>& source, const TElement& element)
{
	source.push_back(element);
	return source;
}

template<typename T, typename TElement>
vector<T*>& operator<<(vector<T*>& source, TElement* element)
{
	source.push_back(element);
	return source;
}


template<typename T1>
struct Tuple
{
	T1 Item1;

	Tuple(T1 item1) : Item1(item1) { }
};

#endif

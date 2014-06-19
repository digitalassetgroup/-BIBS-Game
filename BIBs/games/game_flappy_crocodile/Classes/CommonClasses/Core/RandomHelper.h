#pragma once
#ifndef RandomHelper_h
#define RandomHelper_h

#include "Core/CoreAll.h"

namespace RandomHelper
{
	template<typename T>
	static T RandomFromVector(const vector<T>& vec)
	{		
		int i = Math::RandomRange(0, vec.size() - 1);
		return vec[i];
	}

	//Генерация случайного индекса массива arr
	//только среди НЕнулевых элементов массива arr
	//excludeElement - как бы нулевой элемент
	//Пример: [1,6,0,0,6] и excludeElement = 0 - здесь будет возвращаться 0,1 или 4 (т.к. в массиве на этих индексах содержатся непустые элементы)
	template<typename T>
	static int GetRandomIndexFromArray(T* array, int count, T excludeElement = T())
	{
		vector<int> vec;
		for (int i = 0; i < count; ++i)
		{
			if (array[i] != excludeElement)
				vec.push_back(i);
		}

		return vec.empty() ? -1 : RandomFromVector(vec);
	}

	//beginIndex <= index <= endIndex
	template<typename T>
	static int GetRandomIndexFromArray(T* array, int beginIndex, int endIndex, T excludeElement = T())
	{
		vector<int> vec;
		for (int i = beginIndex; i <= endIndex; ++i)
		{
			if (array[i] != excludeElement)
				vec.push_back(i);
		}

		return vec.empty() ? -1 : RandomFromVector(vec);
	}
}

#endif

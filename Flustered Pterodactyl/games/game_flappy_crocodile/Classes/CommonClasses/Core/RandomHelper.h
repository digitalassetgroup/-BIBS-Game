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

	//��������� ���������� ������� ������� arr
	//������ ����� ��������� ��������� ������� arr
	//excludeElement - ��� �� ������� �������
	//������: [1,6,0,0,6] � excludeElement = 0 - ����� ����� ������������ 0,1 ��� 4 (�.�. � ������� �� ���� �������� ���������� �������� ��������)
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

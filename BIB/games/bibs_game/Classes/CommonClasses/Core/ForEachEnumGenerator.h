#pragma once
#ifndef ForEachEnumGenerator_h
#define ForEachEnumGenerator_h

//for c++11 foreach loop - loop over enum value, from minValue to maxValue - for(const auto a : A::All)
//GENERATE_ITERATORS_METHOD_FOR_ENUM(A, A::Min, A::Max)
#define GENERATE_ITERATOR_METHODS_FOR_ENUM(EnumType, minValue, maxValue) \
	inline EnumType& operator++(EnumType& element) \
	{ \
		element = (EnumType)((int)element + 1); \
		return element; \
	} \
	inline EnumType operator*(EnumType element) { return element; } \
	inline EnumType begin(EnumType) { return minValue; } \
	inline EnumType end(EnumType) { return (EnumType)((int)maxValue + 1); } 


//example - for (auto upgradeType : ShopUpgradeType::All()) where ShopUpgradeType - namespace with inner enum
#define GENERATE_ITERATOR_METHODS_FOR_ENUM_IN_NAMESPACE(EnumType, minValue, maxValue) \
	GENERATE_ITERATOR_METHODS_FOR_ENUM(EnumType, minValue, maxValue) \
	inline EnumType All() { return EnumType(); } /* get enumerator - for convenience: return value not using, just type */

#endif

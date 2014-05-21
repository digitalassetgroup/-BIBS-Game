#pragma once
#ifndef IdentityObject_h
#define IdentityObject_h

#include "NonCopyable.h"
#include "StringHelper.h"

//объект с id-шкой
class IdentityObject : NonCopyable
{
private:
	static int _totalCount;

public:
	int Id;
	std::string ToString() { return MAKE_STRING("Id = " << Id); }

protected:
	IdentityObject() : Id(++_totalCount) {}
};

#endif

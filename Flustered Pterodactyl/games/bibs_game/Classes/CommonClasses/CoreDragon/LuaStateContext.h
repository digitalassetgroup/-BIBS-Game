#pragma once
#ifndef LuaStateContext_h
#define LuaStateContext_h

#include "Core/NonCopyable.h"
#include <string>
#include <vector>
#include "LuaPlusHelper.h"
using std::string;
using std::vector;
namespace LuaPlus { class LuaState; class LuaObject; }


class LuaStateContext : NonCopyable
{
private:
	//для SaveContext
	string _source;
	bool _isInObject;
	string _currentObjectName;
	//для LoadContext
	LuaPlus::LuaState* _luaState;
	mutable LuaPlus::LuaObject* _currentLuaObject;

public:
	LuaStateContext();
	~LuaStateContext();
	
	//для SaveContext
	string& GetSource() { return _source; }
	void AddPropertyBool(const string& propName, bool value);
	void AddPropertyInt(const string& propName, int value);
	void AddPropertyIntWithIndex(const string& indexName, int value);
	void AddPropertyArrayInt(const string& propName, const vector<int>& value);
	void StartObject(const string& objectName);
	void EndObject();

	//для LoadContext
	bool LoadLuaStateFromCode(const string& code, bool isIncludeLua);
	bool LoadLuaStateFromFile(const string& path, bool isIncludeLua);
	bool GetPropertyBool(const string& propName, bool defaultValue) const;
	int GetPropertyInt(const string& propName, int defaultValue = 0) const;
	int GetPropertyIntWithIndex(int index, int defaultValue = 0) const;
	void GetPropertyArrayInt(const string& propName, vector<int>& result);
	bool GetObjectStart(const string& objectName) const;
	void GetObjectEnd() const;

	//templates
	template<typename TNumber>
	void AddPropertyArrayNumber(const string& propName, const vector<TNumber>& value)
	{
		_source += StringHelper::format("%s = { ", propName.c_str());
		for (uint i = 0; i < value.size(); ++i)
		{
			_source += MAKE_STRING(value[i] << ", ");
		}
		_source += "}";
		_source += (_isInObject) ? ", " : "\n";
	}

	template<typename TNumber>
	void GetPropertyArrayNumber(const string& propName, vector<TNumber>& result)
	{
		const auto& objParent = (_currentLuaObject != NULL) ? *_currentLuaObject : _luaState->GetGlobals();
		LuaPlusHelper::GetLuaObjectArray(objParent, propName.c_str(), result);
	}	
};

#endif
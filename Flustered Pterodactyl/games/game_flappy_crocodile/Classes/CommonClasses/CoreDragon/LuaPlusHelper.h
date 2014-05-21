#pragma once
#ifndef LuaPlusHelper_h
#define LuaPlusHelper_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
#include "lib/LuaPlus/LuaPlus.h"

#define LUA_STATE_ERROR(state, ...) \
	luaL_error(LuaState_to_lua_State(state), __VA_ARGS__);	


class LuaObjectWrapper
{
	private_ const LuaPlus::LuaObject _luaObject;

	public_ LuaObjectWrapper(const LuaPlus::LuaObject& luaObject);
	public_ const LuaPlus::LuaObject& GetRealObject() const;
	LuaObjectWrapper GetByIndex(int index) const;
	LuaObjectWrapper GetByName(const char* name) const;
	LuaObjectWrapper Get(const char* name) const;
	LuaObjectWrapper operator[](const char* name) const;
	LuaObjectWrapper operator[](int index) const;
	bool IsNil() const;
	size_t GetCount() const;

	public_ float F(const char* fieldName, float defaultValue) const;
	public_ int I(const char* fieldName, int defaultValue) const;
	public_ bool B(const char* fieldName, bool defaultValue) const;
	public_ void S(const char* fieldName, string& result) const;
	public_ string S(const char* fieldName) const;
};


//при выходе за границы LuaStackObject Popится
class LuaStackObjectWrapper : public Qt::QPointerBase<LuaPlus::LuaStackObject>
{
	private_ LuaPlus::LuaStackObject _realStackObject;

	public_ LuaStackObjectWrapper(const LuaPlus::LuaStackObject& p);
	public_ ~LuaStackObjectWrapper();
	public_ LuaStackObjectWrapper GetByIndex(int index) const;
	public_ LuaStackObjectWrapper GetByName(const char* name) const;
	public_ LuaStackObjectWrapper operator[](const char* name) const;
	public_ LuaStackObjectWrapper operator[](int index) const;
};


namespace LuaPlusHelper
{
	void LuaLogError(const char* message);
	bool ExecuteAsFile(LuaPlus::LuaState* state, const string& fileName);
	bool ExecuteAsStringFromFile(LuaPlus::LuaState* state, const string& fileName);
	bool CheckFieldsIfExisting(const LuaStackObjectWrapper& table, const vector<string>& fieldNames);
	void CheckFieldOnType(const LuaStackObjectWrapper& table, const char* fieldName, int type);		
	void CheckFieldOnTypeObject(const LuaPlus::LuaObject& table, const char* fieldName, int type);
	void CheckIntOnPositive(const LuaStackObjectWrapper& table, const char* fieldName);
	void CheckFieldOnTypeIndex(const LuaStackObjectWrapper& table, int index, int type);

	float GetLuaObjectFloat(const LuaPlus::LuaObject& table, const char* fieldName, float defaultValue);
	int GetLuaObjectInt(const LuaPlus::LuaObject& table, const char* fieldName, int defaultValue);
	void LuaObjectGetFloat(const LuaPlus::LuaObject& table, const char* fieldName, float& var);
	bool GetLuaObjectBool(const LuaPlus::LuaObject& table, const char* fieldName, bool defaultValue);
	void GetLuaObjectString(const LuaPlus::LuaObject& table, const char* fieldName, string& result);
	string GetLuaObjectStringReturn(const LuaPlus::LuaObject& table, const char* fieldName);


	template<typename TNumber> 
	struct LuaObjectNumberConverter
	{
		static TNumber Get(const LuaPlus::LuaObject& objChild) { return (TNumber)objChild.GetInteger(); }
	};

	template<>
	struct LuaObjectNumberConverter<bool>
	{
		static bool Get(const LuaPlus::LuaObject& objChild) { return objChild.GetInteger() > 0; }
	};

	template<>
	struct LuaObjectNumberConverter<float>
	{
		static float Get(const LuaPlus::LuaObject& objChild) { return objChild.GetFloat(); }
	};

	template<>
	struct LuaObjectNumberConverter<double> 
	{
		static double Get(const LuaPlus::LuaObject& objChild) { return objChild.GetDouble(); }
	};

	template<>
	struct LuaObjectNumberConverter<string> 
	{
		static string Get(const LuaPlus::LuaObject& objChild) { return objChild.GetString(); }
	};


	template<typename TNumber>
	void GetLuaObjectArray(const LuaPlus::LuaObject& table, const char* fieldName, vector<TNumber>& result)
	{
		auto obj = table[fieldName];
		if (obj.IsNil())
			return;

		int count = obj.GetCount();
		for (int i = 0; i < count; ++i)
		{
			auto objChild = obj.GetByIndex(i + 1);
			if (!objChild.IsNil())
			{
				TNumber value = LuaObjectNumberConverter<TNumber>::Get(objChild);
				result.push_back(value);
			}
		}
	}
}

#endif

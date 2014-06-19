#include "LuaPlusHelper.h"
#include "CorePlatform/LogHelper.h"
using namespace LuaPlus;
using namespace cocos2d; //NOTE: нужен дл€ андроид-билда дл€ MY_LOG_ERROR -> CCMessageBox


//--------------LuaObjectWrapper---------------------

LuaObjectWrapper::LuaObjectWrapper(const LuaPlus::LuaObject& luaObject)
	: _luaObject(luaObject)
{
}

const LuaPlus::LuaObject& LuaObjectWrapper::GetRealObject() const
{
	return _luaObject;
}

LuaObjectWrapper LuaObjectWrapper::GetByIndex(int index) const { return _luaObject.GetByIndex(index); }
LuaObjectWrapper LuaObjectWrapper::GetByName(const char* name) const { return _luaObject.GetByName(name); }
LuaObjectWrapper LuaObjectWrapper::Get(const char* name) const { return _luaObject.Get(name); }
LuaObjectWrapper LuaObjectWrapper::operator[](const char* name) const { return _luaObject[name]; }
LuaObjectWrapper LuaObjectWrapper::operator[](int index) const { return _luaObject[index]; }
bool LuaObjectWrapper::IsNil() const { return _luaObject.IsNil(); }
size_t LuaObjectWrapper::GetCount() const { return _luaObject.GetCount(); }

float LuaObjectWrapper::F(const char* fieldName, float defaultValue) const
{
	return LuaPlusHelper::GetLuaObjectFloat(_luaObject, fieldName, defaultValue);
}

int LuaObjectWrapper::I(const char* fieldName, int defaultValue) const
{
	return LuaPlusHelper::GetLuaObjectInt(_luaObject, fieldName, defaultValue);
}

bool LuaObjectWrapper::B(const char* fieldName, bool defaultValue) const
{
	return LuaPlusHelper::GetLuaObjectBool(_luaObject, fieldName, defaultValue);
}

void LuaObjectWrapper::S(const char* fieldName, string& result) const
{
	LuaPlusHelper::GetLuaObjectString(_luaObject, fieldName, result);
}

string LuaObjectWrapper::S(const char* fieldName) const
{
	return LuaPlusHelper::GetLuaObjectStringReturn(_luaObject, fieldName);
}

//-----------

//---------------LuaStackObjectWrapper-----------------

LuaStackObjectWrapper::LuaStackObjectWrapper(const LuaPlus::LuaStackObject& p)
	: _realStackObject(p), QPointerBase<LuaPlus::LuaStackObject>(&_realStackObject) 
{
}

LuaStackObjectWrapper::~LuaStackObjectWrapper()
{
	_data->Pop();
}

LuaStackObjectWrapper LuaStackObjectWrapper::GetByIndex(int index) const
{
	return LuaStackObjectWrapper(_data->GetByIndex(index)); 
}

LuaStackObjectWrapper LuaStackObjectWrapper::GetByName(const char* name) const
{
	return LuaStackObjectWrapper(_data->GetByName(name));
}

LuaStackObjectWrapper LuaStackObjectWrapper::operator[](const char* name) const
{
	return GetByName(name);
}

LuaStackObjectWrapper LuaStackObjectWrapper::operator[](int index) const
{
	return GetByIndex(index);
}

//-------------------------------------------------------------------


namespace LuaPlusHelper
{
	void LuaLogError(const char* message)
	{
		MY_LOG(message);
	}

	bool ExecuteAsFile(LuaState* state, const string& fileName)
	{
		int error = state->DoFile(fileName.c_str());
		if (error != 0)
		{
			LuaLogError(state->ToString(-1));
			return false;
		}
		//state->DoString("io.write (\"Hello\" .. _VERSION .. \"\\n\")   ");

		return true;
	}

	//NOTE: version especcialy for android, where all resource files in zip
	bool ExecuteAsStringFromFile(LuaState* state, const string& fileName)
	{
		//если не андроид, то вызываем ExecuteAsFile
		#if CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
			return ExecuteAsFile(state, fileName);
		#endif
    
		CCFileData fileData(fileName.c_str(), "rb", false);
		if (fileData.getSize() == 0)
		{
			MY_LOG_ERROR(StringHelper::format("Can't find '%s'", fileName.c_str()).c_str());
			return false;
		}

		auto buffer = (char*)fileData.getBuffer();
		string stBuffer(buffer, buffer + (int)fileData.getSize()); //копирование строки :'(

		int error = state->DoString(stBuffer.c_str());
		if (error != 0)
		{
			LuaLogError(state->ToString(-1));
			return false;
		}
		return true;
	}

	//TODO: нужно сделать также проверку на типы полей. то есть нужно где-то описывать структуру объектов, а-л€ маппинг
	//TODO: может быть сделать маппинг, как в sqlite?
	//ѕроверка на наличие полей
	bool CheckFieldsIfExisting(const LuaStackObjectWrapper& table, const vector<string>& fieldNames)
	{
		for_each_ref(auto fieldName, fieldNames)
		{
			auto field = table.GetByName(fieldName.c_str());
			if (field->IsNil() || field->IsNone())
			{
				LUA_STATE_ERROR(table->GetState(), "No input parameter '%s'!", fieldName.c_str());
				return false;
			}
		}
		for_each_ref_end

		return true;
	}

	void CheckFieldOnType(const LuaStackObjectWrapper& table, const char* fieldName, int type)
	{
		auto field = table.GetByName(fieldName);
		if (field->IsNil())
			return;
	
		if (field->GetType() != type)
		{
			LUA_STATE_ERROR(table->GetState(), "'%s' has wrong value!", fieldName);
		}
	}

	void CheckFieldOnTypeIndex(const LuaStackObjectWrapper& table, int index, int type)
	{
		auto field = table[index];
		if (field->IsNil())
			return;
	
		if (field->GetType() != type)
		{
			LUA_STATE_ERROR(table->GetState(), "field number '%d' has wrong value!", index);
		}
	}

	void CheckFieldOnTypeObject(const LuaObject& table, const char* fieldName, int type)
	{
		auto field = table.GetByName(fieldName);
		if (field.IsNil())
			return;
	
		if (field.Type() != type)
		{
			LUA_STATE_ERROR(table.GetState(), "'%s' has wrong value!", fieldName);
		}
	}

	//TODO: по идее, нужно создать специальную структуру дл€ валидации, где на одно поле может быть несколько валидаторов
	void CheckIntOnPositive(const LuaStackObjectWrapper& table, const char* fieldName)
	{
		auto field = table.GetByName(fieldName);
		if (field->IsNil() || field->GetType() != LUA_TNUMBER)
			return;

		int value = field->GetInteger();
		if (value <= 0)
		{
			LUA_STATE_ERROR(table->GetState(), "'%s' has zero or negative value = %d!", fieldName, value);
		}
	}


	//-----

	void GetLuaObjectString(const LuaPlus::LuaObject& table, const char* fieldName, string& result)
	{
		auto obj = table[fieldName];
		if (obj.IsNil())
			return;

		if (obj.Type() != LUA_TSTRING)
		{
			MY_LOG(fieldName << " has wrong type!");
			return;
		}

		result = obj.GetString();
	}

	string GetLuaObjectStringReturn(const LuaPlus::LuaObject& table, const char* fieldName)
	{
		string result;
		GetLuaObjectString(table, fieldName, result);
		return result;
	}

	float GetLuaObjectFloat(const LuaPlus::LuaObject& table, const char* fieldName, float defaultValue)
	{
		auto obj = table[fieldName];
		if (obj.IsNil())
			return defaultValue;

		if (obj.Type() != LUA_TNUMBER)
		{
			MY_LOG(fieldName << " has wrong type!");
			return defaultValue;
		}

		float result = obj.GetFloat();
		return result;
	}

	bool GetLuaObjectBool(const LuaPlus::LuaObject& table, const char* fieldName, bool defaultValue)
	{
		auto obj = table[fieldName];
		if (obj.IsNil())
			return defaultValue;

		if (obj.Type() != LUA_TBOOLEAN)
		{
			MY_LOG(fieldName << " has wrong type!");
			return defaultValue;
		}

		return obj.GetBoolean();
	}

	int GetLuaObjectInt(const LuaPlus::LuaObject& table, const char* fieldName, int defaultValue)
	{
		auto obj = table[fieldName];
		if (obj.IsNil())
			return defaultValue;

		if (obj.Type() != LUA_TNUMBER)
		{
			MY_LOG(fieldName << " has wrong type!");
			return defaultValue;
		}

		return obj.GetInteger();
	}

	void LuaObjectGetFloat(const LuaPlus::LuaObject& table, const char* fieldName, float& var)
	{
		var = GetLuaObjectFloat(table, fieldName, var);
	}
}
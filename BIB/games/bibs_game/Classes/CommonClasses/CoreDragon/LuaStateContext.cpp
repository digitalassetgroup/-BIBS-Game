#include "LuaStateContext.h"
#include "Core/StringHelper.h"
#include "CorePlatform/LogHelper.h"
#include "lib/LuaPlus/LuaPlus.h"
using namespace LuaPlus;


LuaStateContext::LuaStateContext()
	: _isInObject(), _luaState(), _currentLuaObject()
{
}

LuaStateContext::~LuaStateContext()
{
	SAFE_DELETE(_currentLuaObject);

	if (_luaState != NULL)
	{
		LuaState::Destroy(_luaState);
		_luaState = NULL;
	}
}

void LuaStateContext::AddPropertyBool(const string& propName, bool value)
{
	AddPropertyInt(propName, value ? 1 : 0);
}

void LuaStateContext::AddPropertyInt(const string& propName, int value)
{
	_source += StringHelper::format("%s = %s", propName.c_str(), StringHelper::ToString(value).c_str());
	_source += (_isInObject) ? ", " : "\n";
}

void LuaStateContext::AddPropertyIntWithIndex(const string& indexName, int value)
{
	MY_ASSERT(_isInObject, "");
	_source += StringHelper::format("\n\t[%s] = %s, ", indexName.c_str(), StringHelper::ToString(value).c_str());
}

void LuaStateContext::AddPropertyArrayInt(const string& propName, const vector<int>& value)
{
	_source += StringHelper::format("%s = { ", propName.c_str());
	for (uint i = 0; i < value.size(); ++i)
	{
		_source += MAKE_STRING(value[i] << ", ");
	}
	_source += "}";
	_source += (_isInObject) ? ", " : "\n";
}

void LuaStateContext::StartObject(const string& objectName)
{
	_currentObjectName = objectName;
	_source += objectName + " = \n{ ";
	_isInObject = true;
}

void LuaStateContext::EndObject()
{
	_source += "\n}\n";
	_isInObject = false;
}

//-----------
bool LuaStateContext::LoadLuaStateFromFile(const string& path, bool isIncludeLua)
{
	LuaState::SetErrorHandler(&LuaPlusHelper::LuaLogError);
	_luaState = LuaState::Create();

	if (isIncludeLua)
	{
		LuaPlusHelper::ExecuteAsStringFromFile(_luaState, CCFileUtils_SHARED::fullPathForFilename("lua/include.lua"));
	}

	int error = _luaState->DoFile(path.c_str());
	if (error != 0)
	{
		LuaPlusHelper::LuaLogError(_luaState->ToString(-1));
		return false;
	}

	return true;
}

bool LuaStateContext::LoadLuaStateFromCode(const string& code, bool isIncludeLua)
{
	LuaState::SetErrorHandler(&LuaPlusHelper::LuaLogError);
	_luaState = LuaState::Create();

    if (isIncludeLua)
	{
		LuaPlusHelper::ExecuteAsStringFromFile(_luaState, CCFileUtils_SHARED::fullPathForFilename("lua/include.lua"));
	}
    
	int error = _luaState->DoString(code.c_str());
	if (error != 0)
	{
		LuaPlusHelper::LuaLogError(_luaState->ToString(-1));
		return false;
	}

	return true;
}

bool LuaStateContext::GetPropertyBool(const string& propName, bool defaultValue) const
{
	return GetPropertyInt(propName, defaultValue ? 1 : 0) == 1;
}

int LuaStateContext::GetPropertyInt(const string& propName, int defaultValue) const
{
	const auto& objParent = (_currentLuaObject != NULL) ? *_currentLuaObject : _luaState->GetGlobals();

	int result = LuaPlusHelper::GetLuaObjectInt(objParent, propName.c_str(), defaultValue);
	return result;
}

int LuaStateContext::GetPropertyIntWithIndex(int index, int defaultValue) const
{
	MY_ASSERT(_currentLuaObject != NULL, "");
	const auto& objParent = *_currentLuaObject;

	//TODO: вынести в helper-метод GetByIndex
	auto obj = objParent.GetByIndex(index);
	if (obj.IsNil())
		return defaultValue;

	int result = obj.GetInteger();
	return result;
}

void LuaStateContext::GetPropertyArrayInt(const string& propName, vector<int>& result)
{
	const auto& objParent = (_currentLuaObject != NULL) ? *_currentLuaObject : _luaState->GetGlobals();
	LuaPlusHelper::GetLuaObjectArray(objParent, propName.c_str(), result);
}

bool LuaStateContext::GetObjectStart(const string& objectName) const
{
	auto obj = _luaState->GetGlobals().GetByName(objectName.c_str());
	if (obj.IsNil())
		return false;

	_currentLuaObject = new LuaObject(obj);
	return true;
}

void LuaStateContext::GetObjectEnd() const
{
	SAFE_DELETE(_currentLuaObject);
}

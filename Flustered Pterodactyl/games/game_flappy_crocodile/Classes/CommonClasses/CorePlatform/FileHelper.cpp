#include "FileHelper.h"

#include <fstream>
#include <iostream>
#include "CorePlatform/CorePlatformAll.h"
#include "LogHelper.h"
#include "Core/StringHelper.h"
#include <algorithm>
#include <platform/CCFileUtils.h>
#include "support/ccUTF8.h"

using namespace cocos2d;

#if CC_TARGET_PLATFORM != CC_PLATFORM_WP8
	#include <dirent.h>

	static void GetFilesInDevicePath(const string& path, vector<string>& result)
	{
		DIR* dir = opendir(path.c_str());
		if (dir == NULL)
		{
			MY_LOG(MAKE_STRING("Can't find folder " << path.c_str()));
			return;
		}

		for (dirent* ent = readdir(dir); ent != NULL; ent = readdir(dir))
		{
			if (ent->d_type == DT_REG)
				result.push_back(ent->d_name);
		}

		closedir(dir);
	}
#else
	static void GetFilesInDevicePath(const string& path, vector<string>& result)
	{
	}
#endif

//вернуть список файлов в каталоге path, который находится в Resources
void FileHelper::GetFilesInResourcePath(const string& path, vector<string>& result)
{
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		//TODO: need to implement from SteamPanic code!

		string androidPathToAssets = "assets/assets/" + path + "/";
		string resourcePath = CCFileUtils::sharedFileUtils()->GetResourcePath();
		MY_LOG(MAKE_STRING("apk-path = " << resourcePath.c_str()));
		//resourcePath = "d:/projects/cocos2d-x/games/snowdragon/proj.android/bin/CatsCloneAndroid-debug.apk"; //for debug

		CCFileUtils::GetFileNamesInResourcePathFromZip(resourcePath, androidPathToAssets, result);

		for_each_iterator(result)
		{
			string& s = *it;
			StringHelper::Replace(s, androidPathToAssets, "");
		}
	}
	#else
	{
		string absolutePath = CCFileUtils_SHARED::fullPathForFilename(path.c_str());
		GetFilesInDevicePath(absolutePath, result);
	}
	#endif

	string debugFiles;
	StringHelper::Join(debugFiles, result, ";");
	MY_LOG(MAKE_STRING("finded_files: " << debugFiles));
}

//вернуть все левелы из папки Resources/Levels
void FileHelper::GetAllLevelsFromResources(vector<string>& result)
{
	GetFilesInResourcePath("levels/", result);
}

//если поместить этот функтор внутри метода, то gcc выдаст ошибку при передачи этого функтора в remove_if :'(
//In the standard (C++98/03) local classes (local functors) can't be used as classes as a template parameter
//TODO: как сделать, чтобы VC++ выдавал ошибки компиляции по стандарту c++ >_<
struct NonPostfixRemover
{
	private_ string _postfix;
	public_ NonPostfixRemover(const string& postfix) : _postfix(postfix) {}

	protected_ inline bool IsNoPostfix(const string& str) const
	{
		uint len = _postfix.length();
		bool noPostfix = (str.length() <= len || (str.substr(str.length() - len, len) != _postfix)); 
		return noPostfix;
	}

	public_ bool operator()(const string& str) const 
	{ 
		return IsNoPostfix(str);
	}
};

struct NonPrefixAndPostfixRemover : protected NonPostfixRemover
{
	private_ string _prefix;
	public_ NonPrefixAndPostfixRemover(const string& prefix, const string& postfix) : 
		NonPostfixRemover(postfix), _prefix(prefix) {}

	protected_ inline bool IsNoPrefix(const string& str) const
	{
		uint len = _prefix.length();
		bool noPrefix = (str.length() <= len || (str.substr(0, len) != _prefix));  //substr начинается с 0-го символа
		return noPrefix;
	}

	public_ bool operator()(const string& str) const 
	{ 
		return IsNoPrefix(str) || IsNoPostfix(str);
	}
};


void FileHelper::GetAllLevelsFromUserPath(vector<string>& result, const string& prefix, const string& postfix)
{
	auto path = CCFileUtils_SHARED::getWritablePath();
	GetFilesInDevicePath(path, result);

	auto new_end = std::remove_if(result.begin(), result.end(), NonPrefixAndPostfixRemover(prefix, postfix));
	result.erase(new_end, result.end());
}

void FileHelper::GetAllLevelsFromUserPath(vector<string>& result)
{
	auto path = CCFileUtils_SHARED::getWritablePath();
	GetFilesInDevicePath(path, result);
	
	//TODO: попробовать в XCode 4.4 с поддержкой с++11 лямбд
	//auto nonTxtRemover = [](const string& str) {
	//	return (str.length() <= 4 || (str.substr(str.length() - 4, 4) != ".txt")); };
	
	//TODO: проверить
	auto new_end = std::remove_if(result.begin(), result.end(), NonPostfixRemover(".txt"));
	result.erase(new_end, result.end());
}

string FileHelper::GetDocumentsPath()
{
	return CCFileUtils_SHARED::getWritablePath();
}

bool FileHelper::LoadContentFromUserPathFile(const string& fileName, string& content)
{
	string fullPath = CCFileUtils_SHARED::getWritablePath() + fileName;
	CCFileUtils_SHARED::SET_IS_POPUP_NOTIFY(false);  //отключаем на период вызова метода вызов сообщения об ошибке
	bool result = FromFileToString(fullPath, content);
	CCFileUtils_SHARED::SET_IS_POPUP_NOTIFY(true);
	return result;
}

bool FileHelper::SaveContentToFile(const string& fullPath, const string& content, bool isConvertFromUTF8ToUTF16)
{
	std::ofstream file;
	
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	//ofstream use UTF16, not UTF8. В VisualC++ ofstream принимает wchar
	if (isConvertFromUTF8ToUTF16)
	{
		auto fullPathUTF16 = cc_utf8_to_utf16(fullPath.c_str());
		file.open((wchar_t*)fullPathUTF16);
		free(fullPathUTF16);
	}
	else
	{
		file.open(fullPath);
	}
#else
	file.open(fullPath.c_str()); //TODO: проверить открытие файлов с путями русскими символами на MacOS
#endif

	if (!file.is_open())
	{
		MY_LOG(StringHelper::format("Error saved at '%s'", fullPath.c_str()));
		return false;
	}
	file << content;
	MY_LOG(StringHelper::format("'%s' saved ", fullPath.c_str()));
	return true;
}

bool FileHelper::SaveContentToUserPathFile(const string& fileName, const string& content)
{
	string fullPath = CCFileUtils_SHARED::getWritablePath() + fileName;
	return SaveContentToFile(fullPath, content, true);
}

//сохранить level с названием userLevelName
void FileHelper::SaveUserLevel(const string& userLevelName, const string& content)
{
	//if linux then create dir!
	auto path = CCFileUtils_SHARED::getWritablePath();
	string fullPath = path + userLevelName + ".txt";
	std::ofstream file(fullPath.c_str());
	if (!file.is_open())
	{
		MY_LOG(StringHelper::format("Error saved at '%s'", fullPath.c_str()));
		return;
	}

	file << content;

	MY_LOG(StringHelper::format("UserLevel saved at '%s'", fullPath.c_str()));
}

bool FileHelper::FromFileToString(const string& absoluteFileName, string& result)
{
	CCFileData ini_file_raw(absoluteFileName.c_str(), "rb", false);
	if (ini_file_raw.getSize() == 0)
	{
		MY_LOG(StringHelper::format("Can't find '%s'", absoluteFileName.c_str()).c_str());
		return false;
	}

	MY_LOG(MAKE_STRING("load file " << absoluteFileName.c_str()));

	char* buffer = (char*)ini_file_raw.getBuffer();
	result.assign(buffer, buffer + ini_file_raw.getSize());
	//string mapString(buffer, buffer + ini_file_raw.getSize());
	return true;
}

bool FileHelper::IsFileExists(const string& fullPath)
{
	return CCFileUtils::sharedFileUtils()->isFileExist(fullPath);

	//TODO: need to remove
	//doesn't work on android
	FILE *fp = fopen(fullPath.c_str(), "r");
	if (fp != NULL) //if exists
	{
		fclose(fp);
		return true;
	} 

	return false;
}
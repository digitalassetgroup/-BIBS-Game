#pragma once
#ifndef FileHelper_h
#define FileHelper_h

#include "Core/CoreAll.h"

namespace FileHelper
{
	void GetFilesInResourcePath(const string& path, vector<string>& result);
	void GetAllLevelsFromResources(vector<string>& result);
	void SaveUserLevel(const string& userLevelName, const string& content);
	void GetAllLevelsFromUserPath(vector<string>& result, const string& prefix, const string& postfix);
	void GetAllLevelsFromUserPath(vector<string>& result);
	bool FromFileToString(const string& absoluteFileName, string& result);
	
	bool LoadContentFromUserPathFile(const string& fileName, string& content);
	bool SaveContentToUserPathFile(const string& fileName, const string& content);
	bool SaveContentToFile(const string& fullPath, const string& content, bool isConvertFromUTF8ToUTF16);

	string GetDocumentsPath();
	bool IsFileExists(const string& fullPath);
}

#endif

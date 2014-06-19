#pragma once
#ifndef ScreenLogListener_h
#define ScreenLogListener_h

#include "Core/CoreAll.h"
#include "LogHelper.h"
#include "LogObserver.h"
#include <queue>

class ScreenLogListener
{
	private_ unsigned int _stringsMaxCount; //макс. количество стрингов
	private_ unsigned int _stringMaxLength; //макс. длина строки
	private_ list<string> _strings;
	private_ bool _isNeedToUpdateScreen;
	public_ bool IsNeedToUpdateScreen();

	public_ ScreenLogListener();
	public_ ~ScreenLogListener();
	private_ void SetLogText(LogObserverInfo* logText);
	public_ void GetString(string& result);
	public_ void ClearLog();
};

#endif

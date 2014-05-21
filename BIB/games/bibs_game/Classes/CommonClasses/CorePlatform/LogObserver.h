#pragma once
#ifndef LogObserver_h
#define LogObserver_h

#include "Core/CoreAll.h"
#include "Core/ObserverEvent.h"

struct LogObserverInfo
{
	//string* Level;
	const string* FileName;
	const char LogMessageType;
	const string* Message; 
	unsigned int Line;

	LogObserverInfo(const string* fileName, const char logMessageType, unsigned int line, const string* message)
		: FileName(fileName), LogMessageType(logMessageType), Line(line), Message(message) {}

	const char* GetLogTypeString()
	{
		static const char* stInfo = "INFO";
		static const char* stWarning = "WARNING";
		static const char* stError = "ERROR";

		switch (LogMessageType)
		{
			case 'I' : return stInfo;
			case 'W' : return stWarning;
			case 'E' : return stError;
		}

		return "";
	}

	string MakeString()
	{
		return StringHelper::format("%s (%d): %s - %s", (*FileName).c_str(), Line, 
			GetLogTypeString(), (*Message).c_str());
		//return MAKE_STRING(*FileName << ":" << Line << " - " << *Message);
	}
};


class LogObserver
{
	public_ static LogObserver* Inst()
	{
		static LogObserver _inst;
		return &_inst;
	}

	private_ LogObserver() {};
	public_ ObserverEvent<LogObserverInfo*> LogAddedEvent;
	//public_ ObserverEvent<LogObserverInfo*>* LogAddedEvent;
};

#endif

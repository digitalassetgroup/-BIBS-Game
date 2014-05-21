#include "ScreenLogListener.h"
#include "Core/StringHelper.h"

#include <ctime>

ScreenLogListener::ScreenLogListener()
	: _isNeedToUpdateScreen(false), _stringsMaxCount(20), _stringMaxLength(80)
{
	LogObserver::Inst()->LogAddedEvent.AttachMember(this, &ScreenLogListener::SetLogText);
}

ScreenLogListener::~ScreenLogListener()
{
	LogObserver::Inst()->LogAddedEvent.DetachMember(this, &ScreenLogListener::SetLogText);
}

static string GetTimeNow()
{
	time_t now = time(0);
	tm* localtm = localtime(&now);
	return StringHelper::format("%02d:%02d:%02d", localtm->tm_hour, localtm->tm_min, localtm->tm_sec);
}

void ScreenLogListener::SetLogText(LogObserverInfo* logText)
{
#ifndef DEBUG_GAME
	if (logText->LogMessageType == 'E')
		return;
#endif

	string message = *logText->Message;
	if (logText->LogMessageType != 'I')
	{
		message.insert(0, ": ");
		message.insert(0, logText->GetLogTypeString());
	}
	message.insert(0, GetTimeNow() + " - ");

	vector<string> logTextStrings;
	StringHelper::Tokenize(message, logTextStrings, "\n");

	for_each_iterator(logTextStrings)
	{
		string& s = *it;
		if (s.size() <= _stringMaxLength)
			_strings.push_back(s);
		else
			StringHelper::SplitByLength(_strings, s, _stringMaxLength);
	}
	
	while(_strings.size() > _stringsMaxCount)
		_strings.pop_front();

	_isNeedToUpdateScreen = true;
}

void ScreenLogListener::GetString(string& result)
{
	StringHelper::Join(result, _strings, "\n");
}

bool ScreenLogListener::IsNeedToUpdateScreen()
{
	if (_isNeedToUpdateScreen)
	{
		bool result = _isNeedToUpdateScreen;
		_isNeedToUpdateScreen = false;
		return result;
	}

	return false;
}

void ScreenLogListener::ClearLog()
{
	_strings.clear();
	_isNeedToUpdateScreen = true;
}
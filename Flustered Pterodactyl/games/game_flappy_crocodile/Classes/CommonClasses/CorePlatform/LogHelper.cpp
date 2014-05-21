#include "LogHelper.h"
#include "LogObserver.h"
#include <stdarg.h>
#include "platform/CCPlatformConfig.h"

void __my_assert(bool cond, const char* msg, const char* file, int line)
{
	if (!cond)
	{
		auto some_string = MAKE_STRING(file << ":" << line << " - " << msg);
		//CCAssert((cond), some_string.c_str());
		LOGE("%s", some_string.c_str());
        
        #if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
            assert(cond);
        #endif
	}
}

void __my_assert(bool cond, const string& msg, const char* file, int line)
{
	__my_assert(cond, msg.c_str(), file, line);
}

string __my_log_extended(const string& msg, const char logMessageType, const char* file, int line)
{
	string filestr(file);
	LogObserverInfo logInfo(&filestr, logMessageType, line, &msg);
	LogObserver::Inst()->LogAddedEvent.Notify(&logInfo);

	auto str = logInfo.MakeString();
	cocos2d::CCLog("%s", str.c_str()); //temp
	return str;
}

#if LOG_LEVEL <= 1
    //WARNING: в функциях MY_LOG_F и MY_LOG_ERROR_F некорректно указывается file и line
    void MY_LOG_F(const char* fmt, ...)
    {
        va_list marker;
        va_start(marker, fmt);
        string msg = StringHelper::format_v(fmt, marker);
        va_end(marker);
        __MY_LOG_EXTENDED(msg, 'I', LOGI, CCLOG, __FILE__, __LINE__);
    }
#endif


#if LOG_LEVEL <= 2
    void MY_LOG_WARNING_F(const char* fmt, ...)
    {
        va_list marker;
        va_start(marker, fmt);
        string msg = StringHelper::format_v(fmt, marker);
        va_end(marker);
        __MY_LOG_EXTENDED(msg, 'W', LOGW, CCLOGERROR, __FILE__, __LINE__);
    }
#endif

void MY_LOG_ERROR_F(const char* fmt, ...)
{
	va_list marker;
	va_start(marker, fmt);
	string msg = StringHelper::format_v(fmt, marker);
	va_end(marker);
	__MY_LOG_EXTENDED(msg, 'E', LOGE, CCLOGERROR, __FILE__, __LINE__);
}

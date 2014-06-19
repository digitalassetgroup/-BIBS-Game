#pragma once
#ifndef LogHelper_h
#define LogHelper_h

#include "Core/CoreAll.h"
#include <platform/CCPlatformMacros.h>
#include <ccMacros.h>
#include <iostream>

#if 1 //DEBUG
	#define LOG_LEVEL 1 //1 - all logs; 2 - warning and more; 3 - error
#else
	#define LOG_LEVEL 3
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	#include <android/log.h>

	#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "JNI_DEBUGGING", __VA_ARGS__)
	#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   "JNI_DEBUGGING", __VA_ARGS__)
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,    "JNI_DEBUGGING", __VA_ARGS__)
	#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,    "JNI_DEBUGGING", __VA_ARGS__)
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,   "JNI_DEBUGGING", __VA_ARGS__)
#else
	#define LOGV(...) void(0)
	#define LOGD(...) void(0)
	#define LOGI(...) void(0)
	#define LOGW(...) void(0)
	#define LOGE(...) void(0)
#endif

void __my_assert(bool cond, const char* msg, const char* file, int line);
void __my_assert(bool cond, const string& msg, const char* file, int line);

#define MY_ASSERT(cond, msg) \
	do \
	{ \
		if (!(cond)) \
			std::cerr << (msg); \
		CC_ASSERT(cond); \
		__my_assert(cond, msg, __FILE__, __LINE__); \
	} \
	while(false);

#define MY_ASSERT_ONLY(cond) \
	MY_ASSERT(cond, "")

string __my_log_extended(const string& msg, const char errorType, const char* file, int line);

#define __MY_LOG_EXTENDED(msg, errorType, log_method1, log_method2, file, line) \
	do  \
	{ \
		auto some_string = __my_log_extended(msg, errorType, file, line); \
		log_method1("%s", some_string.c_str()); /*по идее, для этих штук нужны свои лог-листенеры*/ \
		log_method2("%s", some_string.c_str()); \
	} \
	while(false);

#define MY_LOG_ERROR(msg) \
	do  \
	{ \
		std::string msgFull = MAKE_STRING(msg); \
		__MY_LOG_EXTENDED(msgFull, 'E', LOGE, CCLOGERROR, __FILE__, __LINE__); \
		MY_ASSERT((#msg, false), msgFull); /*comment on iOS*/ \
	} \
	while(false);

#if LOG_LEVEL <= 1
    #define MY_LOG(msg) \
        __MY_LOG_EXTENDED(MAKE_STRING(msg), 'I', LOGI, CCLOG, __FILE__, __LINE__)
#else
    #define MY_LOG(msg) 
#endif



#if LOG_LEVEL <= 1
    void MY_LOG_F(const char* fmt, ...);
#else
    static inline void MY_LOG_F(const char* fmt, ...) {}
#endif

#if LOG_LEVEL <= 2
    void MY_LOG_WARNING_F(const char* fmt, ...);    
#else
    static inline void MY_LOG_WARNING_F(const char* fmt, ...) {}
#endif

void MY_LOG_ERROR_F(const char* fmt, ...);

#endif

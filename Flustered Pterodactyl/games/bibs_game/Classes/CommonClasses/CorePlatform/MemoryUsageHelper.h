#ifndef MemoryUsageLogger_h
#define MemoryUsageLogger_h

#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        #define DEFINE_IOS_METHOD(...) __VA_ARGS__;
#else
        #define DEFINE_IOS_METHOD(...) static inline __VA_ARGS__ {}
#endif

namespace MemoryUsageHelper
{
	DEFINE_IOS_METHOD(void LogMemoryUsage());
}

#endif

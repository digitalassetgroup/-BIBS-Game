#pragma once

#ifndef __GAF_COCOS_2DX_H__
#define __GAF_COCOS_2DX_H__

#define CC_SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define CC_SAFE_RELEASE(p)           do { if(p) { (p)->release(); } } while(0)
#define CC_SAFE_RELEASE_NULL(p)      do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define CC_SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)

#include "IwDebug.h"

#define GAFLOGERROR(format, ...)  IwDebugTraceLinePrintf(format, ##__VA_ARGS__)
#define GAFLOGWARN(format, ...)   IwDebugTraceLinePrintf(format, ##__VA_ARGS__)
#define GAFLOGINFO(format, ...)   IwDebugTraceLinePrintf(format, ##__VA_ARGS__)
#define GAFLog(str)               IwDebugTraceLinePrintf(str)
#define GAFLOG(str)               IwDebugTraceLinePrintf(str)

#include <cassert>

#define GAFAssert(cond, msg)     IwAssertMsg(0, (cond), (msg))

#define GAFDICT_FOREACH(___dict_, pElement) for(CCDictionaryIt pElement = (___dict_) ->begin(); pElement != (___dict_) ->end(); ++pElement)

#endif // __GAF_COCOS_2DX_H__

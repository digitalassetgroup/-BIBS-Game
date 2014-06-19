#pragma once
#ifndef _DracoGames_igAssert_h_
#define _DracoGames_igAssert_h_

#include <assert.h>
#include "../scenes/scene_manager.h"	/* Included to display the AssertScene. */
//#include "analytics.h"

#ifdef DEBUG

#define assert_if(test, msg, ...)  {												\
	if (test) {																		\
		printf("\n\n\nASSERTION FAILED!\n FILE: %s : %d\n", __FILE__, __LINE__);	\
		printf(msg, ##__VA_ARGS__);													\
		printf("\n\n\n");															\
		CSceneManager::gotoAssertScene(__FILE__, __LINE__, msg, ##__VA_ARGS__);		\
	} assert(!(test) && (msg));														\
}

#define warning_if(test, msg, ...) if (test) { printf(msg, ##__VA_ARGS__); }

#else /* RELEASE */

//#define assert_if(test, msg, ...) {									\
//	if (test) {														\
//		char *str;													\
//		std::string errorMessage;									\
//		asprintf(&str, "File %s, line %d. ", __FILE__, __LINE__);	\
//		errorMessage.append(str);									\
//		free(str);													\
//		asprintf(&str, msg, ##__VA_ARGS__);							\
//		errorMessage.append(str);									\
//		free(str);													\
//	}																\
//}
//Analytics::logError("Assert_if", errorMessage);

#define assert_if(test, msg, ...)

#define warning_if(test, msg, ...)

#endif /* DEBUG vs RELEASE */

#endif /* _DracoGames_igAssert_h_ */

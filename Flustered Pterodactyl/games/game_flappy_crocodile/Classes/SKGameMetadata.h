#pragma once
#ifndef  __SK_GAME_METADATA_H__
#define  __SK_GAME_METADATA_H__

#include "platform/CCPlatformConfig.h"

//--------

#ifndef SK_PAID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) // Free
#define APP_STORE_URL                               "http://itunes.apple.com/us/app/cilink-free/id604419691?ls=1&mt=8"
#define DEFAULT_LEADERBOARD_STORE_ID                "com.dracollc.flappycrocodilefree.leader"
#define INAPP_UNLOCKALL_STORE_ID                    "com.dracollc.cilinkfree.unlock"
#define INAPP_REMOVE_ADS_STORE_ID                   "com.dracollc.cilinkfree.removeads"
#define FLURRY_APP_KEY                              "NOPE"
#define CB_APP_ID                                   "52bd45f32d42da5ca1392aa0"
#define CB_APP_SIGNATURE                            "a1253cfd68af4dccc4da285a1dea0161d98468fc"
#define REVMOB_APP_ID                               "52bd44ce11a6bf8c0f00004d"
#define ADMOB_MEDIATION_ID                          "8df28fedb7754d1a"
#define ADCOLONY_APPLICATION_ID                     "app60e374da1ff04c60914a7c"
#define ADCOLONY_ZONE                               "vzf128e42f9186469e981ea4"
#define PLAYHEAVEN_TOKEN                            "a3456dca3f29478cba81b3bbee512e93"
#define PLAYHEAVEN_SECRET                           "63a020bd968a4c08a6e5dbba360aea95"
#define TAPJOY_APP_ID								"NOPE"
#define TAPJOY_APP_KEY								"NOPE"


#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) // Free

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) // Free

#define DEFAULT_LEADERBOARD_STORE_ID                "CgkIw5Cynv8aEAIQBw"

#define APP_STORE_URL                               "https://play.google.com/store/apps/details?id=com.dracollc.flappycrocodile&hl=en"
#define FLURRY_APP_KEY                              "NOPE"
#define FLURRY_AD_KEY                               "NOPE"
#define TAPJOY_APP_ID								"NOPE"
#define TAPJOY_APP_KEY								"NOPE"
#define SPONSORPAY_APP_ID                           "NOPE"
#define SPONSORPAY_APP_KEY                          "NOPE"
#define ADCOLONY_APPLICATION_ID                     "NOPE"
#define ADCOLONY_ZONE                               "NOPE"

#define ADMOB_MEDIATION_ID                          "a152effe648927f"

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) //

#else
#error "SKGameMetadata does not support this platfrom, fix it"
#endif

#else // #ifdef SK_PAID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) // Paid
#define APP_STORE_URL                               "https://itunes.apple.com/us/app/cilink-pro/id623451866?ls=1&mt=8"
#define DEFAULT_LEADERBOARD_STORE_ID                "com.dracollc.cilinkpaid.leader"
#define INAPP_UNLOCKALL_STORE_ID                    "com.dracollc.cilinkpaid.unlock"
#define FLURRY_APP_KEY                              "GHP6H5XBPZB6SSBY348R"
#define CB_APP_ID                                   "52bd45f32d42da5ca1392aa0"
#define CB_APP_SIGNATURE                            "a1253cfd68af4dccc4da285a1dea0161d98468fc"
#define REVMOB_APP_ID                               "52bd44ce11a6bf8c0f00004d"
#define ADCOLONY_APPLICATION_ID                     "app585115b3a3de47eb8cb910"
#define ADCOLONY_ZONE                               "vzf709bea8b99446cca342c7"
#define PLAYHEAVEN_TOKEN                            "525fd5245ac7404f9074db361a688f27"
#define PLAYHEAVEN_SECRET                           "956327ce06c04c2eaaeebf6e4f370c9b"

#define INAPP_REMOVE_ADS_STORE_ID                   "NOPE"
#define ADMOB_MEDIATION_ID                          "NOPE"
#define TAPJOY_APP_ID								"NOPE"
#define TAPJOY_APP_KEY								"NOPE"



#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) // Paid


#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) // Paid

#else
#error "SKGameMetadata does not support this platfrom, fix it"
#endif


#endif // #ifdef SK_PAID


void InitAchievements();

#endif // __SK_GAME_METADATA_H__


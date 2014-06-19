#pragma once
#ifndef  __SK_GAME_METADATA_H__
#define  __SK_GAME_METADATA_H__

#include "platform/CCPlatformConfig.h"

//--------

#ifndef SK_PAID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) // Free
#define APP_STORE_URL                               "http://itunes.apple.com/us/app/cilink-free/id816870672?ls=1&mt=8"
#define DEFAULT_LEADERBOARD_STORE_ID                "com.appfresh.flappyfairyleaderboard"
#define INAPP_UNLOCKALL_STORE_ID                    "com.dracollc.cilinkfree.unlock"
#define INAPP_REMOVE_ADS_STORE_ID                   "com.dracollc.cilinkfree.removeads"
#define FLURRY_APP_KEY                              "7XK5MW492NMGH92F5YW8"
#define CB_APP_ID                                   "NOPE"
#define CB_APP_SIGNATURE                            "NOPE"
#define REVMOB_APP_ID                               "NOPE"
#define ADMOB_MEDIATION_ID                          "NOPE"
#define ADCOLONY_APPLICATION_ID                     "NOPE"
#define ADCOLONY_ZONE                               "NOPE"
#define PLAYHEAVEN_TOKEN                            "NOPE"
#define PLAYHEAVEN_SECRET                           "NOPE"
#define TAPJOY_APP_ID								 "NOPE"
#define TAPJOY_APP_KEY								 "NOPE"


#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) // Free

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) // Free

#define DEFAULT_LEADERBOARD_STORE_ID             "CgkIw5Cynv8aEAIQBw"

#define APP_STORE_URL                               "https://play.google.com/store/apps/details?id=com.dracollc.flappycrocodile&hl=en"
#define FLURRY_APP_KEY                               "NOPE"
#define FLURRY_AD_KEY                                "NOPE"
#define TAPJOY_APP_ID								 "NOPE"
#define TAPJOY_APP_KEY								 "NOPE"
#define SPONSORPAY_APP_ID                            "NOPE"
#define SPONSORPAY_APP_KEY                           "NOPE"
#define ADCOLONY_APPLICATION_ID                      "NOPE"
#define ADCOLONY_ZONE                                "NOPE"

#define ADMOB_MEDIATION_ID                           "a152effe648927f"

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) //

#else
#error "SKGameMetadata does not support this platfrom, fix it"
#endif

#else // #ifdef SK_PAID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) // Paid
#define APP_STORE_URL                               "https://itunes.apple.com/us/app/cilink-pro/id623451866?ls=1&mt=8"
#define DEFAULT_LEADERBOARD_STORE_ID                "com.dracollc.cilinkpaid.leader"
#define INAPP_UNLOCKALL_STORE_ID                    "com.dracollc.cilinkpaid.unlock"
#define FLURRY_APP_KEY                              "DYBR33PK2J89F99B6MHM"
#define CB_APP_ID                                   "5148cdd716ba470f7600005c"
#define CB_APP_SIGNATURE                            "859ce510ecf3b87727f4f1d61d1e2d2646a01f5d"
#define REVMOB_APP_ID                               "51f9491c617a702f9200001a"
#define ADCOLONY_APPLICATION_ID                     "app585115b3a3de47eb8cb910"
#define ADCOLONY_ZONE                               "vzf709bea8b99446cca342c7"
#define PLAYHEAVEN_TOKEN                            "525fd5245ac7404f9074db361a688f27"
#define PLAYHEAVEN_SECRET                           "956327ce06c04c2eaaeebf6e4f370c9b"


#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) // Paid


#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) // Paid

#else
#error "SKGameMetadata does not support this platfrom, fix it"
#endif


#endif // #ifdef SK_PAID


void InitAchievements();

#endif // __SK_GAME_METADATA_H__


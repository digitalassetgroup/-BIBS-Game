#include "SKGameMetadata.h"
#include "sk_game_services/sk_game_services.h"
#include <cocos2d.h>
#include "SimpleAudioEngine.h"
using namespace cocos2d;
using namespace CocosDenshion;

namespace sk
{
	namespace game_services
	{
		bool are_in_apps_disabled() { return false; }
		int levels_per_interstitial() { return 0; }
		bool disable_offer_wall() { return false; }
		bool disable_video_ad() { return false; }

		bool disable_all_inerestials()
		{
			// !!!! must be false in production code
			return true;
		}
		
		supported_orientations get_supported_orientation()
		{
            #if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
                return SO_LANDSCAPE_ONLY;
            #else
                return SO_PORTRAIT_ONLY;
            #endif
		}
		
		bool disable_retina_ipad()
		{
			return true;
		}
		
		bool is_game_paid()
		{
#ifdef SK_PAID
			return true;
#else
			return false;
#endif
		}
		
		const char* desktop_title()
		{
			return "Sugar";
		}

		const char* game_title()
		{
			return desktop_title();
		}

		int desktop_prefered_windowed_height()
		{
			return 640;
		}

		int get_offer_wall_provider() { return 0; }

		bool disable_banner_ads() 
		{ 
			return false;
		}

		bool is_game_center_disabled() { return false; }
		
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        mac_mouse_handlers get_mac_mouse_handler()
        {
            return MAC_MOUSE_HANDLER_COCOS_STANDART;
        }
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		const char * android_base64_encoded_public_key() { return " "; }
		android_purchase_mode get_android_purchase_mode() { return APM_REAL_SKU; }
		int swarm_app_id() { return 0; }
		const char * swarm_app_key() { return " "; }
		bool is_admob_in_test_mode() { return true; }

		

		const char* get_flurry_app_key() { return " "; }
		const char* sponsorpay_app_id() { return " "; }
		const char* sponsorpay_app_key() { return " "; }

		const char* get_flurry_ad_key() { return " "; }
		const char* get_adcolony_zone() { return " "; }
		const char* get_adcolony_app_id() { return " "; }
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) 
        
        const char* tapjoy_app_id()
        {
            return TAPJOY_APP_ID;
        }
        
		const char* tapjoy_app_key()
        {
            return TAPJOY_APP_KEY;
        }
        
		const char * get_admob_mediation_id()
		{
			return ADMOB_MEDIATION_ID;
		}

		const char * get_cb_app_id()
		{
			return CB_APP_ID;
		}

		const char * get_cb_app_signature()
		{
			return CB_APP_SIGNATURE;
		}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        
		const char * get_flurry_app_key()
		{
			return FLURRY_APP_KEY;
		}			
		
		const char * get_revmob_app_id()
		{
			return REVMOB_APP_ID;
		}
		
		const char * get_adcolony_app_id()
		{
			return ADCOLONY_APPLICATION_ID;
		}
		
		const char * get_adcolony_zone()
		{
			return ADCOLONY_ZONE;
		}
		
		const char * get_playheaven_token()
		{
			return PLAYHEAVEN_TOKEN;
		}
		
		const char * get_playheaven_secret()
		{
			return PLAYHEAVEN_SECRET;
		}
        
        const char* facebook_app_id()
        {
            return "";
        }
        
        bool disable_all_interstitials()
        {
            return true;
        }
        
        bool is_banner_ads_in_test_mode()
        {
            return true;
        }
		
#ifndef SK_PAID		
#endif // #ifndef SK_PAID
		
#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		const char* android_share_score_subtitle() 
		{
			static const char* dummy = "";
			return dummy;
		}

		const char* android_share_score_description() 
		{ 
			return android_share_score_subtitle(); 
		}

		const char* android_share_score_picture_url() 
		{ 
			return android_share_score_subtitle(); 
		}
#endif

		void game_do_resume()
		{
			CCDirector::sharedDirector()->startAnimation();
			SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
		}
		
		void game_do_pause()
		{
			CCDirector::sharedDirector()->stopAnimation();
			SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
		}
		
	} // namespace game_services
} // namspace sk


void InitAchievements()
{
	sk::game_services::init_info info;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    info.app_store_url = APP_STORE_URL;
	info.add_leaderboard(0, DEFAULT_LEADERBOARD_STORE_ID);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	info.app_store_url = APP_STORE_URL;
	info.add_leaderboard(0, DEFAULT_LEADERBOARD_STORE_ID);
#endif
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    info.add_inapp(0, INAPP_REMOVE_ADS_STORE_ID, sk::game_services::on_ads_removed, true);
	info.add_inapp(1, INAPP_UNLOCKALL_STORE_ID, sk::game_services::on_unlock_all, true);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	
#endif

	sk::game_services::init(info);
}


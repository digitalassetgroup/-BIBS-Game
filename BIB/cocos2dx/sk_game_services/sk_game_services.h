#pragma once

#ifndef _SK_GAME_SERVICES__
#define _SK_GAME_SERVICES__

#define SK_NO_PLAYHEAVEN    // if enabled, Playheaven is not used
//#define SK_NO_ADMOB         // if enabled, it uses iAD only
//#define SK_NO_ADCOLONY      // if enabled, adcolony ads and TV buttons will not work
//#define SK_DISABLE_IN_APPS  // if enabled, no in-apps will
#define SK_NO_TAPJOY

#include <string>
#include <vector>
#include "platform/CCPlatformConfig.h"
#include "CCPlatformDefine.h"

#define SK_IS_JAILBROKEN sk::game_services::IS_JAILBROKEN
#define IS_JAILBROKEN asdfsadfsadfs3adf


#define ui_on_purchase fjf32urfgifsdiufs
#define ui_on_video_ad_award fffufs34t43j2jj2
#define ui_on_currency_earned adfsdfsdfdff


#define LAUNCH_KEY "asdfd9887fd9f8d98fdf"

namespace cocos2d
{
	class CCImage;
}

namespace sk
{
	
namespace game_services
{
    enum KEYCODE
    {
        KEYCODE_1,
        KEYCODE_2,
        KEYCODE_3,
        KEYCODE_4,
        KEYCODE_W,
        KEYCODE_A,
        KEYCODE_S,
        KEYCODE_D,
        KEYCODE_Z,
        KEYCODE_X,
        KEYCODE_SPACE,
        KEYCODE_LEFT,
        KEYCODE_RIGHT,
        KEYCODE_UP,
        KEYCODE_DOWN
    };
    
	enum OFFER_WALL_PROVIDER
	{
		OFFER_WALL_PROVIDER_TAPJOY,
		OFFER_WALL_PROVIDER_SPONSORPAY
	};
	
	typedef void (*game_logic_purchase_callback)();
	
	class CC_DLL ui_purchase_observer
	{
	public:
		virtual void ui_on_purchase(unsigned int game_id, bool purchased) = 0;
		virtual ~ui_purchase_observer() {}
	};
	
	
	class CC_DLL ui_dialog_observer
	{
	public:
		virtual void ui_on_yes_no_dialog(unsigned int dialog_id, bool result) = 0;
		virtual ~ui_dialog_observer() {}
	};
	
	class CC_DLL ui_offer_observer
	{
	public:
		virtual void ui_on_currency_earned(int amount) = 0;
		virtual ~ui_offer_observer() {}
	};
	
	class CC_DLL ui_video_ad_observer
	{
	public:
		virtual void ui_on_video_ad_award(int amount) = 0;
		virtual ~ui_video_ad_observer() {}
	};

	class CC_DLL facebook_check_page_like_observer
	{
	public:
		virtual void ui_on_facebook_liked(bool isLiked) = 0;
		virtual ~facebook_check_page_like_observer() {}
	};
	
	CC_DLL bool is_game_center_disabled();
	CC_DLL bool disable_offer_wall();
	CC_DLL bool is_offer_wall_enabled();
	CC_DLL bool is_video_ad_enabled();
	CC_DLL bool disable_video_ad();
	CC_DLL bool open_offer_wall();
	CC_DLL void on_offer_wall_closed();
	CC_DLL void on_offer_earned(int amount);
	CC_DLL void on_video_ad_award(int amount);
	
	CC_DLL void register_offer_wall_observer(ui_offer_observer * observer);
	CC_DLL void unregister_offer_wall_observer(ui_offer_observer * observer);
	
	CC_DLL void register_video_ad_observer(ui_video_ad_observer * observer);
	CC_DLL void unregister_video_ad_observer(ui_video_ad_observer * observer);
	
	//internal package
	CC_DLL void facebook_check_page_like_callback(bool isLiked);

	//public
	CC_DLL void facebook_register_check_like_observer(facebook_check_page_like_observer* observer);
	CC_DLL void facebook_unregister_check_like_observer(facebook_check_page_like_observer* observer);
	CC_DLL void facebook_check_page_like(const char* page_id);
	CC_DLL void facebook_open_game_page(const char* page_id);
	CC_DLL bool facebook_is_like_game_page_local();
	//


	enum supported_orientations
	{
		SO_UNDEFINED,
		SO_PORTRAIT_ONLY,
		SO_LANDSCAPE_ONLY
		// do not support both at the moment
	};
    
    enum mac_mouse_handlers
    {
        MAC_MOUSE_HANDLER_COCOS_STANDART,
        MAC_MOUSE_HANDLER_DRAGON
    };
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	enum android_purchase_mode
	{
		APM_REAL_SKU,
		APM_PURCAHSED,
		APM_CANCELED,
		APM_ITEM_UNAVALBLE,
		APM_REFUNDED
	};
	
	CC_DLL void on_android_adview_height_changed(int height);
	CC_DLL android_purchase_mode get_android_purchase_mode();
	CC_DLL const char * sponsorpay_app_id();
	CC_DLL const char *  sponsorpay_app_key();
	
	CC_DLL const char * android_get_device_model();
	CC_DLL bool is_galaxy_s2();
#endif
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CC_DLL bool ios_gamecenter_is_combined();
#endif
	
	enum dialog_type
	{
		DIALOG_TYPE_YES_NO
	};
	
	struct CC_DLL leaderboard
	{
		unsigned int game_id;
		std::string store_id;
	};
	
	struct CC_DLL achievement
	{
		unsigned int game_id;
		std::string store_id;
	};
	
	struct CC_DLL inapp
	{
		unsigned int game_id;
		std::string store_id;
		bool        confiremd_by_store;
		bool        purchased;
		bool        consumable;
		game_logic_purchase_callback logic_on_purchase; // this is only positive login for non-ui stuff
	};
	
	struct CC_DLL init_info
	{
		inline init_info()
		{
			
		}
		
		void add_leaderboard(int game_id, const char * store_id);
		void add_inapp(int game_id, const char * store_id, game_logic_purchase_callback logic, bool cosumable);
		void add_achievement(int game_id, const char * store_id);
		
		std::string app_store_url;		
		std::vector<leaderboard> leaderboards;
		std::vector<inapp>       inapps;
		std::vector<achievement> achievements;
	};
	
	enum ImageSource
	{
		ImageSourceUndefined,
		ImageSourceCamera,
		ImageSourcePhotoLibrary
	};
	
	class CC_DLL image_observer
	{
	public:
		virtual void on_get_image(cocos2d::CCImage * photo, ImageSource source) = 0;
	};
	

	
	struct CC_DLL image_info
	{
		inline image_info()
		:
		source(ImageSourceUndefined),
		 width(-1),
		height(-1)
		{
			
		}
		ImageSource source;
		int width;
		int height;
	};
	// return true if started taking photo
	CC_DLL bool get_image(image_observer * observer, const image_info& info = image_info());
	CC_DLL bool device_can_take_photo();
	CC_DLL void on_take_image(cocos2d::CCImage * photo);
	
	CC_DLL const image_info& get_image_info();
	

	CC_DLL void init(const init_info& info);	
	CC_DLL void on_rate_me_pressed();
	CC_DLL void show_more_games();
    CC_DLL void show_cb_ads();
    CC_DLL void show_revmob();
    CC_DLL void show_revmob_banner(bool shown);
	CC_DLL void log_event(const char * event);
	CC_DLL void on_level_ended();
	CC_DLL void show_video_ad(bool respect_ads_removed = true, bool show_fail_dialog = false);
	CC_DLL void on_gift_pressed();
	CC_DLL void on_tv_pressed();
	
	CC_DLL bool is_paused();
	CC_DLL void set_paused(bool paused);
	
	CC_DLL bool app_is_going_to_bg();
	CC_DLL void set_app_is_goint_to_bg(bool to_bg);
	
	CC_DLL int get_banner_height();
    
    CC_DLL int get_offer_wall_provider();
	
	CC_DLL void share_game_score_facebook(const char * message);
	CC_DLL void share_game_score_twitter (const char * message);
	CC_DLL void share_image_facebook(cocos2d::CCImage * image, const char * message);
	CC_DLL void save_image_to_photos(cocos2d::CCImage * image);
	
	struct CC_DLL email_info
	{
		inline email_info()
		:
		photo(0)
		{
		}
		std::string to;
		std::string subject;
		std::string message;
		cocos2d::CCImage * photo;
	};
	
	CC_DLL void send_email(const email_info& info);
	CC_DLL bool can_send_email();
	
	CC_DLL bool show_leaderboard(int leaderboars_id);
	CC_DLL bool show_achievements();
	CC_DLL bool gamecenter_pressed();
	
	CC_DLL void on_gamecenter_auth();
	CC_DLL void on_gamecenter_auth_failed();
	CC_DLL void on_achievement_reported(const char * achievement_store_id, float percent);
	CC_DLL void on_leaderboard_score_submitted(const char * leaderboard_store_id, int score);
	CC_DLL bool report_achievement(int achievent_id, float percent = 100.0f);
	CC_DLL bool submit_score(int leaderboars_id, unsigned int score);
	CC_DLL const char * get_leaderboard_store_id(unsigned int leaderboard_id);
	CC_DLL const char * get_achievement_store_id(unsigned int achievement_id);
	CC_DLL unsigned int get_achievement_game_id(const char * store_id);
	CC_DLL unsigned int get_leaderboard_game_id(const char * store_id);
	
	
	CC_DLL bool IS_JAILBROKEN();
	CC_DLL void on_launch();
	CC_DLL int num_launched();
	
	CC_DLL bool is_ads_removed();
	CC_DLL void on_ads_removed();

	CC_DLL void show_ads(bool isVisible);
	
	CC_DLL void on_unlock_all();
	CC_DLL bool is_all_unlocked();
	
	// for testing purposes only
	CC_DLL void clear_unlock_all();
	
	CC_DLL void restore_purchases();	
	CC_DLL int get_total_inapps();
	CC_DLL const char * get_inapp_store_id(int index);
	
	CC_DLL bool buy_product(unsigned int game_id);
	CC_DLL void on_purchase(const char * store_id, bool purchased);
	CC_DLL int get_index_for_in_app(const char * store_id);
	CC_DLL void on_inapp_confirmed_by_store(const char * store_id);
	CC_DLL void   register_ui_purchase_observer(ui_purchase_observer * observer);
	CC_DLL void unregister_ui_purchase_observer(ui_purchase_observer * observer);	
	
	CC_DLL const char * game_title();
	CC_DLL bool is_inapp_consumable(const char * store_id);
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CC_DLL const char * android_base64_encoded_public_key();
	CC_DLL const char * android_share_score_subtitle();
	CC_DLL const char * android_share_score_description();
	CC_DLL const char * android_share_score_picture_url();
	CC_DLL bool is_admob_in_test_mode();
#endif

	CC_DLL std::string GetAppStoreName();
	
	CC_DLL const char * get_app_store_url();
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID ||  CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	CC_DLL const char * get_flurry_app_key();
	CC_DLL const char * get_flurry_ad_key();
    CC_DLL const char * tapjoy_app_id();
	CC_DLL const char * tapjoy_app_key();
	//CC_DLL const char * tapjoy_currency_id(int type);
#endif
	CC_DLL const char * get_cb_app_id();	
	CC_DLL const char * get_cb_app_signature();
	CC_DLL const char * get_revmob_app_id();
	CC_DLL const char * get_adcolony_app_id();
	CC_DLL const char * get_adcolony_zone();
#ifndef SK_NO_PLAYHEAVEN
	CC_DLL const char * get_playheaven_token();
	CC_DLL const char * get_playheaven_secret();
	CC_DLL void set_ph_shown(bool);
#endif
	
	CC_DLL void set_cb_shown(bool);
	CC_DLL void set_revmob_shown(bool);
	
	CC_DLL int  levels_per_interstitial();
	CC_DLL bool is_interstitial_shown();	
	CC_DLL bool disable_all_interstitials();
	CC_DLL bool disable_banner_ads();
	CC_DLL bool is_banner_ads_in_test_mode();
	CC_DLL bool are_in_apps_disabled();
#ifndef SK_PAID
	CC_DLL const char * get_admob_mediation_id();
#endif
	
	CC_DLL const char * facebook_app_id();
	CC_DLL supported_orientations get_supported_orientation();
	CC_DLL bool is_portrait();
	CC_DLL bool is_landscape();
	CC_DLL bool disable_retina_ipad();
	CC_DLL bool is_game_paid();
	CC_DLL void open_url(const char * url);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	CC_DLL int windowed_width();
	CC_DLL int windowed_height();
	CC_DLL int desktop_width();
	CC_DLL int desktop_height();
	CC_DLL int desktop_prefered_windowed_height();
	CC_DLL const char * desktop_title();	
#endif
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	CC_DLL bool mac_gamecenter_supported();
	CC_DLL bool mac_inapps_supported();
    CC_DLL mac_mouse_handlers get_mac_mouse_handler();
#endif
	
	
	// all dialogs are modal
	CC_DLL bool open_yes_no_dialog(unsigned int dialog_id, ui_dialog_observer * observer, const std::string& message, const std::string& title = "", const std::string& yes_text = "Yes", const std::string& no_text = "No");
	
	CC_DLL void set_yes_no_dialog_result(bool result);
		
	
	CC_DLL bool is_music_on();
	CC_DLL bool is_sounds_on();
	CC_DLL void set_music_on(bool on);
	CC_DLL void set_sounds_on(bool on);

	CC_DLL void game_do_resume();
	CC_DLL void game_do_pause();

#if CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT

	class CC_DLL GameServicesBase
	{
	public:
		static void SetGameServices(GameServicesBase* gameServices);
		static void RemoveGameServices();
		static GameServicesBase* GetGameServices();

	public:
		virtual ~GameServicesBase() {};

		virtual bool are_in_apps_disabled() = 0;
		virtual bool disable_offer_wall() = 0;
		virtual bool disable_video_ad() = 0;
		virtual bool is_game_center_disabled() = 0;
		virtual bool is_game_paid() = 0;
		virtual supported_orientations get_supported_orientation() = 0;
		virtual int levels_per_interstitial() = 0;
		virtual bool disable_all_interstitials() = 0;
		virtual void game_do_resume() = 0;
		virtual void game_do_pause() = 0;

		//
		virtual bool disable_banner_ads() { return true; }
		virtual bool is_banner_ads_in_test_mode() { return false; }
		virtual bool disable_retina_ipad() { return false; }
		virtual const char* get_admob_mediation_id() { return ""; }
		virtual const char* facebook_app_id()  { return ""; }

		#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
			virtual int desktop_prefered_windowed_height() = 0;
			virtual const char* desktop_title() = 0;
		#endif

		#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
			virtual mac_mouse_handlers get_mac_mouse_handler() = 0;
		#endif

		virtual const char* get_cb_app_id() { return ""; }
		virtual const char* get_cb_app_signature() { return ""; }
		virtual const char* get_flurry_app_key() { return ""; }
		virtual const char* get_flurry_ad_key() { return ""; }
		//ios
		virtual const char* get_revmob_app_id() { return ""; }
		virtual const char* get_adcolony_app_id() { return ""; }
		virtual const char* get_adcolony_zone() { return ""; }
		virtual const char* get_playheaven_token() { return ""; }
		virtual const char* get_playheaven_secret() { return ""; }
		//

		virtual const char* game_title() = 0;

		//android
		#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
			virtual int get_offer_wall_provider() = 0;
			virtual const char * tapjoy_app_id() = 0;
			virtual const char * tapjoy_app_key() = 0;
		    virtual const char * tapjoy_currency_id(int type) = 0;
			virtual const char * sponsorpay_app_id() = 0;
			virtual const char * sponsorpay_app_key() = 0;
			virtual android_purchase_mode get_android_purchase_mode() = 0;
			virtual bool is_admob_in_test_mode() = 0;
			virtual const char * android_base64_encoded_public_key() = 0;
			virtual const char * android_share_score_subtitle() = 0;
			virtual const char * android_share_score_description() = 0;
			virtual const char * android_share_score_picture_url() = 0;
		#endif


	};

#endif

} // namespace game_servies
	
} // namespace sk



#endif // _SK_GAME_SERVICES__
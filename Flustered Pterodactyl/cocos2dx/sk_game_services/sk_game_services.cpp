#include "sk_game_services.h"
#include "cocos2d.h"

using namespace cocos2d;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void ios_gc_init();
bool ios_gc_show_leaderboard(const char * leaderboard);
void ios_gc_submit_score(const char * cat, unsigned int value);
void ios_inapps_init();
bool ios_buy_product(const char * store_id);
void ios_restore_purchases();
void ios_on_ads_remove();
void ios_show_video_ad( bool respectAdsRemoved );
void ios_show_cb();
void ios_show_ph();
void ios_show_revmob();
void ios_show_revmob_banner(bool shown);
void ios_open_url(const char * url);
void ios_show_more_games();
void ios_log_event(const char * event);
void ios_share_facebook(const char * message);
void ios_share_twitter (const char * message);
void ios_share_image_facebook(cocos2d::CCImage * image, const char * message);
void ios_save_image_to_photos(cocos2d::CCImage * image);
bool ios_device_can_take_photo();
void ios_take_image();
void ios_start_yes_no_dialog(const std::string& message, const std::string& title, const std::string& yes_text, const std::string& no_text);
void ios_share_email(const sk::game_services::email_info& info);
bool ios_can_send_email();
bool ios_gc_show_achievements();
void ios_report_achievement(const char * identifier, float percent, bool show_banner);
void ios_tapjoy_openofferwall();

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
void mac_inapps_init();
bool mac_buy_product(const char * store_id);
void mac_restore_purchases();
void mac_gc_init();
bool mac_gc_show_leaderboard(const char * leaderboard);
void mac_gc_submit_score(const char * cat, unsigned int value);
void mac_open_url(const char * url);
void mac_start_yes_no_dialog(const std::string& message, const std::string& title, const std::string& yes_text, const std::string& no_text);
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
bool android_device_can_take_photo();
bool android_take_photo();
void android_start_yes_no_dialog(const std::string& message, const std::string& title, const std::string& yes_text, const std::string& no_text);
void android_share_facebook_score(const char * message);
bool android_share_image_facebook(cocos2d::CCImage * image, const char * message);
bool android_save_image_to_photos(cocos2d::CCImage * image);
void android_share_twitter (const char * message);
void android_facebook_post_wall(const char * message, const char * url, const char * title, const char * subtitle, const char * description, const char * picture_url);
void android_inapps_init();
bool android_buy_product(const char * store_id);
bool android_kill_product(const char * store_id);
void android_restore_purchases();
void android_share_email(const sk::game_services::email_info& info);
bool android_can_send_email();
bool android_gc_show_leaderboard(const char * leaderboard);
void android_gc_submit_score(const char * cat, unsigned int value);
bool android_gc_show_achievements();
void android_report_achievement(const char * identifier, float percent, bool show_banner);
void android_show_cb();
void android_show_more_games();
void android_show_ads(bool isVisible);
void android_open_url(const char * url);
void android_on_rate_me();
void android_open_offer_wall();
void android_log_event(const char *);
void android_gameservices_init();
void android_show_video_ad(bool reward, bool show_fail_dialog);
void android_facebook_open_page(const char* page_id);
void android_facebook_check_page_like(const char* page_id);
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
void bb_open_url(const char * url);
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#include "platform/wp8/inapp/InAppController.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#include "platform/winrt/interop/Win_WebTask.h"
#endif

using namespace cocos2d;

namespace sk {

namespace game_services {

static const char* LIKE_KEY = "vfGhyuj";

static bool APP_IS_GOING_TO_BG = false;
static bool PAUSED = false;
static init_info info;

static bool CB_SHOWN = false;
static bool RM_SHOWN = false;
#ifndef SK_NO_PLAYHEAVEN
	static bool PH_SHOWN = false;
#endif

static bool offer_opened = false;
static image_observer * _image_observer = 0;
static ui_dialog_observer * _dialog_observer = 0;
static unsigned int _dialog_id = 0;
static image_info _image_info;
	
template<class T>
struct observers_pool
{
	std::vector<T *> observers;
	
	int get_observer_index(T * observer)
	{
		for (size_t i = 0; i < observers.size(); ++i)
		{
			if (observer == observers[i])
			{
				return i;
			}
		}
		return -1;
	}
	
	void add(T * observer)
	{
		int index = get_observer_index(observer);
		if (-1 != index)
		{
			return;
		}
		observers.push_back(observer);
	}
	
	void remove(T * observer)
	{
		int index = get_observer_index(observer);
		if (-1 == index)
		{
			return;
		}
		if (index != observers.size() - 1)
		{
			observers[index] = observers.back();
		}
		observers.pop_back();
	}
};

	
static observers_pool<ui_purchase_observer>  _purchase_observers;
static observers_pool<ui_offer_observer>     _offer_observers;
static observers_pool<ui_video_ad_observer>  _video_ad_observers;
static observers_pool<facebook_check_page_like_observer> _facebook_check_page_like_observers;

void register_offer_wall_observer(ui_offer_observer * observer)
{
	_offer_observers.add(observer);
}
	
void unregister_offer_wall_observer(ui_offer_observer * observer)
{
	_offer_observers.remove(observer);
}
	
void register_video_ad_observer(ui_video_ad_observer * observer)
{
	_video_ad_observers.add(observer);
}
	

void unregister_video_ad_observer(ui_video_ad_observer * observer)
{
	_video_ad_observers.remove(observer);
}
	
void on_video_ad_award(int amount)
{
	for (int i = 0; i < _video_ad_observers.observers.size(); ++i)
	{
		_video_ad_observers.observers[i]-> ui_on_video_ad_award(amount);
	}
}
	
void on_offer_earned(int amount)
{
	for (int i = 0; i < _offer_observers.observers.size(); ++i)
	{
		_offer_observers.observers[i]->ui_on_currency_earned(amount);
	}
}

void register_ui_purchase_observer(ui_purchase_observer * observer)
{
	if (are_in_apps_disabled())
	{
		return;
	}
	_purchase_observers.add(observer);
}

void unregister_ui_purchase_observer(ui_purchase_observer * observer)
{
	if (are_in_apps_disabled())
	{
		return;
	}
	_purchase_observers.remove(observer);
}

void facebook_register_check_like_observer(facebook_check_page_like_observer* observer)
{
	_facebook_check_page_like_observers.add(observer);
}

void facebook_unregister_check_like_observer(facebook_check_page_like_observer* observer)
{
	_facebook_check_page_like_observers.remove(observer);
}

void facebook_check_page_like_callback(bool isLiked)
{
	CCUserDefault::sharedUserDefault()->setBoolForKey(LIKE_KEY, isLiked);
	CCUserDefault::sharedUserDefault()->flush();

	for (unsigned int i = 0; i < _facebook_check_page_like_observers.observers.size(); ++i)
	{
		_facebook_check_page_like_observers.observers[i]->ui_on_facebook_liked(isLiked);
	}
}

//check, if there is like on page from current facebook user
void facebook_check_page_like(const char* page_id)
{
	#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		android_facebook_check_page_like(page_id);
	#endif
}

//open page for like
//pageId - id of page in Graph API
//for example, for page https://www.facebook.com/SpaceFlipGame there is pageId = '512348648783389'
void facebook_open_game_page(const char* page_id)
{
	#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID		
		android_facebook_open_page(page_id);
	#endif
}

//if user has already liked page. see in the local storage
bool facebook_is_like_game_page_local()
{
	bool isHasLike = CCUserDefault::sharedUserDefault()->getBoolForKey(LIKE_KEY, false);
	return isHasLike;
}

	
bool IS_JAILBROKEN()
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
	return false;
#endif	
	FILE *f = fopen("/bin/bash", "r");
	if (f != NULL)
	{
		fclose(f);
		return true;
	}
	fclose(f);
	return false;
}
	
bool open_offer_wall()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (offer_opened)
	{
		//return false;
	}
	//offer_opened = true;
	android_open_offer_wall();
	return true;
#endif
	return false;
}
	
void on_offer_wall_closed()
{
	offer_opened = false;
}

bool app_is_going_to_bg() {
	return APP_IS_GOING_TO_BG;
}

void set_app_is_goint_to_bg(bool to_bg) {
	APP_IS_GOING_TO_BG = to_bg;
}

void set_cb_shown(bool shown) {
	CB_SHOWN = shown;
}

void set_revmob_shown(bool shown) {
	RM_SHOWN = shown;
}

#ifndef SK_NO_PLAYHEAVEN
void set_ph_shown(bool shown) {
	PH_SHOWN = shown;
}
#endif

bool is_interstitial_shown() {
	return CB_SHOWN || RM_SHOWN
#ifndef SK_NO_PLAYHEAVEN
	|| PH_SHOWN
#endif
	;
}

bool is_paused() {
	return PAUSED;
}

void set_paused(bool paused) {
	if (paused == PAUSED) {
		return;
	}
	PAUSED = paused;

	if (PAUSED) {
		game_do_pause();
	} else {
		game_do_resume();
	}
}
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	
void on_android_adview_height_changed(int height)
{
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	d->setIntegerForKey("__android__banner_height___", height);
	d->flush();
}
	
#endif

std::string GetAppStoreName()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
	return "Mac App Store";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_MAC
	return "App Store";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	#if defined(PLATFORM_ANDROID_AMAZON)
		return "Amazon Appstore";
	#else
		return "Google Play Store";
	#endif
#else
	return "";
#endif
}

int get_banner_height()
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	int W = (int) size.width;
	int H = (int) size.height;

	int m = W > H ? W : H;
	

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (m == 480)
	{
		return 50;
	}
	if (m == 960 || m == 1136)
	{
		return 100;
	}
	if (m == 2048)
	{
		return 90 * 2;
	}
	return 90;
#endif	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	int h = d->getIntegerForKey("__android__banner_height___", 0);
	if (h)
	{
		return h;
	}	
	int _m = W < H ? W : H;
	if (480 == _m)
	{
		return 75;
	}
	if (m < 720)
	{
		return 50;
	}
	if (m > 1400)
	{
		return 90 * 2;
	}
	else
	{
		return 90;
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
	return 90;
#else
	return 50;
#endif
}

void share_game_score_facebook(const char * message) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_share_facebook(message);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_share_facebook_score(message);
#endif
}

void share_game_score_twitter(const char * message) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_share_twitter(message);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_share_twitter(message);
#endif
}

void share_image_facebook(cocos2d::CCImage * image, const char * message) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_share_image_facebook(image, message);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_share_image_facebook(image, message);
#endif
}

void save_image_to_photos(cocos2d::CCImage * image) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_save_image_to_photos(image);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_save_image_to_photos(image);
#endif
}

void send_email(const email_info& info) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_share_email(info);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_share_email(info);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	WebTask::winrt_share_email(info);
#endif
}

bool can_send_email() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return ios_can_send_email();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return android_can_send_email();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	return WebTask::winrt_can_send_email();
#endif
	return false;
}

void init(const init_info& _info)
{
	// a bit dirty, all strings in _info must be compile time constant strings !!!
	info = _info;
	
	if (!sk::game_services::is_game_center_disabled())
	{
		if (info.leaderboards.size() > 0 || info.achievements.size() > 0) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			ios_gc_init();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
			mac_gc_init();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			android_gameservices_init();
#endif
		}
	}
	
	if (!are_in_apps_disabled())
	{
		if (info.inapps.size() > 0)
		{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			ios_inapps_init();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
			mac_inapps_init();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			android_inapps_init();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
			InAppController::InitInApps();
#endif
		}
	}
}
	
bool is_offer_wall_enabled()
{
	if (disable_offer_wall())
	{
		return false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return true;
#endif
	return false;
}
	
bool is_video_ad_enabled()
{
	if (disable_video_ad())
	{
		return false;
	}
#if  (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return true;
#endif
	return false;
}

void init_info::add_leaderboard(int game_id, const char * store_id)
{
	CCAssert(game_id == leaderboards.size(),
			"Leaderboard ids must be consequtive integers, e.g. 0, 1, 2, etc.");
	leaderboards.push_back(leaderboard());
	leaderboards.back().game_id = game_id;
	leaderboards.back().store_id = std::string(store_id);
}
	
void init_info::add_achievement(int game_id, const char * store_id)
{
	CCAssert(game_id == achievements.size(),
			 "Achievement ids must be consequtive integers, e.g. 0, 1, 2, etc.");
	achievements.push_back(achievement());
	achievements.back().game_id = game_id;
	achievements.back().store_id = std::string(store_id);
}

void init_info::add_inapp(int game_id, const char * store_id,
		game_logic_purchase_callback logic, bool consumable) {
	if (are_in_apps_disabled())
	{
		return;
	}
	CCAssert(game_id == inapps.size(),
			"In-App ids must be consequtive integers, e.g. 0, 1, 2, etc.");
	CCAssert(logic, "In-App must be valid function pointer");
	inapps.push_back(inapp());
	inapps.back().game_id = game_id;
	inapps.back().store_id = std::string(store_id);
	inapps.back().confiremd_by_store = false;
	inapps.back().purchased = false;
	inapps.back().consumable = consumable;
	inapps.back().logic_on_purchase = logic;
}

int get_total_inapps()
{
	if (are_in_apps_disabled())
	{
		return 0;
	}
	return info.inapps.size();
}

const char * get_inapp_store_id(int index)
{
	if (are_in_apps_disabled())
	{
		return 0;
	}
	if (index < 0 || index >= info.inapps.size()) {
		CCLOG("In-App Purchases : get_inapp_store_id - incorrect index : %d",
				index);
		return 0;
	}
	return info.inapps[index].store_id.c_str();
}

int get_index_for_in_app(const char * store_id)
{
	if (are_in_apps_disabled())
	{
		return -1;
	}
	std::string s(store_id);
	for (size_t i = 0; i < info.inapps.size(); ++i) {
		if (info.inapps[i].store_id == s) {
			return i;
		}
	}
	return -1;
}



void on_inapp_confirmed_by_store(const char * store_id) {
	if (are_in_apps_disabled())
	{
		return;
	}
	int index = get_index_for_in_app(store_id);
	if (-1 == index) {
		CCLOG(
				"In-App Purchases : ATTENTION : product %s exists in the application store, but NOT in the app!",
				store_id);
		return;
	}
	info.inapps[index].confiremd_by_store = true;
	CCLOG("In-App Purchases : product %s confirmed by application store",
			store_id);
}

bool buy_product(unsigned int game_id) {
	if (are_in_apps_disabled())
	{
		return false;
	}
    
	if (game_id >= info.inapps.size()) {
		CCLOG(
				"In-App Purchases : buy_product - incorrect game_id : %d. Check you ids enum declaration and in-apps initialization code.",
				game_id);
		return false;
	}

	if (!info.inapps[game_id].confiremd_by_store) {
		CCLOG(
				"In-App Purchases : buy_product - product %s is NOT confirmed by application store. Check you store account and initialization code.",
				info.inapps[game_id].store_id.c_str());
		return false;
	}
	const char * store_id = info.inapps[game_id].store_id.c_str();
	if (!store_id)
	{
		return false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return ios_buy_product(store_id);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	return mac_buy_product(store_id);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return android_buy_product(store_id);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	return InAppController::BuyProduct(store_id);
#endif

	return false;
}
	
bool on_product_consumed(unsigned int game_id)
{
	if (are_in_apps_disabled())
	{
		return false;
	}
	if (game_id >= info.inapps.size()) {
		CCLOG(
			  "In-App Purchases : on_product_consumed - incorrect game_id : %d. Check you ids enum declaration and in-apps initialization code.",
			  game_id);
		return false;
	}
	
	if (!info.inapps[game_id].confiremd_by_store) {
		CCLOG(
			  "In-App Purchases : on_product_consumed - product %s is NOT confirmed by application store. Check you store account and initialization code.",
			  info.inapps[game_id].store_id.c_str());
		return false;
	}
	const char * store_id = info.inapps[game_id].store_id.c_str();
	if (!store_id)
	{
		return false;
	}

	return false;
}
	
bool is_inapp_consumable(const char * store_id)
{
	int index = get_index_for_in_app(store_id);
	if (-1 == index)
	{
		CCLOG(
			  "In-App Purchases : is_inapp_consumable : ATTENTION : product %s exists in the application store, but NOT in the app!",
			  store_id);
		return false;
	}	
	return info.inapps[index].consumable;
}

void on_purchase(const char * store_id, bool purchased)
{
	if (are_in_apps_disabled())
	{
		return;
	}
	int index = get_index_for_in_app(store_id);
	if (-1 == index)
	{
		CCLOG(
				"In-App Purchases : ATTENTION : product %s exists in the application store, but NOT in the app!",
				store_id);
		return;
	}

	if (purchased)
	{
		CCLOG("In-App Purchases : purchase succesful : %s", store_id);
	}
	else
	{
		CCLOG("In-App Purchases : purchase failed : %s", store_id);
	}

	if (purchased)
	{
		info.inapps[index].purchased = true;
		if (info.inapps[index].logic_on_purchase)
		{
			info.inapps[index].logic_on_purchase();
		}
	}

	for (int i = 0; i < _purchase_observers.observers.size(); ++i)
	{
		_purchase_observers.observers[i]->ui_on_purchase(index, purchased);
	}
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (info.inapps[index].consumable)
	{
		android_kill_product(store_id);
	}
#endif
}

bool show_leaderboard(int leaderboars_id)
{
	if (is_game_center_disabled())
	{
		return false;
	}
	const char * l = get_leaderboard_store_id(leaderboars_id);
	if (!l) {
		return false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return ios_gc_show_leaderboard(l);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	return mac_gc_show_leaderboard(l);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return android_gc_show_leaderboard(l);
#endif
	return false;
}
	
	
bool show_achievements()
{
	if (is_game_center_disabled())
	{
		return false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return ios_gc_show_achievements();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return android_gc_show_achievements();
#endif
	return false;
}
	
bool gamecenter_pressed()
{
	if (is_game_center_disabled())
	{
		return false;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return ios_gc_show_leaderboard(get_leaderboard_store_id(0));
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)	
	return android_gc_show_achievements();
#endif
	return false;
}
		
static float get_achievement_percent(unsigned int achievement_id)
{
	if (achievement_id >= info.achievements.size())
	{
		return 0;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "qmol%d", achievement_id);
	return d->getFloatForKey(key, 0);
}
	
static float get_achievement_reported_percent(unsigned int achievement_id)
{
	if (achievement_id >= info.achievements.size())
	{
		return 0;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "qmolr%d", achievement_id);
	return d->getFloatForKey(key, 0);
}
	
static void set_achievement_percent(unsigned int achievement_id, float percent)
{
	if (achievement_id >= info.achievements.size())
	{
		return;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "qmol%d", achievement_id);
	d->setFloatForKey(key, percent);
	d->flush();
}
	
static void set_achievement_reported_percent(unsigned int achievement_id, float percent)
{
	if (achievement_id >= info.achievements.size())
	{
		return;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "qmolr%d", achievement_id);
	d->setFloatForKey(key, percent);
	d->flush();
}
	
static int get_leaderboard_score(unsigned int leaderboard_id)
{
	if (leaderboard_id >= info.leaderboards.size())
	{
		return 0;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "sqmol%d", leaderboard_id);
	return d->getIntegerForKey(key, 0);
}

static int get_leaderboard_reported_score(unsigned int leaderboard_id)
{
	if (leaderboard_id >= info.leaderboards.size())
	{
		return 0;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "sqmolr%d", leaderboard_id);
	return d->getIntegerForKey(key, 0);
}

static void set_leaderboard_score(unsigned int leaderboard_id, unsigned int score)
{
	if (leaderboard_id >= info.leaderboards.size())
	{
		return;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "sqmol%d", leaderboard_id);
	d->setIntegerForKey(key, score);
	d->flush();
}

static void set_leaderboard_reported_score(unsigned int leaderboard_id, unsigned int score)
{
	if (leaderboard_id >= info.leaderboards.size())
	{
		return;
	}
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	char key[32];
	sprintf(key, "sqmolr%d", leaderboard_id);
	d->setIntegerForKey(key, score);
	d->flush();
}
	
static void do_report_achievement(const char * a, float percent, bool showBanner)
{
	if (is_game_center_disabled())
	{
		return;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_report_achievement(a, percent, showBanner);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_report_achievement(a, percent, showBanner);
#endif
}
	
bool report_achievement(int achievent_id, float percent)
{
	const char * a = get_achievement_store_id(achievent_id);
	if (!a)
	{
		return false;
	}
	float p = get_achievement_percent(achievent_id);
	bool showBanner = true;
	if (p >= percent)
	{
		CCLOG("achievment %s already has percent that is >= %f", a, percent);
		percent = p;
		float rp = get_achievement_reported_percent(achievent_id);
		if (percent > rp)
		{
			// try to report
			showBanner = false;
			CCLOG("achievment %s was not reported, will retry report it again", a);
		}
		else
		{
			return false;
		}
	}
	set_achievement_percent(achievent_id, percent);
	do_report_achievement(a, percent, showBanner);
	return true;
}

void on_achievement_reported(const char * achievement_store_id, float percent)
{
	unsigned int achievement_id = get_achievement_game_id(achievement_store_id);
	if (-1 == achievement_id)
	{
		return;
	}
	CCLOG("achievement report %s confirmed by score", achievement_store_id);
	set_achievement_reported_percent(achievement_id, percent);
}

void on_leaderboard_score_submitted(const char * leaderboard_store_id, int score)
{
	unsigned int leaderboard_id = get_leaderboard_game_id(leaderboard_store_id);
	if (-1 == leaderboard_id)
	{
		return;
	}
	CCLOG("leaderboard score %s confirmed by score", leaderboard_store_id);
	set_leaderboard_reported_score(leaderboard_id, score);
}
	
static void do_submit_store(const char * l, int score)
{
	if (is_game_center_disabled())
	{
		return;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_gc_submit_score(l, score);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	mac_gc_submit_score(l, score);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_gc_submit_score(l, score);
#endif
}
	
bool submit_score(int leaderboars_id, unsigned int score)
{
	const char * l = get_leaderboard_store_id(leaderboars_id);
	if (!l)
	{
		return false;
	}
	int s = get_leaderboard_score(leaderboars_id);
	if (s >= score)
	{
		CCLOG("leaderboard %s already has score that is >= %d", l, score);
		score = s;
		int rs = get_leaderboard_reported_score(leaderboars_id);
		if (score > rs)
		{
			CCLOG("score %s was not submitted, will retry submit it again", l);
		}
		else
		{
			return false;
		}
	}
	set_leaderboard_score(leaderboars_id, score);
	do_submit_store(l, score);
	return true;
}
	

	
void on_gamecenter_auth()
{	
	CCLOG("sk::game_services::on_gamecenter_auth() - player authenticated :)");
	for (int i = 0; i < info.achievements.size(); ++i)
	{
		unsigned int a_id = info.achievements[i].game_id;
		float percent  = get_achievement_percent(a_id);
		float rp = get_achievement_reported_percent(a_id);
		if (rp < percent)
		{
			const char * a = info.achievements[i].store_id.c_str();
			do_report_achievement(a, percent, false);
		}
	}
	
	for (int i = 0; i < info.leaderboards.size(); ++i)
	{
		unsigned int l_id = info.leaderboards[i].game_id;
		float score  = get_leaderboard_score(l_id);
		float rs = get_leaderboard_reported_score(l_id);
		if (rs < score)
		{
			const char * l = info.leaderboards[i].store_id.c_str();
			do_submit_store(l, score);
		}
	}
}
	
void on_gamecenter_auth_failed()
{
	CCLOG("sk::game_services::on_gamecenter_auth_failed() - player NOT authenticated :)");
}
		
const char * get_leaderboard_store_id(unsigned int leaderboard_id)
{
	for (size_t i = 0; i < info.leaderboards.size(); ++i)
	{
		if (info.leaderboards[i].game_id == leaderboard_id)
		{
			return info.leaderboards[i].store_id.c_str();
		}
	}
	return 0;
}

const char * get_achievement_store_id(unsigned int achievement_id)
{
	for (size_t i = 0; i < info.achievements.size(); ++i)
	{
		if (info.achievements[i].game_id == achievement_id)
		{
			return info.achievements[i].store_id.c_str();
		}
	}
	return 0;
}

unsigned int get_achievement_game_id(const char * _store_id)
{
	std::string achievement_id(_store_id);
	for (size_t i = 0; i < info.achievements.size(); ++i)
	{
		if (info.achievements[i].store_id == achievement_id)
		{
			return info.achievements[i].game_id;
		}
	}
	return -1;
}
	
unsigned int get_leaderboard_game_id(const char * _store_id)
{
	std::string leaderboard_id(_store_id);
	for (size_t i = 0; i < info.leaderboards.size(); ++i)
	{
		if (info.leaderboards[i].store_id == leaderboard_id)
		{
			return info.leaderboards[i].game_id;
		}
	}
	return -1;
}
	
const char * get_app_store_url()
{
	return info.app_store_url.c_str();
}

void on_ads_removed()
{
	log_event("on_ads_removed");
	CCUserDefault::sharedUserDefault()->setBoolForKey("qiur3df1e3", true);
	CCUserDefault::sharedUserDefault()->flush();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_on_ads_remove();
#endif	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_ads(false);
#endif
}

void show_ads(bool isVisible)
{
	if (disable_banner_ads())
		return;

	if (isVisible && is_ads_removed())
		return;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_ads(isVisible);
#endif
}

bool is_ads_removed()
{
	if (is_game_paid())
	{
		return true;
	}
	return CCUserDefault::sharedUserDefault()->getBoolForKey("qiur3df1e3", false);
}

void on_unlock_all()
{
	if (is_all_unlocked())
	{
		return;
	}
	log_event("on_unlock_all");
	CCUserDefault::sharedUserDefault()->setBoolForKey("qiur3df1e", true);
	CCUserDefault::sharedUserDefault()->flush();
}

void clear_unlock_all()
{
	CCUserDefault::sharedUserDefault()->setBoolForKey("qiur3df1e", false);
	CCUserDefault::sharedUserDefault()->flush();
}

bool is_all_unlocked()
{
	return CCUserDefault::sharedUserDefault()->getBoolForKey("qiur3df1e", false);
}

void show_more_games() {
	log_event("show_more_games");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)	
	ios_show_more_games();
#endif	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_more_games();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
	open_url("https://itunes.apple.com/us/artist/draco-llc/id581754327?mt=12");
#endif
}
    
void show_cb_ads()
{
    log_event("show_cb_ads");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_show_cb();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_cb();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
	
#endif
}
    
void show_revmob()
{
    log_event("show_revmob_ads");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_show_revmob();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_revmob();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
	
#endif
}
    
void show_revmob_banner(bool shown)
{
    log_event("show_revmob_banner_ads");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_show_revmob_banner(shown);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_revmob();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
	
#endif
}
    
void log_event(const char * event) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)		
	ios_log_event(event);
#endif		
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_log_event(event);
#endif
}

void on_level_ended()
{
	log_event("level_ended");
	if (is_game_paid()) {
		return;
	}
	if (is_ads_removed()) {
		return;
	}
	static int num_played = 0;
	if (num_played == levels_per_interstitial())
	{
		num_played = 0;
	}
	else
	{
		++num_played;
		return;
	}
	
	static int ad_type = 0;
	static int num_video_ad_per_interstitial = 3; // TODO : we can add function later
	
	if (ad_type == num_video_ad_per_interstitial)
	{
		ad_type = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		android_show_cb();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		srand(time(NULL));
        int t = rand() % 7;
        if (t == 0)
        {
            ios_show_revmob();
        }
        else
        {
            ios_show_cb();
        }
#endif
	}
	else
	{
		++ad_type;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		android_show_video_ad(true, false);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		//ios_tapjoy_openofferwall();
		show_video_ad(true, false);
#endif
	}
}

void show_video_ad(bool respect_ads_removed, bool show_fail_dialog) {
	if (respect_ads_removed && is_ads_removed()) {
		return;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#ifndef SK_NO_ADCOLONY
	ios_show_video_ad(true);
#endif
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_video_ad(true, show_fail_dialog);
#endif	
}

void restore_purchases() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_restore_purchases();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	mac_restore_purchases();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_restore_purchases();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	InAppController::RestoreInApps();
#endif
}

void on_tv_pressed() {
	show_video_ad(false);
}

void on_gift_pressed() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_show_revmob();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	show_more_games();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_cb();
#endif
}
	
int num_launched()
{
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	return d->getIntegerForKey(LAUNCH_KEY, 0);
}

void on_launch() {
	CCUserDefault * d = CCUserDefault::sharedUserDefault();
	int n = d->getIntegerForKey(LAUNCH_KEY, 0);
	d->setIntegerForKey(LAUNCH_KEY, n + 1);
	d->flush();
	if (is_ads_removed()) {
		return;
	}
	if (is_game_paid()) {
		return;
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//ios_show_revmob();
	//ios_show_ph();
	//ios_show_cb();
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_show_cb();
#endif	
}

void open_url(const char * url) {
	if (!url)
	{
		return;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_open_url(url);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	mac_open_url(url);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_open_url(url);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
	bb_open_url(url);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	WebTask::winrt_open_url(url);
#endif
}

void on_rate_me_pressed() {
	log_event("rate_me_pressed");
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_on_rate_me();
#else
	open_url(get_app_store_url());
#endif
}

bool device_can_take_photo() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return ios_device_can_take_photo();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return android_device_can_take_photo();
#endif

	return false;
}

const image_info& get_image_info() {
	return _image_info;
}

bool get_image(image_observer * observer, const image_info& info) {
	if (_image_observer) {
		return false;
	}
	if (ImageSourceUndefined == info.source) {
		return false;
	}
	if (ImageSourceCamera == info.source && !device_can_take_photo()) {
		return false;
	}
	_image_info = info;
	_image_observer = observer;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)	 
	ios_take_image();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_take_photo();
#endif
	return true;
}

void on_take_image(cocos2d::CCImage * photo) {
	if (_image_observer) {
		_image_observer->on_get_image(photo, get_image_info().source);
	} else {
		if (photo) {
			delete photo;
		}
	}
	_image_observer = 0;
}

bool open_yes_no_dialog(unsigned int dialog_id, ui_dialog_observer * observer,
		const std::string& message, const std::string& title,
		const std::string& yes_text, const std::string& no_text) {
	if (_dialog_observer) {
		return false;
	}
	_dialog_id = dialog_id;
	_dialog_observer = observer;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ios_start_yes_no_dialog(message, title, yes_text, no_text);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	mac_start_yes_no_dialog(message, title, yes_text, no_text);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	android_start_yes_no_dialog(message, title, yes_text, no_text);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN)
	//attention: synchronous call
	bool isYes = CCApplication::ShowYesNoDialog(message.c_str(), title.c_str());
	sk::game_services::set_yes_no_dialog_result(isYes);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//attention: synchronous call
	int result = MessageBoxA(CCEGLView::sharedOpenGLView()->getHWnd(), message.c_str(), title.c_str(), MB_YESNO);
	bool isYes = result == IDYES;
	sk::game_services::set_yes_no_dialog_result(isYes);
#endif
	return true;
}

void set_yes_no_dialog_result(bool result) {
	if (_dialog_observer) {
		_dialog_observer->ui_on_yes_no_dialog(_dialog_id, result);
	}
	_dialog_id = 0;
	_dialog_observer = 0;
}

void set_music_on(bool on) {
	CCUserDefault::sharedUserDefault()->setIntegerForKey("sk_is_music_on",
			on ? 1 : 0);
	CCUserDefault::sharedUserDefault()->flush();
}

void set_sounds_on(bool on) {
	CCUserDefault::sharedUserDefault()->setIntegerForKey("sk_is_sounds_on",
			on ? 1 : 0);
	CCUserDefault::sharedUserDefault()->flush();
}

bool is_music_on() {
	return 0
			!= CCUserDefault::sharedUserDefault()->getIntegerForKey(
					"sk_is_music_on", 1);
}

bool is_sounds_on() {
	return 0
			!= CCUserDefault::sharedUserDefault()->getIntegerForKey(
					"sk_is_sounds_on", 1);
}
	
bool is_portrait()
{
	return !is_landscape();
}
	
bool is_landscape()
{
	return SO_LANDSCAPE_ONLY == get_supported_orientation();
}


//

#if CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT

	static GameServicesBase* _gameServices;

	void GameServicesBase::SetGameServices(GameServicesBase* gameServices)
	{
		CCAssert(_gameServices == NULL, "");
		_gameServices = gameServices;
	}

	void GameServicesBase::RemoveGameServices()
	{
		CC_SAFE_DELETE(_gameServices);
	}

	GameServicesBase* GameServicesBase::GetGameServices()
	{
		CCAssert(_gameServices != NULL, "");
		return _gameServices;
	}

	//

	bool are_in_apps_disabled()
	{
		return GameServicesBase::GetGameServices()->are_in_apps_disabled();
	}

	bool disable_offer_wall()
	{
		return GameServicesBase::GetGameServices()->disable_offer_wall();
	}
	
	bool disable_video_ad()
	{
		return GameServicesBase::GetGameServices()->disable_video_ad();
	}


	bool is_game_center_disabled()
	{
		return GameServicesBase::GetGameServices()->is_game_center_disabled();
	}

	bool is_game_paid()
	{
		return GameServicesBase::GetGameServices()->is_game_paid();
	}

	supported_orientations get_supported_orientation()
	{
		return GameServicesBase::GetGameServices()->get_supported_orientation();
	}

	int levels_per_interstitial()
	{
		return GameServicesBase::GetGameServices()->levels_per_interstitial();
	}

	bool disable_all_interstitials()
	{
		return GameServicesBase::GetGameServices()->disable_all_interstitials();
	}

	void game_do_resume()
	{
		GameServicesBase::GetGameServices()->game_do_resume();
	}

	void game_do_pause()
	{
		GameServicesBase::GetGameServices()->game_do_pause();
	}

	//

	bool disable_banner_ads()
	{
		return GameServicesBase::GetGameServices()->disable_banner_ads();
	}

	bool is_banner_ads_in_test_mode()
	{
		return GameServicesBase::GetGameServices()->is_banner_ads_in_test_mode();
	}

	bool disable_retina_ipad()
	{
		return GameServicesBase::GetGameServices()->disable_retina_ipad();
	}

	const char* get_admob_mediation_id()
	{
		return GameServicesBase::GetGameServices()->get_admob_mediation_id();
	}

	const char* facebook_app_id()
	{
		return GameServicesBase::GetGameServices()->facebook_app_id();
	}

#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
	int desktop_prefered_windowed_height()
	{
		return GameServicesBase::GetGameServices()->desktop_prefered_windowed_height();
	}
		
	const char* desktop_title()
	{
		return GameServicesBase::GetGameServices()->desktop_title();
	}
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
	mac_mouse_handlers get_mac_mouse_handler()
	{
		return GameServicesBase::GetGameServices()->get_mac_mouse_handler();
	}
#endif

	//
	const char* get_cb_app_id()
	{
		return GameServicesBase::GetGameServices()->get_cb_app_id();
	}

	const char* get_cb_app_signature()
	{
		return GameServicesBase::GetGameServices()->get_cb_app_signature();
	}

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID ||  CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	const char* get_flurry_app_key()
	{
		return GameServicesBase::GetGameServices()->get_flurry_app_key();
	}
	
	const char* get_flurry_ad_key()
	{
		return GameServicesBase::GetGameServices()->get_flurry_ad_key();
	}
#endif
	


	//ios
	const char* get_revmob_app_id()
	{
		return GameServicesBase::GetGameServices()->get_revmob_app_id();
	}
		
	const char* get_adcolony_app_id()
	{
		return GameServicesBase::GetGameServices()->get_adcolony_app_id();
	}
		
	const char* get_adcolony_zone()
	{
		return GameServicesBase::GetGameServices()->get_adcolony_zone();
	}
		
	const char* get_playheaven_token()
	{
		return GameServicesBase::GetGameServices()->get_playheaven_token();
	}
		
	const char* get_playheaven_secret()
	{
		return GameServicesBase::GetGameServices()->get_playheaven_secret();
	}

	const char* game_title()
	{
		return GameServicesBase::GetGameServices()->game_title();
	}

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	int get_offer_wall_provider()
    {
        return GameServicesBase::GetGameServices()->get_offer_wall_provider();
    }
        
	const char * tapjoy_app_id()
	{
		return GameServicesBase::GetGameServices()->tapjoy_app_id();
	}
		
	const char * tapjoy_app_key()
	{
		return GameServicesBase::GetGameServices()->tapjoy_app_key();
	}
	
//	const char * tapjoy_currency_id(int type)
//	{
//		return GameServicesBase::GetGameServices()->tapjoy_currency_id(type);
//	}
    
    const char * sponsorpay_app_id()
	{
		return GameServicesBase::GetGameServices()->sponsorpay_app_id();
	}
		
	const char * sponsorpay_app_key()
	{
		return GameServicesBase::GetGameServices()->sponsorpay_app_key();
	}
		
	android_purchase_mode get_android_purchase_mode()
	{
		return GameServicesBase::GetGameServices()->get_android_purchase_mode();
	}
		
	bool is_admob_in_test_mode()
	{
		return GameServicesBase::GetGameServices()->is_admob_in_test_mode();
	}
		
	const char * android_base64_encoded_public_key()
	{
		return GameServicesBase::GetGameServices()->android_base64_encoded_public_key();
	}
		
	const char * android_share_score_subtitle()
	{
		return GameServicesBase::GetGameServices()->android_share_score_subtitle();
	}
		
	const char * android_share_score_description()
	{
		return GameServicesBase::GetGameServices()->android_share_score_description();
	}

	const char * android_share_score_picture_url()
	{
		return GameServicesBase::GetGameServices()->android_share_score_picture_url();
	}
#endif



#endif

//


} // namespace game_services

} // namespace sk

#import "sk_game_services/sk_game_services.h"

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import <StoreKit/StoreKit.h>
#import "Chartboost.h"
#import <RevMobAds/RevMobAds.h>

#define SK_NO_TAPJOY

#ifndef SK_NO_PLAYHEAVEN
	#import "PlayHavenSDK.h"
	#import "PHAPIRequest.h"
#endif
#import <MessageUI/MessageUI.h>
#import <MessageUI/MFMailComposeViewController.h>

#ifndef SK_PAID
#ifdef SK_NO_ADMOB
	#import <iAd/iAd.h>
	#define IAD_BANNER_OFFSET 100
#else
	#import "GADBannerViewDelegate.h"
#endif // #ifdef SK_NO_ADMOB
#endif // #ifndef SK_PAID

#ifndef SK_NO_ADCOLONY
    #import <AdColony/AdColony.h>
#endif

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 60000
#define GC_DELEGATE_LIST GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GKGameCenterControllerDelegate
#else
#define GC_DELEGATE_LIST GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate
#endif


@class TapJoyHandler;


@interface RootViewController : UIViewController <

#ifndef SK_PAID
#ifdef SK_NO_ADMOB
	ADBannerViewDelegate,
#else
	GADBannerViewDelegate,
#endif // #ifdef SK_NO_ADMOB
#endif // #ifndef SK_PAID

GC_DELEGATE_LIST,
SKProductsRequestDelegate,
SKPaymentTransactionObserver,
RevMobAdsDelegate,
ChartboostDelegate,
#ifndef SK_NO_PLAYHEAVEN
	PHAPIRequestDelegate,
	PHPublisherContentRequestDelegate,
#endif
#ifndef SK_NO_ADCOLONY
	AdColonyDelegate,
    AdColonyAdDelegate,
#endif
UIImagePickerControllerDelegate,
UINavigationControllerDelegate,
UIAlertViewDelegate,
UIPopoverControllerDelegate,
MFMailComposeViewControllerDelegate
>
{
	bool                  gc_login_failed;
	bool                  gc_login_cancelled;
#ifndef SK_NO_PLAYHEAVEN
	PHNotificationView *_notificationView;
    PHPublisherContentRequest *_request;
#endif
}
#ifndef SK_NO_PLAYHEAVEN
	@property (nonatomic, retain) PHPublisherContentRequest *request;
#endif
@property (nonatomic, retain) NSArray* products;
@property (nonatomic, retain) SKProductsRequest * products_request;
#ifndef SK_NO_TAPJOY
@property (nonatomic, retain) TapJoyHandler* TapJoy;
#endif
- (void) inapps_init;
- (bool) buy_product:(const char *) product_id;

- (void) gc_init;
- (void) gc_show_achivements; // iOS 5
- (void) gc_achivements; // iOS 6+
- (bool) gc_auth;
- (bool) submit_score:(const char *) cat value:(unsigned int) value;
- (void) gc_leaderboard_show:(NSString *)category; // iOS 5
- (void) gc_show:(NSString*)category;              // iOS 6+
- (void) on_launch;
- (void) cb;
- (void) rm;
- (void) rm_baner:(bool)shown;
- (void) noad;
- (void) adc:(BOOL)reward;
- (void) more_games;
- (void) log_event:(const char *) event;
@end


#import "TapJoyHandler.h"
#import "RootViewController.h"
#include "sk_game_services/sk_game_services.h"
#import <Tapjoy/Tapjoy.h>


@implementation TapJoyHandler
{
    @private RootViewController* _rootViewController;
}


+(TapJoyHandler*)create:(RootViewController*)rootViewController
{
    TapJoyHandler* result = [[[TapJoyHandler alloc] init] autorelease];
    result->_rootViewController = rootViewController;
    return result;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];    
    
    [super dealloc];
}

-(void)connect
{
    if (!sk::game_services::is_offer_wall_enabled())
        return;
    
    //add notifications for application
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(applicationDidBecomeActive:)
          name:UIApplicationDidBecomeActiveNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(applicationWillResignActive:)
          name:UIApplicationWillResignActiveNotification object:nil];
    //
    
    //view notifications
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcViewClosed:)
          name:TJC_VIEW_CLOSED_NOTIFICATION object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcViewLoadingError:)
          name:TJC_VIEW_LOADING_ERROR_NOTIFICATION object:nil];
    
    //get tap points notification
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcGetTapPoints:)
          name:TJC_TAP_POINTS_RESPONSE_NOTIFICATION object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcGetTapPointsError:)
          name:TJC_TAP_POINTS_RESPONSE_NOTIFICATION_ERROR object:nil];

    //get full screen ad notification
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcGetFullScreenAd:)
          name:TJC_FULL_SCREEN_AD_RESPONSE_NOTIFICATION object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcGetFullScreenAdError:)
          name:TJC_FULL_SCREEN_AD_RESPONSE_NOTIFICATION_ERROR object:nil];

    
    // Tapjoy Connect Notifications
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcConnectSuccess:)
          name:TJC_CONNECT_SUCCESS object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
          selector:@selector(tjcConnectFail:)
          name:TJC_CONNECT_FAILED object:nil];
    //
    
    const char* appId = sk::game_services::tapjoy_app_id();
    const char* keyId = sk::game_services::tapjoy_app_key();
    
    [Tapjoy requestTapjoyConnect:[NSString stringWithUTF8String:appId]
					   secretKey:[NSString stringWithUTF8String:keyId]
						 options:@{ TJC_OPTION_ENABLE_LOGGING : @(YES) }];
    
    [Tapjoy getTapPoints];  //not need
}

- (void) openOfferWall
{
    sk::game_services::set_paused(true);
    [Tapjoy showOffersWithViewController:_rootViewController];
}

-(void) doShowTapjoyInterstitial
{
    [Tapjoy getFullScreenAd];
}

//-------------application callbacks-------------

- (void)applicationDidBecomeActive:(NSNotification*)notification
{
    if (!sk::game_services::is_offer_wall_enabled())
        return;
    
	// Add an observer for when a user has successfully earned currency.
	[[NSNotificationCenter defaultCenter] addObserver:self
			selector:@selector(showEarnedCurrencyAlert:)
			name:TJC_TAPPOINTS_EARNED_NOTIFICATION object:nil];
    
    [Tapjoy getTapPoints];
}

- (void)applicationWillResignActive:(NSNotification*)notification
{
    if (!sk::game_services::is_offer_wall_enabled())
        return;
    
	// Remove this to prevent the possibility of multiple redundant notifications.
	[[NSNotificationCenter defaultCenter] removeObserver:self
            name:TJC_TAPPOINTS_EARNED_NOTIFICATION object:nil];
}

//------------connect callbacks--------

-(void)tjcConnectSuccess:(NSNotification*)notification
{
	NSLog(@"Tapjoy connect Succeeded");
}

- (void)tjcConnectFail:(NSNotification*)notification
{
	NSLog(@"Tapjoy connect Failed");
}

//---- view notifications -----

- (void)tjcViewClosed:(NSNotification*)notification
{
    sk::game_services::on_offer_wall_closed();
  	sk::game_services::set_paused(false);
}

- (void)tjcViewLoadingError:(NSNotification*)notification
{
    NSLog(@"tjcViewLoadingError");
    sk::game_services::set_paused(false);
}

//---get tap points---

- (void)tjcGetTapPoints:(NSNotification*)notification
{
    NSLog(@"tjcGetTapPoints");
}

- (void)tjcGetTapPointsError:(NSNotification*)notification
{
    NSLog(@"tjcGetTapPointsError");
}

//--- get full screen ad----

- (void)tjcGetFullScreenAd:(NSNotification*)notification
{
    [Tapjoy showFullScreenAdWithViewController:_rootViewController];
    
    //android: tryFlurry
}

- (void)tjcGetFullScreenAdError:(NSNotification*)notification
{
    NSLog(@"tjcGetFullScreenAdError");
    
    //android: tryFlurry
}


//---TJC_TAPPOINTS_EARNED_NOTIFICATION----

- (void)showEarnedCurrencyAlert:(NSNotification*)notification
{
	NSNumber* tapPointsEarned = notification.object;
	int earnedNum = [tapPointsEarned intValue];
	
	NSLog(@"Currency earned: %d", earnedNum);
	
	// Pops up a UIAlert notifying the user that they have successfully earned some currency.
	// This is the default alert, so you may place a custom alert here if you choose to do so.
	[Tapjoy showDefaultEarnedCurrencyAlert];
    
    //
    sk::game_services::on_offer_earned(earnedNum);
}

@end

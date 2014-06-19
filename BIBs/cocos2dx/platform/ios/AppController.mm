//
//  iphoneAppDelegate.m
//  iphone
//
//  Created by Walzer on 10-11-16.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "AppController.h"
#import "EAGLView.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "RootViewController.h"
#import "SHKConfiguration.h"
#import "SHKFacebook.h"
#import "sk_game_services/sk_game_services.h"

@interface SK_SHKConfigurator : DefaultSHKConfigurator
{
}
@end

@implementation SK_SHKConfigurator
- (NSString*)facebookAppId
{
	return [NSString stringWithUTF8String:sk::game_services::facebook_app_id()];
}
@end


@implementation AppController

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	DefaultSHKConfigurator *configurator = [[[SK_SHKConfigurator alloc] init] autorelease];
	[SHKConfiguration sharedInstanceWithConfigurator:configurator];
    
    // Override point for customization after application launch.

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    
    // Init the EAGLView
    EAGLView *__glView = [EAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGB565
                                     depthFormat: GL_DEPTH24_STENCIL8_OES
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples: 0];
    [__glView setMultipleTouchEnabled:YES];

    // Use RootViewController manage EAGLView 
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    viewController.wantsFullScreenLayout = YES;
    viewController.view = __glView;

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:viewController];
    }
    
    [window makeKeyAndVisible];
    
    [[UIApplication sharedApplication] setStatusBarHidden:true];
    
    cocos2d::CCApplication::sharedApplication()->run();

    /// Chartboost init
    Chartboost *cb = [Chartboost sharedChartboost];
	
	cb.appId        = [NSString stringWithUTF8String:sk::game_services::get_cb_app_id()];
	cb.appSignature = [NSString stringWithUTF8String:sk::game_services::get_cb_app_signature()];
	
	cb.delegate = self;
	[cb startSession];
	[cb cacheInterstitial];
	[cb cacheMoreApps];
    /////
    
	[viewController on_launch];
	
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    cocos2d::CCDirector::sharedDirector()->pause();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    cocos2d::CCDirector::sharedDirector()->resume();
	[SHKFacebook handleDidBecomeActive];
	

}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    cocos2d::CCApplication::sharedApplication()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    cocos2d::CCApplication::sharedApplication()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
	[SHKFacebook handleWillTerminate];
}

- (BOOL)handleOpenURL:(NSURL*)url
{
    NSString* scheme = [url scheme];
    NSString* prefix = [NSString stringWithFormat:@"fb%@", SHKCONFIG(facebookAppId)];
    if ([scheme hasPrefix:prefix])
        return [SHKFacebook handleOpenURL:url];
    return YES;
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    return [self handleOpenURL:url];
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
    return [self handleOpenURL:url];
}

#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
	cocos2d::CCDirector::sharedDirector()->purgeCachedData();
}


- (void)dealloc {
    [window release];
    [super dealloc];
}

#pragma mark Chartboost delegate methods
// Called when an interstitial has been received and cached.
- (void)didCacheInterstitial:(NSString *)location
{
	NSLog(@"ChartBoost : cached interstitial %@", location);
}

// Called when the More Apps page has been received and cached
- (void)didCacheMoreApps
{
	NSLog(@"ChartBoost : didCacheMoreApps");
	[viewController cb_cached_more_games_true];
}

// Called when an interstitial has been received, before it is presented on screen
// Return NO if showing an interstitial is currently innapropriate, for example if the user has entered the main game mode.
- (BOOL)shouldDisplayInterstitial:(NSString *)location
{
	NSLog(@"ChartBoost : will dispaly interstitial now %@", location);
	sk::game_services::set_cb_shown(true);
	sk::game_services::set_paused(true);
	sk::game_services::log_event("chartboost_interstitial_will_show");
	// SKTODO : make sure you do not call this gameplay ???
	return YES; // !!!! YES
}


- (void)didClickInterstitial:(NSString *)location
{
	NSLog(@"ChartBoost : didClickInterstitial now %@", location);
	sk::game_services::set_cb_shown(false);
	sk::game_services::set_paused(false);
	sk::game_services::log_event("chartboost_interstitial_did_click");
}
// Same as above, but only called when dismissed for a close
- (void)didCloseInterstitial:(NSString *)location
{
	NSLog(@"ChartBoost : didCloseInterstitial now %@", location);
	sk::game_services::set_cb_shown(false);
	sk::game_services::set_paused(false);
	sk::game_services::log_event("chartboost_closed_interstitials");
}

- (BOOL)shouldDisplayMoreApps
{
	NSLog(@"ChartBoost : shouldDisplayMoreApps");
	sk::game_services::set_cb_shown(true);
	sk::game_services::set_paused(true);
	sk::game_services::log_event("chartboost_more_apps_will_show");
	return YES;
}

- (void)didCloseMoreApps
{
	NSLog(@"ChartBoost : didCloseMoreApps");
	sk::game_services::set_cb_shown(false);
	sk::game_services::set_paused(false);
	sk::game_services::log_event("chartboost_closed_more_apps");
}

- (void)didClickMoreApps
{
	NSLog(@"ChartBoost : didClickMoreApps");
	sk::game_services::set_cb_shown(false);
	sk::game_services::set_paused(false);
	sk::game_services::log_event("chartboost_click_more_apps");
}



@end

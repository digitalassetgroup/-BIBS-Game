#import "RootViewController.h"
#import "SHKFacebook.h"
#import "SHKTwitter.h"
#import "verification_controller.h"
#import "Flurry.h"
#import "TapJoyHandler.h"

#ifndef SK_PAID
#ifdef SK_NO_ADMOB
	#import <iAd/iAd.h>
	ADBannerView *adView;
	bool bannerIsVisible = false;
#else
	#import "GADAdSize.h"
	#import "GADBannerView.h"
	GADBannerView *bannerView_;
#endif // #ifdef SK_NO_ADMOB
#endif // #ifndef SK_PAID

#import "cocos2d.h"
#import "EAGLView.h"
static RootViewController * vc;
static bool cb_cached_more_games = false;
static UIImagePickerController * imagePicker = nil;
static UIPopoverController *popover;

bool      ios_init_ccimage_with_ui_image(cocos2d::CCImage * img, UIImage * uiimage);
UIImage * ios_init_ui_image_with_ccimage(cocos2d::CCImage * img, bool keep_alpha);

bool ios_device_can_take_photo()
{
	return [UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera];
}

void ios_take_image()
{
	if (sk::game_services::get_image_info().source == sk::game_services::ImageSourceCamera && !ios_device_can_take_photo())
	{
		return;
	}
	if (imagePicker == nil)
	{
		imagePicker = [[UIImagePickerController alloc] init];
		imagePicker.sourceType = sk::game_services::get_image_info().source == sk::game_services::ImageSourceCamera ? UIImagePickerControllerSourceTypeCamera : UIImagePickerControllerSourceTypePhotoLibrary;
		imagePicker.delegate = vc;
	}
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
	{
		[vc presentModalViewController:imagePicker animated:YES];
	}
	else
	{
		popover = [[UIPopoverController alloc] initWithContentViewController:imagePicker];
		EAGLView * view = (EAGLView*)[EAGLView sharedEGLView];
		popover.delegate = vc;
		// TODO : check it will work as expected in portrait mode
		CGRect rect = CGRectMake(view.frame.size.width/1.5, view.frame.size.height/2.5, 1, 1);
	    [popover presentPopoverFromRect:rect inView:view permittedArrowDirections:0 animated:YES];
	}

}

void ios_share_twitter(const char * message)
{
	NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:sk::game_services::get_app_store_url()]];
	SHKItem *item = [SHKItem URL:url title:[NSString stringWithUTF8String:message] contentType:SHKURLContentTypeWebpage];
	[SHKTwitter shareItem:item];
}

void ios_share_facebook (const char * message)
{
	NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:sk::game_services::get_app_store_url()]];
	SHKItem *item = [SHKItem URL:url title:[NSString stringWithUTF8String:message] contentType:SHKURLContentTypeWebpage];
	[SHKFacebook shareItem:item];
}

void ios_share_image_facebook(cocos2d::CCImage * img, const char * message)
{	
	UIImage * image = ios_init_ui_image_with_ccimage(img, false);
	SHKItem * item = [SHKItem image:image title:[NSString stringWithUTF8String:message]];
	[SHKFacebook shareItem:item];
	[image release];
}

void ios_save_image_to_photos(cocos2d::CCImage * img)
{
	UIImage * image = ios_init_ui_image_with_ccimage(img, false);
	UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil);
	[image release];
}

bool ios_can_send_email()
{
	Class mailClass = (NSClassFromString(@"MFMailComposeViewController"));
	return mailClass != nil && [mailClass canSendMail] == YES;
}

void ios_report_achievement(const char * identifier, float percent, bool show_banner)
{
	if (![GKLocalPlayer localPlayer].authenticated)
	{
		[vc gc_auth];
		return;
	}
	GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier:[NSString stringWithUTF8String:identifier]];
    if (achievement)
    {
		achievement.percentComplete = percent;
		achievement.showsCompletionBanner = show_banner ? YES : NO;
		[achievement reportAchievementWithCompletionHandler:^(NSError *error)
		 {
			 if (error != nil)
			 {
				 NSLog(@"Error in reporting achievements: %@", error);
			 }
			 else
			 {
				 sk::game_services::on_achievement_reported(identifier, percent);
			 }
		 }];
    }
}

void ios_share_email(const sk::game_services::email_info& info)
{
	if (!ios_can_send_email())
	{
		return;
	}
	MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
	picker.mailComposeDelegate = vc;
	
	if (info.subject.length())
	{
		[picker setSubject:[NSString stringWithUTF8String:info.subject.c_str()]];
	}
	
/*
	// Set up recipients

	NSArray *ccRecipients = [NSArray arrayWithObjects:@"second@example.com", @"third@example.com", nil];
	NSArray *bccRecipients = [NSArray arrayWithObject:@"fourth@example.com"];
	
	[picker setToRecipients:toRecipients];
	[picker setCcRecipients:ccRecipients];
	[picker setBccRecipients:bccRecipients];
*/ 
	if (info.to.length())
    {
        NSArray *toRecipients = [NSArray arrayWithObject:[NSString stringWithUTF8String:info.to.c_str()]];
        [picker setToRecipients:toRecipients];
    }
    
	if (info.photo)
	{
		UIImage * image = ios_init_ui_image_with_ccimage(info.photo, false);
		NSData *myData = UIImageJPEGRepresentation(image, 1.0);
		[picker addAttachmentData:myData mimeType:@"image/jpeg" fileName:@"photo"];
	}

	if (info.message.length())
	{
		NSString * message = [NSString stringWithUTF8String:info.message.c_str()];
		[picker setMessageBody:message isHTML:YES];
	}
	
	[vc presentModalViewController:picker animated:YES];
	[picker release];
}

void ios_show_video_ad( bool reward )
{
#ifndef SK_NO_ADCOLONY
	if (sk::game_services::is_interstitial_shown())
	{
		return;
	}
	[vc adc:reward];
#endif
}


void ios_show_cb()
{
	if (sk::game_services::is_interstitial_shown())
	{
		return;
	}
	[vc cb];
}

void ios_show_revmob()
{
	if (sk::game_services::is_interstitial_shown())
	{
		return;
	}
	[vc rm];
}

void ios_show_ph()
{
#ifndef SK_NO_PLAYHEAVEN
	if (sk::game_services::is_interstitial_shown())
	{
		return;
	}
	[vc ph];
#endif
}

void ios_gc_init()
{
	[vc gc_init];
}

void ios_on_ads_remove()
{
	[vc noad];
}

void ios_show_more_games()
{
	[vc more_games];
    //[vc shw_more_games];
}

void ios_log_event(const char * event)
{
	[vc log_event:event];
}

void ios_open_url(const char * url)
{
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

void ios_gc_submit_score(const char * cat, unsigned int value)
{
	[vc submit_score:cat value:value];
}

namespace sk
{
namespace game_services
{
	bool ios_gamecenter_is_combined()
	{
		return [[[UIDevice currentDevice] systemVersion] floatValue] >= 6.0f;
	}
	
}
}

bool ios_gc_show_leaderboard(const char * leaderboard)
{
	if (![GKLocalPlayer localPlayer].authenticated)
	{
		[vc gc_auth];
		return false;
	}
	NSString * cat  = (leaderboard && leaderboard[0]) ? [NSString stringWithUTF8String:leaderboard] : nil;
	
	if (sk::game_services::ios_gamecenter_is_combined())
	{
		[vc gc_show:cat];
	}
	else
	{
		[vc gc_leaderboard_show:cat];
	}
	return true;
}

bool ios_gc_show_achievements()
{
	if (![GKLocalPlayer localPlayer].authenticated)
	{
		[vc gc_auth];
		return false;
	}
	if (sk::game_services::ios_gamecenter_is_combined())
	{
		[vc gc_achivements];
	}
	else
	{
		[vc gc_show_achivements];
	}
	return true;
}

void ios_inapps_init()
{
	[vc inapps_init];
}

bool ios_buy_product(const char * store_id)
{
	return [vc buy_product:store_id];
}

void ios_restore_purchases()
{
	[[SKPaymentQueue defaultQueue]restoreCompletedTransactions];
}

void ios_start_yes_no_dialog(const std::string& _message, const std::string& _title, const std::string& _yes_text, const std::string& _no_text)
{
	NSString *  message = [NSString stringWithUTF8String:_message.c_str()];
	NSString *  title   = [NSString stringWithUTF8String:_title.c_str()];
	NSString * yes_text = [NSString stringWithUTF8String:_yes_text.c_str()];
	NSString *  no_text = [NSString stringWithUTF8String:_no_text.c_str()];
	
	UIAlertView *alert = [[UIAlertView alloc]
						  
						  initWithTitle:title
						  message:message
						  delegate:vc
						  cancelButtonTitle:no_text
						  otherButtonTitles:yes_text, nil];
	alert.tag = sk::game_services::DIALOG_TYPE_YES_NO;
	[alert show];
	[alert release];
	
}

#ifndef SK_NO_TAPJOY

void ios_tapjoy_openofferwall()
{
    [vc.TapJoy openOfferWall];
}

void ios_tapjoy_show_interstitial()
{
	[vc.TapJoy s];
}
#endif



@implementation RootViewController

@synthesize products;
@synthesize products_request;
#ifndef SK_NO_PLAYHEAVEN
	@synthesize request = _request;
#endif


- (void)mailComposeController:(MFMailComposeViewController*)controller
		  didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error {	
	switch (result)
	{
		case MFMailComposeResultCancelled:
			break;
		case MFMailComposeResultSaved:
			break;
		case MFMailComposeResultSent:
			break;
		case MFMailComposeResultFailed:
			break;
		default:
			break;
	}
	[self dismissModalViewControllerAnimated:YES];
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
	if (sk::game_services::DIALOG_TYPE_YES_NO == alertView.tag)
	{
		sk::game_services::set_yes_no_dialog_result(buttonIndex != 0);
	}
}

- (UIImage*)imageWithImage:(UIImage*)image
			  scaledToSize:(CGSize)newSize;
{
	UIGraphicsBeginImageContext( newSize );
	[image drawInRect:CGRectMake(0,0,newSize.width,newSize.height)];
	UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
	UIGraphicsEndImageContext();
	
	return newImage;
}

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
	cocos2d::CCImage * res = new cocos2d::CCImage();
	UIImage * image = [info objectForKey:UIImagePickerControllerOriginalImage];
	const sk::game_services::image_info& photo_info = sk::game_services::get_image_info();
	UIImage * resized = nil;
	if (photo_info.width > 0 && photo_info.height > 0)
	{
		resized = [self imageWithImage:image scaledToSize:CGSizeMake(photo_info.width, photo_info.height)];
	}
	if (ios_init_ccimage_with_ui_image(res, resized != nil ? resized : image))
	{
		// conversion is ok
	}
	else
	{
		delete res;
		res = 0;
	}
	sk::game_services::on_take_image(res);
	[self closePicker];
}

//Tells the delegate that the user cancelled the pick operation.
- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
	sk::game_services::on_take_image(0);
	[self closePicker];
	[popover release];
	popover = nil;
	[imagePicker release];
	imagePicker = nil;
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
	sk::game_services::on_take_image(0);
}
- (void) closePicker
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
	{
		[self dismissModalViewControllerAnimated:YES];
	}
	else
	{
		[popover dismissPopoverAnimated:YES];
		popover = nil;
	}
	[imagePicker release];
	imagePicker = nil;
}

- (void)createAdBannerView
{
	
#ifndef SK_PAID
#ifdef SK_NO_ADMOB
	adView = [[ADBannerView alloc] initWithFrame:CGRectZero];
	adView.frame = CGRectOffset(adView.frame, 0, -IAD_BANNER_OFFSET);
	NSObject * contentSize = sk::game_services::SO_PORTRAIT_ONLY == sk::game_services::get_supported_orientation() ? ADBannerContentSizeIdentifierPortrait : ADBannerContentSizeIdentifierLandscape;
	adView.requiredContentSizeIdentifiers = [NSSet setWithObject:contentSize];
	[self.view addSubview:adView];
	adView.delegate = self;
	bannerIsVisible = false;
#else
	bannerView_ = [[GADBannerView alloc] initWithAdSize: sk::game_services::get_supported_orientation() == sk::game_services::SO_PORTRAIT_ONLY ? kGADAdSizeSmartBannerPortrait : kGADAdSizeSmartBannerLandscape];
	
	// Specify the ad's "unit identifier." This is your AdMob Publisher ID.
	//bannerView_.adUnitID = @"a1511fd7b5ef799";
	bannerView_.adUnitID = [NSString stringWithUTF8String:sk::game_services::get_admob_mediation_id()];
	
	// Let the runtime know which UIViewController to restore after taking
	// the user wherever the ad goes and add it to the view hierarchy.
	bannerView_.rootViewController = self;
	bannerView_.delegate = self;
#endif // #ifdef SK_NO_ADMOB
#endif // #ifndef SK_PAID
}

#ifndef SK_PAID
#ifdef SK_NO_ADMOB
- (BOOL)bannerViewActionShouldBegin:(ADBannerView *)banner willLeaveApplication:(BOOL)willLeave
{
	NSLog(@"iAd : bannerViewActionShouldBegin");
	sk::game_services::set_paused(true);
	return YES;
}

- (void)bannerViewActionDidFinish:(ADBannerView *)banner
{
	NSLog(@"iAd : bannerViewActionDidFinish");
	sk::game_services::set_paused(false);
}

- (void)bannerViewDidLoadAd:(ADBannerView *)banner
{
	if (!bannerIsVisible)
	{
		[UIView beginAnimations:@"animateAdBannerOn" context:NULL];
		banner.frame = CGRectOffset(banner.frame, 0, IAD_BANNER_OFFSET);
		[UIView commitAnimations];
		bannerIsVisible = true;
	}
}

- (void)bannerView:(ADBannerView *)banner didFailToReceiveAdWithError:(NSError *)error
{
	if (bannerIsVisible)
	{
		[UIView beginAnimations:@"animateAdBannerOff" context:NULL];
		banner.frame = CGRectOffset(banner.frame, 0, -IAD_BANNER_OFFSET);
		[UIView commitAnimations];
		bannerIsVisible = false;
	}
}

#else
- (void)adViewWillPresentScreen:(GADBannerView *)bannerView
{
	NSLog(@"AdMob: adViewWillPresentScreen");
	sk::game_services::set_paused(true);
}

- (void)adViewDidDismissScreen:(GADBannerView *)bannerView
{
	NSLog(@"AdMob: adViewDidDismissScreen");
	sk::game_services::set_paused(false);
}

- (void)adViewWillDismissScreen:(GADBannerView *)bannerView
{
	
}

- (void)adViewWillLeaveApplication:(GADBannerView *)bannerView
{
	
}


- (void)adViewDidReceiveAd:(GADBannerView *)adView
{
	NSLog(@"AdMob: Received ad successfully");
}

- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error
{
	NSLog(@"AdMob: Failed to receive ad with error: %@", [error localizedFailureReason]);
}

- (GADRequest *)createRequest
{
	GADRequest *request = [GADRequest request];
	if (sk::game_services::is_banner_ads_in_test_mode())
	{
		request.testing = YES;
		request.testDevices = [NSArray arrayWithObjects:@"", nil];
	}
	return request;
}
#endif // #ifdef SK_NO_ADMOB
#endif // #ifndef SK_PAID


#pragma mark Revmob delegate methods
- (void)revmobAdDidFailWithError:(NSError *)error
{
	NSLog(@"RevMob : revmobAdDidFailWithError  %@", error);
	[self cb];
}

- (void)revmobAdDidReceive
{
	NSLog(@"RevMob : revmobAdDidReceive");
}

- (void)revmobAdDisplayed
{
	NSLog(@"RevMob : revmobAdDisplayed");
	sk::game_services::set_revmob_shown(true);
	sk::game_services::set_paused(true);
	sk::game_services::log_event("revmob_interstitial_displayed");
}

- (void)revmobUserClickedInTheAd
{
	NSLog(@"RevMob : revmobUserClickedInTheAd");
		
	sk::game_services::set_revmob_shown(false);
	sk::game_services::set_paused(false);
	sk::game_services::log_event("revmob_interstitial_did_click");
}

- (void)revmobUserClosedTheAd
{
	NSLog(@"RevMob : revmobUserClosedTheAd");
	sk::game_services::set_revmob_shown(false);
	sk::game_services::set_paused(false);
	if (!sk::game_services::is_ads_removed())
	{
		// too many ads in game
		//[self cb];
		//[self ph];
	}
}

-(void) cb
{
    //sk::game_services::disable_all_interstitials() ||
	if (sk::game_services::is_interstitial_shown())
	{
		return;
	}
	Chartboost *cbb = [Chartboost sharedChartboost];
	[cbb showInterstitial];
}

-(void) more_games
{	
	Chartboost *cbb = [Chartboost sharedChartboost];
	if (cb_cached_more_games)
	{
		[cbb showMoreApps];
	}
	else
	{
		[cbb cacheMoreApps];
//#ifndef SK_NO_PLAYHEAVEN
//		[self ph];
//#else
		
//#endif
	}
}

- (void) log_event:(const char *) event
{
	if (!event)
	{
		return;
	}
	[Flurry logEvent:[NSString stringWithUTF8String:event]];
}

- (void) rm
{
	/*if (sk::game_services::disable_all_interstitials())
	{
		return;
	}*/
    if(sk::game_services::is_interstitial_shown())
    {
        return;
    }
	RevMobFullscreen *ad = [[RevMobAds session] fullscreen]; // you must retain this object
	ad.delegate = self;
	[ad loadAd];
	[ad showAd];
}

- (void) shw_more_games
{
	//RevMobFullscreen *ad = [[RevMobAds session] fullscreen]; // you must retain this object
	//ad.delegate = self;
	//[[RevMobAds session] openAdLinkWithDelegate:self];
    
    Chartboost *cbb = [Chartboost sharedChartboost];
	if (cb_cached_more_games)
	{
		[cbb showMoreApps];
	}
	else
	{
		[cbb cacheMoreApps];
	}
}

- (void)noad
{	
#ifndef SK_PAID
#ifdef SK_NO_ADMOB
	[adView removeFromSuperview];
#else
	[bannerView_ removeFromSuperview];
#endif // #ifdef SK_NO_ADMOB
#endif // #ifndef SK_PAID
}


- (void) on_launch
{
#ifndef SK_NO_TAPJOY
    self.TapJoy = [TapJoyHandler create:self];
    [self.TapJoy connect];
#endif
    
	[RevMobAds startSessionWithAppID:[NSString stringWithUTF8String:sk::game_services::get_revmob_app_id()]];
#ifndef SK_NO_ADCOLONY
    NSArray* zoneIDs = [[NSArray alloc] initWithObjects: [NSString stringWithUTF8String:sk::game_services::get_adcolony_zone()], nil];
    
    [AdColony configureWithAppID:[NSString stringWithUTF8String:sk::game_services::get_adcolony_app_id()] zoneIDs:zoneIDs delegate:self logging:NO];
#endif
#ifndef SK_PAID
	if (!sk::game_services::is_ads_removed() && !sk::game_services::disable_banner_ads())
	{
		[self createAdBannerView];
#ifndef SK_PAID
#ifdef SK_NO_ADMOB

#else
		[self.view addSubview:bannerView_];
		[self.view bringSubviewToFront:bannerView_];
		[bannerView_ loadRequest:[self createRequest]];
#endif // #ifdef SK_NO_ADMOB
#endif // #ifndef SK_PAID
		
	}
#endif
	[Flurry startSession:[NSString stringWithUTF8String:sk::game_services::get_flurry_app_key()]];
	
#ifndef SK_NO_PLAYHEAVEN
	PHPublisherOpenRequest * request = [PHPublisherOpenRequest requestForApp:[NSString stringWithUTF8String:sk::game_services::get_playheaven_token()] secret:[NSString stringWithUTF8String:sk::game_services::get_playheaven_secret()]];
    //request.customUDID = self.customUDIDField.text;
    request.delegate = self;
    [request send];
#endif
	
	
	sk::game_services::on_launch();
}

#ifndef SK_NO_PLAYHEAVEN

-(void)request:(PHAPIRequest *)request didSucceedWithResponse:(NSDictionary *)responseData
{
	NSLog(@"Playheaven : didSucceedWithResponse");
}

/*
 -(void)request:(PHAPIRequest *)request didFailWithError:(NSError *)error
 {
 NSLog(@"Playheaven : didFailWithError %@", error);
 }
 */

-(void)requestFinishedPrefetching:(PHAPIRequest *)request
{
	NSLog(@"Playheaven : requestFinishedPrefetching");
}

- (void) ph
{
	if (sk::game_services::disable_all_interstitials() || sk::game_services::is_interstitial_shown())
	{
		return;
	}
	if (self.request == nil)
	{

        PHPublisherContentRequest * request = [PHPublisherContentRequest requestForApp:[NSString stringWithUTF8String:sk::game_services::get_playheaven_token()] secret:[NSString stringWithUTF8String:sk::game_services::get_playheaven_secret()] placement:@"more_games" delegate:self];
        [request setShowsOverlayImmediately:YES];
        [request setAnimated:YES];
        [request send];
        [self setRequest:request];
    }
	else
	{
        
        [self.request cancel];
        self.request = nil;
    }
}

-(void)requestWillGetContent:(PHPublisherContentRequest *)request{

}

-(void)requestDidGetContent:(PHPublisherContentRequest *)request{

}

-(void)request:(PHPublisherContentRequest *)request contentWillDisplay:(PHContent *)content
{
	sk::game_services::set_paused(true);
}

-(void)request:(PHPublisherContentRequest *)request contentDidDisplay:(PHContent *)content
{
	NSLog(@"Playheaven :  contentDidDisplay %@", content);
	sk::game_services::set_ph_shown(true);
    [_notificationView clear];

}

-(void)request:(PHPublisherContentRequest *)request contentDidDismissWithType:(PHPublisherContentDismissType *)type
{
	NSLog(@"Playheaven :  contentDidDismissWithType %@", type);
    self.request = nil;
	sk::game_services::set_ph_shown(false);
	sk::game_services::set_paused(false);
	                       
	if ([type isEqualToString:PHPublisherNativeCloseButtonTriggeredDismiss] || [type isEqualToString:PHPublisherContentUnitTriggeredDismiss])
	{
		
	}
	else
	if ([type isEqualToString:PHPublisherApplicationBackgroundTriggeredDismiss])
	{
		sk::game_services::set_app_is_goint_to_bg(true);
	}
	else
	if ([type isEqualToString:PHPublisherNoContentTriggeredDismiss])
	{
		NSLog(@"Playheaven :  no content launching chartboost instead %@", type);
		[self cb];
	}
}

-(void)request:(PHPublisherContentRequest *)request didFailWithError:(NSError *)error
{
	NSLog(@"Playheaven :  Failed with error: %@", error);
	self.request = nil;
}

-(void)request:(PHPublisherContentRequest *)request unlockedReward:(PHReward *)reward{

}

-(void)request:(PHPublisherContentRequest *)request makePurchase:(PHPurchase *)purchase{

}

#endif // #ifndef SK_NO_PLAYHEAVEN

- (void) inapps_init
{
	NSMutableSet * product_ids = [[NSMutableSet alloc] init];
	int s = sk::game_services::get_total_inapps();
	
	for (int i = 0; i < s; ++i)
	{
		[product_ids addObject:[NSString stringWithUTF8String:sk::game_services::get_inapp_store_id(i)]];
	}
	
	[self.products_request = [SKProductsRequest alloc] initWithProductIdentifiers:product_ids];
	[product_ids release];
	self.products_request.delegate = self;
	[self.products_request start];
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
}

- (bool) buy_product:(const char *) product_id
{
	NSString * productId = [NSString stringWithUTF8String:product_id];
	if ([SKPaymentQueue canMakePayments])
	{
		SKProduct *product = nil;
		for (SKProduct * p in self.products)
		{
			if ([p.productIdentifier isEqualToString:productId])
			{
				product = p;
				break;
			}
		}
		
		if(nil == product)
		{
			return false;
		}
		
		SKPayment *payment = [SKPayment paymentWithProduct:product];
		[[SKPaymentQueue defaultQueue] addPayment:payment];
		return false;
	}
	else
	{
		return false;
	}
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
	for (SKPaymentTransaction *transaction in transactions)
	{
		switch (transaction.transactionState)
		{
			case SKPaymentTransactionStateRestored:
				sk::game_services::on_purchase([transaction.originalTransaction.payment.productIdentifier UTF8String],  [[VerificationController sharedInstance] verifyPurchase:transaction]);
				[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
				break;
			case SKPaymentTransactionStatePurchased:
				sk::game_services::on_purchase([transaction.payment.productIdentifier UTF8String],  [[VerificationController sharedInstance] verifyPurchase:transaction]);
				[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
				break;
			case SKPaymentTransactionStateFailed:
				sk::game_services::on_purchase( [transaction.payment.productIdentifier UTF8String], false);
				[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
				break;
			default:
				break;
		}
	}
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
	self.products = response.products;
	
	NSLog(@"In-App Purchases :, %d products loaded", self.products.count);
	for (unsigned int i = 0; i < self.products.count; ++i)
	{
		SKProduct * p = (SKProduct*)[self.products objectAtIndex:i];
		sk::game_services::on_inapp_confirmed_by_store([p.productIdentifier UTF8String]);
	}
	[self.products_request release];
	self.products_request = nil;
}
/*
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
	NSLog(@"In-App Purchases : SKProductsRequest Error: %@", error);
}
 */

// The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]))
	{
		vc = self;
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationWillResignActive:) name:UIApplicationWillResignActiveNotification object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationDidBecomeActive:)  name:UIApplicationDidBecomeActiveNotification  object:nil];
	}
	return self;
}

- (void)dealloc
{
#ifndef SK_NO_TAPJOY
    [self.TapJoy release];
    self.TapJoy = nil;
#endif
    
	[[NSNotificationCenter defaultCenter] removeObserver:self];		
#ifndef SK_PAID
	
#ifdef SK_NO_ADMOB
	adView.delegate = nil;
	[adView release];
#else
	bannerView_.delegate = nil;
	[bannerView_ release];
#endif // #ifdef SK_NO_ADMOB
	
#endif
	
#ifndef SK_NO_PLAYHEAVEN
	[PHAPIRequest cancelAllRequestsWithDelegate:self];
	[_notificationView release], _notificationView = nil;
    [_request release], _request = nil;
#endif
	
    [super dealloc];
}

- (NSString *) scores_queue_path
{
    return [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]
            stringByAppendingPathComponent:@"gc.score.gz"];
}

-(void)applicationWillResignActive:(NSNotification *)notification
{
	
}

-(void)applicationDidBecomeActive:(NSNotification *)notification
{
    gc_login_failed = false;
	
	if (!sk::game_services::is_game_center_disabled())
	{
		if ([self gc_auth])
		{
			return;
		}
		[self submit_scores];
	}
}

-(void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
    [self dismissViewControllerAnimated:YES completion:^(void) {}];
}

- (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController
{
    [self dismissViewControllerAnimated:YES completion:^(void) {}];
}

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 60000
- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    [self dismissViewControllerAnimated:YES completion:^(void) {}];
}
#endif
- (void) gc_show_achivements
{
    GKAchievementViewController *achievementsController = [[GKAchievementViewController alloc] init];
    if (nil != achievementsController)
    {
        achievementsController.achievementDelegate = self;
        [self presentViewController:achievementsController animated:YES completion:^(void) {}];
		[achievementsController release];
    }
	
}

-(void)gc_leaderboard_show:(NSString *)category
{
    GKLeaderboardViewController * leaderboardController = [[GKLeaderboardViewController alloc] init];
    leaderboardController.leaderboardDelegate = self;
    leaderboardController.category = category;
    [self presentViewController:leaderboardController animated:YES completion:^(void) {}];
    [leaderboardController release];
}

-(void)gc_show:(NSString*)category
{
    GKGameCenterViewController *gameCenterController = [[GKGameCenterViewController alloc] init];
    if (nil != gameCenterController)
    {
        gameCenterController.gameCenterDelegate = self;
        gameCenterController.leaderboardCategory = category;
        gameCenterController.viewState = GKGameCenterViewControllerStateLeaderboards;
        [self presentViewController: gameCenterController animated: YES completion:^(void) {}];
		[gameCenterController release];
    } 
}

-(void)gc_achivements
{
    GKGameCenterViewController *gameCenterController = [[GKGameCenterViewController alloc] init];
    if (nil != gameCenterController)
    {
        gameCenterController.gameCenterDelegate = self;
        gameCenterController.viewState = GKGameCenterViewControllerStateAchievements;
        [self presentViewController: gameCenterController animated: YES completion:^(void) {}];
		[gameCenterController release];
    }
}


- (void) gc_init
{	
	gc_login_failed    = false;
	gc_login_cancelled = false;
}

- (bool) submit_score:(const char *) cat value:(unsigned int) value
{
    if (![self gc_auth])
	{
		return false;
	}
	// TODO : after android bug is fixed remove memory leak here
    GKScore  * score      = [[GKScore alloc] initWithCategory:[NSString stringWithUTF8String:cat]];
    score.value = value;
	[self submit_score:score];
    return true;
}

- (void) submit_score:(GKScore *) score
{
	if (nil == score)
	{
		return;
	}
	
	@try {
		[score reportScoreWithCompletionHandler: ^(NSError *error)
		 {
			 if (!error)
			 {
				 sk::game_services::on_leaderboard_score_submitted([score.category UTF8String], score.value);
			 }
			 else
			 {
				 NSLog(@"GameCenter : error while score submisstion: %@\n", error);
				 
				 if ([[error domain] isEqualToString:GKErrorDomain])
				 {
					 switch ([error code])
					 {
						 case GKErrorNotAuthenticated:
						 case GKErrorCommunicationsFailure:
						 case GKErrorAuthenticationInProgress:
							 return;
					 }
				 }
			 }
		 }];		
	}
	@catch (NSException * e)
	{
		NSLog(@"GameCenter : exception during score submission: %@", e);
	}
}

- (bool) gc_auth
{
	if (![GKLocalPlayer localPlayer].authenticated)
    {
        static bool auth_is_going_on = false;
        if (!auth_is_going_on)
        {
            if (gc_login_failed || gc_login_cancelled)
			{
				return true;
			}
			
            auth_is_going_on = true;
            
            [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error)
             {
                 if (!error)
                 {
					 sk::game_services::on_gamecenter_auth();
                 }
                 else
                 {
					 sk::game_services::on_gamecenter_auth_failed();
                     gc_login_failed = true;
                     if ([[error domain] isEqualToString:GKErrorDomain])
                     {
						 if (GKErrorCancelled == [error code])
						 {
							 gc_login_cancelled = true;
						 }
                     }
                 }
                 
                 auth_is_going_on = false;
             }];
        }
    }
	return true;
}

/*
 // Implement loadView to create a view hierarchy programmatically, without using a nib.
 - (void)loadView {
 }
 */

/*
 // Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
 - (void)viewDidLoad {
 [super viewDidLoad];
 }
 
 */
// Override to allow orientations other than the default portrait orientation.
// This method is deprecated on ios6
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	if (sk::game_services::get_supported_orientation() == sk::game_services::SO_PORTRAIT_ONLY)
	{
		return UIInterfaceOrientationIsPortrait(interfaceOrientation);
	}
	else
	{
		return UIInterfaceOrientationIsLandscape(interfaceOrientation);
	}
}

// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead
- (NSUInteger) supportedInterfaceOrientations{
#ifdef __IPHONE_6_0
	if (sk::game_services::get_supported_orientation() == sk::game_services::SO_PORTRAIT_ONLY)
	{
		return UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown;
	}
	else
	{
		return UIInterfaceOrientationMaskLandscape;
	}
#endif
}

- (BOOL) shouldAutorotate
{
    return YES;
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

#ifndef SK_NO_ADCOLONY

#pragma mark - ADColony

- (void) adc:(BOOL)rewardFlag
{
    // rewardFlag is not used
    
    sk::game_services::log_event("adcolony_video_requested");
    [AdColony playVideoAdForZone:[NSString stringWithUTF8String:sk::game_services::get_adcolony_zone()] withDelegate:self withV4VCPrePopup:YES andV4VCPostPopup:YES ];
}

#pragma mark - ADColonyDelegate

/**
 * Provides your app with real-time updates about ad availability changes.
 * This method is called when a zone's ad availability state changes (when ads become available, or become unavailable).
 * Listening to these callbacks allows your app to update its user interface immediately.
 * For example, when ads become available in a zone you could immediately show an ad for that zone.
 * @param available Whether ads became available or unavailable
 * @param zoneID The affected zone
 */
- ( void ) onAdColonyAdAvailabilityChange:(BOOL)available inZone:(NSString*) zoneID
{
    NSLog(@"onAdColonyAdAvailabilityChange to %@", available ? @"available" : @"unavailable");
    
    // we should notify player about videoAD unavailability
}

/**
 * Notifies your app after an ad is displayed when a virtual currency transaction has completed.
 * In your implementation, check for success and implement any app-specific code that should be run when
 * AdColony has successfully rewarded. Client-side V4VC implementations should increment the user's currency
 * balance in this method. Server-side V4VC implementations should contact the game server to determine
 * the current total balance for the virtual currency.
 * @param success Whether the transaction succeeded or failed
 * @param currencyName The name of currency to reward
 * @param amount The amount of currency to reward
 * @param zoneID The affected zone
 */
- ( void ) onAdColonyV4VCReward:(BOOL)success currencyName:(NSString*)currencyName currencyAmount:(int)amount inZone:(NSString*)zoneID
{
    NSLog(@"onAdColonyV4VCReward with name %@ and amount %@ for zone %@", currencyName, [NSString stringWithFormat:@"%d", amount], zoneID );
    sk::game_services::on_video_ad_award( amount );
	sk::game_services::log_event("adcolony_video_viewed");
}

#pragma mark - ADColonyAdDelegate

/**
 * Notifies your app that an ad will actually play in response to the app's request to play an ad.
 * This method is called when AdColony has taken control of the device screen and is about to begin
 * showing an ad. Apps should implement app-specific code such as pausing a game and turning off app music.
 * @param zoneID The affected zone
 */
- ( void ) onAdColonyAdStartedInZone:( NSString * )zoneID
{
    NSLog(@"onAdColonyAdStartedInZone");
	sk::game_services::log_event("adcolony_video_began");
	sk::game_services::set_paused(true);
}

/**
 * Notifies your app that an ad completed playing (or never played) and control has been returned to the app.
 * This method is called when AdColony has finished trying to show an ad, either successfully or unsuccessfully.
 * If an ad was shown, apps should implement app-specific code such as unpausing a game and restarting app music.
 * @param shown Whether an ad was actually shown
 * @param zoneID The affected zone
 */
- ( void ) onAdColonyAdAttemptFinished:(BOOL)shown inZone:( NSString * )zoneID
{
    NSLog(@"onAdColonyAdAttemptFinished");
	sk::game_services::log_event("adcolony_video_ended");
	sk::game_services::set_paused(false);
}

#endif


// Called when an interstitial has failed to come back from the server
- (void)didFailToLoadInterstitial:(NSString *)location
{
	NSLog(@"ChartBoost : didFailToLoadInterstitial %@", location);
	NSLog(@"ChartBoost : launching playheaven instead");
#ifndef SK_NO_PLAYHEAVEN
	//[self ph];
#endif
}

/*
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
	cb_cached_more_games = true;
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
*/

#pragma mark misc methods
-(void)cb_cached_more_games_true
{
    cb_cached_more_games = TRUE;
}

@end





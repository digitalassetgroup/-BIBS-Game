//
//  TapJoyHandler.h
//  GameMadBride
//
//  Created by Slava Arhipov on 23.01.14.
//
//

#import <Foundation/Foundation.h>

@class RootViewController;

@interface TapJoyHandler : NSObject

+ (TapJoyHandler*)create:(RootViewController*)rootViewController;
- (void)connect;
- (void)openOfferWall;
- (void)doShowTapjoyInterstitial;

@end

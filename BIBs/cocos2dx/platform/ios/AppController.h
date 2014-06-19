#import <UIKit/UIKit.h>
#import "Chartboost.h"

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate, ChartboostDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
}

@end


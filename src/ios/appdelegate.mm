#import "UIKit/UIKit.h"
#include <QtCore>

#import "Firebase/Firebase.h"

@interface QIOSApplicationDelegate : UIResponder <UIApplicationDelegate>
@end

@interface QIOSApplicationDelegate(AppDelegate)
@end

@implementation QIOSApplicationDelegate (AppDelegate)

- (BOOL)application:(UIApplication *)application
  didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    Q_UNUSED(application);
    Q_UNUSED(launchOptions);
    // Use Firebase library to configure APIs
    //[FIRApp configure];

    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    Q_UNUSED(application);
    UIApplication.sharedApplication.applicationIconBadgeNumber = 0;
}

@end

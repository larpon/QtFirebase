#import <UIKit/UIKit.h>

#import "Firebase/Firebase.h"

#import <GoogleSignIn/GoogleSignIn.h>

@interface AppDelegate : UIResponder<UIApplicationDelegate, GIDSignInDelegate>

+(AppDelegate *)sharedAppDelegate;

@end


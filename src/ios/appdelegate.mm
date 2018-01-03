#import "UIKit/UIKit.h"
#include <QtCore>

#import "Firebase/Firebase.h"

#import "AppDelegate.h"
#import "qtfirebaseauth.h"

//google header
#import <GoogleSignIn/GoogleSignIn.h>

//facebook heder
//#import <FBSDKCoreKit/FBSDKCoreKit.h>

// Twitter header
//#import <TwitterKit/TwitterKit.h>

@interface QIOSApplicationDelegate : UIResponder <UIApplicationDelegate, GIDSignInDelegate>
@end

@interface QIOSApplicationDelegate(AppDelegate)
@end

@implementation QIOSApplicationDelegate (AppDelegate)

// Firebase project ID e.g. : Your-App-ID

// Google sign in essentials
  static NSString * const googleClientID = @"Your-Google-Client-ID";

// Facebook signin essentails
// * On the Facebook for Developers site, get the App ID and an App Secret for your app.
// * Enable Facebook Login:
// * In the Firebase console, open the Auth section.
// * On the Sign in method tab, enable the Facebook sign-in method and specify the App ID and App Secret you got from Facebook.
// * Then, make sure your OAuth redirect URI (e.g. https://Your-App-ID.firebaseapp.com/__/auth/handler) is listed as one of your OAuth redirect URIs in your Facebook app's settings page on the Facebook for Developers site in the Product Settings > Facebook Login config.
// * Add folowing lines in Info.plist
// <key>LSApplicationQueriesSchemes</key>
// <array>
//    <string>fbauth2</string>
// </array>
// * Add URL schemes in Info.plist with your facebook app id
// <key>CFBundleURLTypes</key>
// <array>
//   <dict>
//   <key>CFBundleURLSchemes</key>
//   <array>
//     <string><your fb id here eg. fbxxxxxx></string>
//   </array>
//   </dict>
// </array>
// * your AppID and App DisplayName
static NSString * const facebookAppID = @"Your-Facebook-App-ID";
static NSString * const facebookAppDisplayName = @"Your-Facebook-App-Display-Name";

// Twitter sign in essentials
// * Register your app as a developer application on Twitter and get your app's API Key and API Secret.
// * Enable Twitter Login: In the Firebase console, open the Auth section.
// * On the Sign in method tab, enable the Twitter sign-in method and specify the API Key and API Secret you got from Twitter.
// * Then, make sure your Firebase OAuth redirect URI (e.g. https://Your-App-ID.firebaseapp.com/__/auth/handler) is set as your Callback URL in your app's settings page on your Twitter app's config.
// * Add folowing lines in Info.plist, in the section: <key>LSApplicationQueriesSchemes</key>
//    <string>twitter</string>
//    <string>twitterauth</string>
// * Add folowing lines in Info.plist, in the section: <key>CFBundleURLSchemes</key>
//    <string>twitterkit-<Your-Consumer-Key></string>
static NSString * const twitterApiKey = @"Your-Twitter-Api-Key";
static NSString * const twitterApiSecret = @"Your-Twitter-Api-Secret";


- (BOOL)application:(UIApplication *)application
  didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    Q_UNUSED(application);
    Q_UNUSED(launchOptions);

    // Use Firebase library to configure APIs
    [FIRApp configure];
    [GIDSignIn sharedInstance].clientID = googleClientID;

    // Facebook configure
    //[FBSDKSettings setAppID:facebookAppID];
    //[FBSDKSettings setDisplayName:facebookAppDisplayName];

    // Twitter configure
    //[[Twitter sharedInstance] startWithConsumerKey:twitterApiKey consumerSecret:twitterApiSecret];

    NSLog(@"FireApp configured successfully");

    return YES;
}

// [START google facebook twitter openurl method]
- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation {

 BOOL googleHandled = [[GIDSignIn sharedInstance] handleURL:url
                      sourceApplication:sourceApplication
                      annotation:annotation];

 //BOOL facebookHandled = [[FBSDKApplicationDelegate sharedInstance] handleURL:url
                      sourceApplication:sourceApplication
                      annotation:annotation];

// BOOL twitterHandled = [[Twitter sharedInstance] handleURL:url
//                      sourceApplication:sourceApplication
//                      annotation:annotation];

  return googleHandled /*|| facebookHandled || twitterHandled*/;
}
// [END google facebook twitter openurl method]

/*- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options {
 return [[Twitter sharedInstance] application:app openURL:url options:options];
}*/

@end

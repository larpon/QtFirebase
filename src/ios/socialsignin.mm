#import "UIKit/UIKit.h"
#include <QtCore>
#import "socialsignin.h"
#import "qtfirebaseauth.h"
#import "qtfirebasegoogleauth.h"
#import "Firebase/Firebase.h"

// Facebook header
//#import <FBSDKCoreKit/FBSDKCoreKit.h>
//#import <FBSDKLoginKit/FBSDKLoginKit.h>

// Twitter header
//#import <TwitterKit/TwitterKit.h>

@interface QIOSViewController : UIResponder <UIPageViewControllerDelegate, GIDSignInDelegate, GIDSignInUIDelegate>
@end

@interface QIOSViewController(socialsignin)
@end

@implementation QIOSViewController (socialsignin)

  UIWindow* rootWindow;

- (void)viewDidLoad {
    [super viewDidLoad];

    rootWindow = [PlatformUtils::getNativeWindow() window];

    [GIDSignIn sharedInstance].delegate = self;
    [GIDSignIn sharedInstance].uiDelegate = self;
}

void QtFirebaseAuth::googleSignIn()
{
    //[GIDSignIn sharedInstance].shouldFetchBasicProfile = YES;
    [GIDSignIn sharedInstance].scopes = @[@"profile", @"email"];

    if ([GIDSignIn sharedInstance].hasAuthInKeychain) {

           NSLog(@"user signed in already, signing silently...");
           GIDGoogleUser *user = [GIDSignIn sharedInstance].currentUser;
           NSString *userId = user.userID;
           NSString *fullName = user.profile.name;
           NSString *email = user.profile.email;
           //NSLog(@"------->>>>User : %@, %@, %@", userId, fullName, email);
           if (user.profile.hasImage)
           {
               NSURL *url = [user.profile imageURLWithDimension:100];
               //NSLog(@"url : %@",url);
           }
           if(!user)
               [[GIDSignIn sharedInstance] signInSilently];
     }
     else {
        NSLog(@"Not signed in, start signing procedure");
        [[GIDSignIn sharedInstance] signIn];
    }
}

/*void QtFirebaseAuth::facebookSignIn()
{
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];

    if ([FBSDKAccessToken currentAccessToken])
    {
        NSLog(@"Token is available : %@",[[FBSDKAccessToken currentAccessToken]tokenString]); 
                  //signInwithCredential(firebase::auth::FacebookAuthProvider::GetCredential([[FBSDKAccessToken currentAccessToken].tokenString UTF8String]));
    }
    else
    {
    [login logInWithReadPermissions: @[@"public_profile",@"email"]
    fromViewController: rootWindow.rootViewController handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
        {
      if (error) {
        NSLog(@"Process error");
      } else if (result.isCancelled) {
        NSLog(@"Cancelled");
      } else {
          NSLog(@"facebook signing in");
          //QString accessToken = QString::fromNSString([FBSDKAccessToken currentAccessToken].tokenString);
          //signInwithCredential(firebase::auth::FacebookAuthProvider::GetCredential([[FBSDKAccessToken currentAccessToken].tokenString UTF8String]));
      }
    }];
   }

    [login logOut];
}*/

/*void QtFirebaseAuth::twitterSignIn()
    {
    // https://developer.twitter.com/en/docs/basics/authentication/overview/application-permission-model#announcement
//    Application permission model
//    There are three levels of permission available to applications:
//    read only
//    read and write
//    read, write and access Direct Messages

    [[Twitter sharedInstance] logInWithCompletion:^(TWTRSession *session, NSError *error) {
      if (session) {

         std::string idtoken = session.authToken.UTF8String;
         std::string secret = session.authTokenSecret.UTF8String;

         //signInwithCredential(firebase::auth::TwitterAuthProvider::GetCredential(idtoken.c_str(),secret.c_str()));

         TWTRSessionStore *store = [[Twitter sharedInstance] sessionStore];
         NSString *userID = store.session.userID;

         [store logOutUserID:userID];

      } else {
          NSLog(@"error: %@", [error localizedDescription]);
      }
    }];
}*/

- (void) signIn:(GIDSignIn *)signIn
didSignInForUser:(GIDGoogleUser *)user
     withError:(NSError *)error {
    if (error) {
        NSLog(@"error at sign in %@", error);
        /*qFirebaseGoogleAuth->setError(error.code, QString::fromNSString(error.localizedDescription));
        qFirebaseGoogleAuth->setSignIn(false);
        qFirebaseGoogleAuth->setComplete(true);*/
        return;
    }
    else
    {
        // The OAuth2 access token to access Google services, needed to connect with firebase
        GIDAuthentication *authentication = user.authentication;
        std::string idtoken = authentication.idToken.UTF8String;

        NSString *userId = user.userID;
        NSString *fullName = user.profile.name;
        NSString *email = user.profile.email;
        //NSLog(@"------->>>>User : %@, %@, %@", userId, fullName, email);

        if (user.profile.hasImage)
        {
            NSURL *url = [user.profile imageURLWithDimension:100];
            qFirebaseGoogleAuth->photoUrl = QString::fromUtf8(url.absoluteString.UTF8String);
            qDebug() << "----->>>> Hereeee is fine the photo_url: " << qFirebaseGoogleAuth->photoUrl;
        }

        qFirebaseGoogleAuth->firebaseSignIn(QString::fromUtf8(idtoken.c_str()));

        [[GIDSignIn sharedInstance] signOut];
    }
}

- (void)signIn:(GIDSignIn *)signIn
didDisconnectWithUser:(GIDGoogleUser *)user
     withError:(NSError *)error {
    if (error) {
        NSLog(@"error at sign out %@", error);
        // qFirebaseGoogleAuth->setError(error.code, QString::fromNSString(error.localizedDescription));
        return;
    }
}

@end

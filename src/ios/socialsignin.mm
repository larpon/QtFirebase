#import "UIKit/UIKit.h"
#include <QtCore>

#import "socialsignin.h"

#import "qtfirebaseauth.h"
#import "Firebase/Firebase.h"

// Facebook header
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>

// Twitter header
#import <TwitterKit/TwitterKit.h>


@interface QIOSViewController : UIResponder <UIPageViewControllerDelegate, GIDSignInDelegate, GIDSignInUIDelegate>
@end

@interface QIOSViewController(socialsignin)
@end

@implementation QIOSViewController (socialsignin)

  UIApplication* app;
  UIWindow* rootWindow;
  UIViewController* rootViewController;

- (void)viewDidLoad {
    [super viewDidLoad];

    app = [UIApplication sharedApplication];
    rootWindow = app.windows[0];
    rootViewController = rootWindow.rootViewController;

    GIDSignIn *signIn = [GIDSignIn sharedInstance];
    signIn.shouldFetchBasicProfile = YES;
    signIn.delegate = self;
    signIn.uiDelegate = self;
}

bool QtFirebaseAuth::googleSignIn()
{
    GIDSignIn *signin = [GIDSignIn sharedInstance];
//    signin.scopes = @[@"https://www.googleapis.com/auth/plus.login",
//                      @"https://www.googleapis.com/auth/plus.me"];
    [signin setScopes:[NSArray arrayWithObject:@"profile"]];

    if (signin.hasAuthInKeychain) {

           NSLog(@"user signed in already, signing silently...");

           GIDGoogleUser *user = [GIDSignIn sharedInstance].currentUser;
           if(!user) {
               [[GIDSignIn sharedInstance] signInSilently];
           }
     }
     else {

        NSLog(@"Not signed in, start signing procedure");
        [signin signIn];
    }
}

bool QtFirebaseAuth::facebookSignIn()
{
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];

    if ([FBSDKAccessToken currentAccessToken])
    {
        NSLog(@"Token is available : %@",[[FBSDKAccessToken currentAccessToken]tokenString]); 
    }
    else
    {
    [login logInWithReadPermissions: @[@"public_profile",@"email"]
    fromViewController: rootViewController handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
        {
      if (error) {
        NSLog(@"Process error");
      } else if (result.isCancelled) {
        NSLog(@"Cancelled");
      } else {

          FIRAuthCredential *credential = [FIRFacebookAuthProvider
              credentialWithAccessToken:[FBSDKAccessToken currentAccessToken].tokenString];

          [[FIRAuth auth] signInWithCredential:credential
                                    completion:^(FIRUser *user, NSError *error) {
              if (error) {
                //[self showMessagePrompt:error.localizedDescription];
                  NSLog(@"error at sign in %@", error, error.localizedDescription);
                return;
              } else {

              // User successfully signed in, getting user data, perform any operations on signed in user here
                  NSString *uid = user.uid;
                  NSString *displayName = user.displayName;
                  NSString *email = user.email;
                  NSString *phoneNumber = user.phoneNumber;
                  NSURL *photoURL = user.photoURL;

                  NSLog(@"userData: %@   %@   %@  %@  %@", uid, displayName, email, phoneNumber, photoURL);
              }
          }];
      }
    }];
   }

    [login logOut];
}

bool QtFirebaseAuth::twitterSignIn()
    {

    [[Twitter sharedInstance] logInWithCompletion:^(TWTRSession *session, NSError *error) {
      if (session) {

          FIRAuthCredential *credential =
              [FIRTwitterAuthProvider credentialWithToken:session.authToken
                                                   secret:session.authTokenSecret];

          // [START signin_credential]
          [[FIRAuth auth] signInWithCredential:credential
                                      completion:^(FIRUser *user, NSError *error) {
              if (error) {
                //[self showMessagePrompt:error.localizedDescription];
                  NSLog(@"error at sign in %@", error, error.localizedDescription);
                return;
              } else {

              // User successfully signed in, getting user data, perform any operations on signed in user here
                  NSString *uid = user.uid;
                  NSString *displayName = user.displayName;
                  NSString *email = user.email;
                  NSString *phoneNumber = user.phoneNumber;
                  NSURL *photoURL = user.photoURL;

                  NSLog(@"userData: %@   %@   %@  %@  %@", uid, displayName, email, phoneNumber, photoURL);

                  TWTRAPIClient *client = [TWTRAPIClient clientWithCurrentUser];
                  // to access the user email
                  [client requestEmailForCurrentUser:^(NSString *email, NSError *error) {
                    if (email) {
                        NSLog(@"signed in as %@", email);
                    } else {
                        NSLog(@"error: %@", [error localizedDescription]);
                    }

                  }];

                  TWTRSessionStore *store = [[Twitter sharedInstance] sessionStore];
                  NSString *userID = store.session.userID;

                  [store logOutUserID:userID];
              }
          }];
          // [END signin_credential]

      } else {
          NSLog(@"error: %@", [error localizedDescription]);
      }
    }];
}

- (void)signIn:(GIDSignIn *)signIn
didSignInForUser:(GIDGoogleUser *)user
     withError:(NSError *)error {
    if (error) {
        NSLog(@"error at sign in %@", error);
        return;
    }
    else
    {
        // The OAuth2 access token to access Google services, needed to connect with firebase
        GIDAuthentication *authentication = user.authentication;
        FIRAuthCredential *credential =
        [FIRGoogleAuthProvider credentialWithIDToken:authentication.idToken
                                          accessToken:authentication.accessToken];

        // [START signin_credential]
        [[FIRAuth auth] signInWithCredential:credential
                                    completion:^(FIRUser *user, NSError *error) {
            if (error) {
              //[self showMessagePrompt:error.localizedDescription];
                NSLog(@"error at sign in %@", error, error.localizedDescription);
              return;
            } else {

            // User successfully signed in, getting user data, perform any operations on signed in user here
                NSString *uid = user.uid;
                NSString *displayName = user.displayName;
                NSString *email = user.email;
                NSString *phoneNumber = user.phoneNumber;
                NSURL *photoURL = user.photoURL;

                NSLog(@"userData: %@   %@   %@  %@  %@", uid, displayName, email, phoneNumber, photoURL);
            }
        }];
        // [END signin_credential]

        [[GIDSignIn sharedInstance] signOut];
    }
}

bool QtFirebaseAuth::addGoogleSignInBtn(const qreal& xPos, const qreal& yPos,const qreal& width,const qreal& height)
{
    GIDSignInButton *loginButton = [[GIDSignInButton alloc] initWithFrame: CGRectMake ( xPos, yPos, width, height)];
    //loginButton.readPermissions = @[@"public_profile", @"email"];
    [rootWindow addSubview:loginButton];
}

bool QtFirebaseAuth::addFacebookSignInBtn(const qreal& xPos, const qreal& yPos,const qreal& width,const qreal& height)
{
    FBSDKLoginButton *loginButton = [[FBSDKLoginButton alloc] initWithFrame: CGRectMake ( xPos, yPos, width, height)];
    loginButton.readPermissions = @[@"public_profile", @"email"];
    [rootWindow addSubview:loginButton];
}

bool QtFirebaseAuth::addTwitterSignInBtn(const qreal& xPos, const qreal& yPos,const qreal& width,const qreal& height)
{
     TWTRLogInButton *loginButton = [[TWTRLogInButton alloc] initWithFrame: CGRectMake ( xPos, yPos, width, height)];

     //loginButton.readPermissions = @[@"public_profile", @"email", @"user_friends"];

     [TWTRLogInButton buttonWithLogInCompletion:^(TWTRSession *session, NSError *error) {
        if (session) {
            NSLog(@"signed in as %@", [session userName]);
        } else {
            NSLog(@"error: %@", [error localizedDescription]);
        }
      }];

      loginButton.center = rootWindow.center;
      [rootWindow addSubview:loginButton];
}

@end

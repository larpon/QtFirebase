#include <QObject>
#include <QtQml>

#import <UIKit/UIKit.h>
#import <GoogleSignIn/GoogleSignIn.h>
#import "Firebase/Firebase.h"

/*
 * GoogleSignIn interface for ios
 */

// [START viewcontroller_interfaces]
@interface socialsignin : UIResponder <UIPageViewControllerDelegate, GIDSignInDelegate, GIDSignInUIDelegate>
// [END viewcontroller_interfaces]

@end


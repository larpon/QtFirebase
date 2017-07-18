<img src="logo.png" align="right" />

# QtFirebase
An effort to bring the Firebase C++ API to Qt 5

QtFirebase aim to bring all the features of the Firebase C++ SDK to Qt 5 - both as C++ wrappers and as QML components.

Please bear in mind that the Firebase C++ SDK currently only support the mobile platforms Android and iOS.

You can still build QtFirebase on other platforms as the project provide "empty shells" or placeholder components - they just return default/empty values when used. Because of this you won't see e.g. ads from AdMob in your desktop builds.

This is due to Google's own limitations in the Firebase C++ SDK implementation.

# Features / Status
The following [features](https://firebase.google.com/docs/cpp/setup) have a working Qt 5 C++ and QML counterpart

Feature | Library | C++ | QML
------- | ------- | --- | ---
Base                      |libapp.a             |✓|✓
AdMob                     |libadmob.a           |✓|✓
Analytics                 |libanalytics.a       |✓|✓
~~Authentication~~	          |~~libauth.a~~          | |
~~Realtime Database~~	        |~~libdatabase.a~~      | |
~~Invites and Dynamic Links~~	|~~libinvites.a~~       | |
~~Cloud Messaging~~	          |~~libmessaging.a~~     |in-progress |:wrench:
Remote Config             |libremote_config.a   |✓|✓
~~Storage~~	                  |~~libstorage.a~~       | |

Tested Firebase C++ SDK versions:

**Base, AdMob, Analytics**
Up until commit [cb52be83](https://github.com/Larpon/QtFirebase/commit/cb52be8328a063956c2d2139fa9ab7152d955cc2)
* v2.1.0
* v2.1.1
* v2.1.2

**Base, AdMob, Analytics, RemoteConfig**
* v3.1.0
* v4.0.1

We recommend you build against the latest version of the Firebase C++ SDK.

## Stability
The Firebase C++ SDK has a lot of stability issues - which QtFirebase does it's best to work around by wrapping the API into more Qt/C++ friendly classes that prevent some of the bugs and crashes found so far.
Please use the latest SDK version to get the latest fixes.

# Setup
For a working and up-to-date example please follow the **Quick start** section found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) README.

The following outlines what need to be done before you can build and use QtFirebase.

* Download and extract [Google's Firebase C++ SDK](https://firebase.google.com/docs/cpp/setup).

## Android
Make sure you have `Google Services` installed and updated on the *target* device. Firebase won't work without it.

  * Enable gradle in your QtCreator build options
  * Add some lines to `gradle.build`
  * Add some lines to `gradle.properties`
  * Add some lines to `local.properties`
  * Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/)

## iOS

  * [Install CocoaPods](http://stackoverflow.com/questions/20755044/how-to-install-cocoa-pods) if you haven't already
  * Run `pod install` in the `src/ios/CocoaPods` directory
  * Add some entries in Info.plist
  * Include `GoogleService-Info.plist` downloaded from the [Firebase console](https://console.firebase.google.com/)


# Examples
Please look at the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) repository.
You will find details here on what to add to your different project files (gradle.build, Info.plist etc.)

# Documentation
Under heavy construction.

# Support
Possible ways of getting support
* Open an [issue](https://github.com/Larpon/QtFirebase/issues)

Possible ways of giving support
* Comment helpfully on issues
* Fork, Change, Make Pull Request
* Share online
* [Buy QtFirebase &#10084;'s](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4DHVNRBQRRU96)
  You'll get a personal email full of hearts from [Black Grain](http://blackgrain.dk/games/)!

# In the wild
The following is a list of software that uses QtFirebase
* [Hammer Bees](http://blackgrain.dk/games/hammerbees/) (Casual game, [Android](https://play.google.com/store/apps/details?id=com.bitkompot.android.hammerbees.ad), [iOS](https://itunes.apple.com/us/app/hammer-bees-free/id1164069527?ls=1&mt=8))
* [Dead Ascend](http://blackgrain.dk/games/deadascend/) (Open Source, Adventure game, [Android](https://play.google.com/store/apps/details?id=com.blackgrain.android.deadascend.ad), [iOS](https://itunes.apple.com/us/app/dead-ascend/id1197443665?ls=1&mt=8))
* \<your awesome project\>

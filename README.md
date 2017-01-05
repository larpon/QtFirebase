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
~~Cloud Messaging~~	          |~~libmessaging.a~~     | |
~~Remote Config~~	            |~~libremote_config.a~~ | |
~~Storage~~	                  |~~libstorage.a~~       | |

## Stability
The current Firebase C++ SDK (v2.1.0) has a lot of stability issues - which QtFirebase does it's best to work around by wrapping the API into more Qt/C++ friendly classes that prevent some of the bugs and crashes found so far.

# Setup
For a working and up-to-date example please follow the **Quick start** section found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) README.

In rough terms the following are what needs to be done before you can build against QtFirebase.

* Download and extract [Google's Firebase C++ SDK](https://firebase.google.com/docs/cpp/setup).

* **Android**

  * Enable gradle in your QtCreator build options
  * Add some lines to `gradle.build`
  * Add some lines to `gradle.properties`
  * Add some lines to `local.properties`
  * Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/)

* **iOS**

  * [Install CocoaPods](http://stackoverflow.com/questions/20755044/how-to-install-cocoa-pods) if you haven't already
  * Run `pod install` in the `src/ios/CocoaPods` directory
  * Add some entries in Info.plist
  * Include `GoogleService-Info.plist` downloaded from the [Firebase console](https://console.firebase.google.com/)


# Examples
Please look at the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) repository.

# Documentation
Under heavy construction.

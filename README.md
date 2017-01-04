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

# Examples
Under heavy construction.
So far you'll have to look at the source in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) repository.

# Documentation
Under heavy construction.

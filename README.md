<img src="docs/img/logo.png" align="right" />

# QtFirebase
An effort to bring the Firebase C++ API to Qt 5

QtFirebase aims to bring all the features of the Firebase C++ SDK to Qt 5 - both as C++ wrappers and as QML components.

Please bear in mind that the Firebase C++ SDK currently only supports the mobile platforms Android and iOS.

You can still build QtFirebase on other platforms as the project provides stub implementations or placeholder components - they just return default/empty values when used. Because of this you won't see e.g. ads from AdMob in your desktop builds.

This is due to Google's own limitations in the Firebase C++ SDK implementation.

The Firebase C++ SDK claim to already provide [stub implementations](https://groups.google.com/d/msg/firebase-talk/PUVJYuR-v2U/711ECRzlBgAJ) for easier building on unsupported platforms - QtFirebase have chosen **not** to link to unnecessary closed source code - thus we do not link against the stub implementations from Google.


# Features / Status
The following [features](https://firebase.google.com/docs/cpp/setup) have a working Qt 5 C++ and QML counterpart

Feature | Library | C++ | QML | Credits
------- | ------- | --- | --- | -------
Base                      |libapp.a               |✓|✓|[Lars Pontoppidan](https://github.com/Larpon)
AdMob                     |libadmob.a             |✓|✓|[Lars Pontoppidan](https://github.com/Larpon)
Analytics                 |libanalytics.a         |✓|✓|[Lars Pontoppidan](https://github.com/Larpon)
Authentication	          |libauth.a              |Partial |Partial |[Isy](https://github.com/isipisi89)
Realtime Database	        |libdatabase.a          |Partial |Partial |[greenfield932](https://github.com/greenfield932)
~~Dynamic Links~~       	|~~libdynamic_links.a~~ | | |
Cloud Messaging	          |libmessaging.a         |✓|✓|[Isy](https://github.com/isipisi89)
Remote Config             |libremote_config.a     |✓|✓|[greenfield932](https://github.com/greenfield932)
~~Cloud Storage~~	        |~~libstorage.a~~       |in progress |:wrench: |[Furkanzmc](https://github.com/Furkanzmc)


## Tested Firebase C++ SDK versions
**Base, AdMob, Analytics**
Up until commit [cb52be83](https://github.com/Larpon/QtFirebase/commit/cb52be8328a063956c2d2139fa9ab7152d955cc2):

`v2.1.0`, `v2.1.1`, `v2.1.2`

**Base, AdMob, Analytics, RemoteConfig**

`v3.1.0`, `v4.0.1`

**Base, AdMob, Analytics, RemoteConfig, Cloud Messaging**

`v4.0.3`

**Base, AdMob, Analytics, RemoteConfig, Cloud Messaging, Authentication, Realtime Database**

`v4.1.0`, `v4.2.0`, `v4.5.0`, `v4.5.1`, `v5.0.0`, `v5.1.1`, `v5.3.0`, `6.1.0`, `6.7.0`

In most cases you'll be able to build against the latest version of the Firebase C++ SDK - but we've experienced times when developers got lost in the woods for doing so. In that case we recommend you build against one of the latest tested versions of the Firebase C++ SDK as listed above.


# Bugs
When you encounter bugs; Please see if they are known issues [here](https://github.com/firebase/quickstart-cpp/issues) or [here](https://github.com/Larpon/QtFirebase/issues) - before reporting

# Setup
For a working and up-to-date example please follow the **Quick start** section found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) README.

Please see [SETUP.md](docs/SETUP.md) for instructions on how to set up QtFirebase in your Qt Creator project.

# Examples
Please look at the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) repository.
In [SETUP.md](docs/SETUP.md) you will find details on what to add to your project files (gradle.build, Info.plist, etc.).

# Documentation
Please see comments and usecases in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) project

# Support
Possible ways of getting support
* Open an [issue](https://github.com/Larpon/QtFirebase/issues)

Possible ways of giving support
* Comment helpfully on issues
* Fork, Change, Make a Pull Request
* Share online
* [Buy QtFirebase &#10084;'s](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4DHVNRBQRRU96)
  You'll get a personal email full of hearts from [Black Grain](http://blackgrain.dk/games/)!

Possible ways of being really awesome
* Open source your own Qt/QML projects
* Be nice (it's really that simple)

# Notes

## Stability
The Firebase C++ SDK has (or has had) a lot of stability issues - which QtFirebase does its best to work around by wrapping the API into more Qt/C++ friendly classes that prevent some of the bugs and crashes found so far.
Please use the latest SDK version to get the latest fixes.

## Versions

Version `v4.5.0`:
Native Express ads are deprecated and removed from Firebase in this release and forward.
Therefore `QtFirebaseAdMobNativeExpressAd` is removed in commit [4e217cc](https://github.com/Larpon/QtFirebase/commit/4e217cc5ae4270631c8f98d1cce94c8b849b1f08).

Version `v6.0.0`:
Invites are removed from Firebase.

# In the wild
The following is a list of software that uses QtFirebase
* [Hammer Bees](http://blackgrain.dk/games/hammerbees/) (Casual game, [Android](https://play.google.com/store/apps/details?id=com.bitkompot.android.hammerbees.ad), [iOS](https://itunes.apple.com/us/app/hammer-bees-free/id1164069527?ls=1&mt=8))
* [Dead Ascend](http://blackgrain.dk/games/deadascend/) (Open Source, Adventure game, [Android](https://play.google.com/store/apps/details?id=com.blackgrain.android.deadascend.ad), [iOS](https://itunes.apple.com/us/app/dead-ascend/id1197443665?ls=1&mt=8))
* Scoreboard Basketball (Scoreboard for basketball, [Android](https://play.google.com/store/apps/details?id=com.alexodus.scorebkfree), [iOS](https://itunes.apple.com/us/app/my-scoreboard-basketball/id1235460810?mt=8))
* \<your awesome project here\>

# Contributors
Without [contributions from all these good people](https://github.com/Larpon/QtFirebase/pulls?q=is%3Apr+is%3Aclosed) this project would not exist.

[Lars Pontoppidan](https://github.com/Larpon) (Maintainer, project founder, Base, AdMob, Analytics),
[greenfield932](https://github.com/greenfield932) (Remote Config, Misc.),
[Isy](https://github.com/isipisi281) (Cloud Messaging, Auth, Misc.),
[Andrew Dolby (adolby)](https://github.com/adolby) (Cloud Messaging, Misc.),
[li3p](https://github.com/li3p) (Bugs, Misc.),
[morebest](https://github.com/morebest) (Bugs, Misc.)
[guillaume charbonnier](https://github.com/gcharbonnier) (Auth) ... and many more

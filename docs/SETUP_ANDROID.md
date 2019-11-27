# Android specific setup
When building QtFirebase for Android targets you need the following extra steps to get everything running.

## General

### Ensure target device has `Google Services` apk installed
Make sure you have `Google Services` apk installed and updated on the *target* device. Firebase won't work without it.
Furthermore the project needs `gradle` and the Android NDK (>= r10d) to build on Android.

### API level
Make sure you set your `Android build SDK` to, at least, **API level 23** in your Qt Creator project settings.
Firebase still work on devices from API level 14 [according to this source](https://firebase.googleblog.com/2016/11/google-play-services-and-firebase-for-android-will-support-api-level-14-at-minimum.html) - but it apparently need to be build with a higher target API.

> **NOTE** Your project's API level might need to be higher than we suggest here due to Qt or Google Play requirements.

### Gradle setup
Enable gradle in your Qt Creator build options.

For a fully working `build.gradle` file please refer to [this example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/build.gradle)

Depending on what modules you intend to use from Firebase you should edit the dependencies in your project's `gradle.build` to match your preferences. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/android/build.gradle#L166-L194))

Edit paths to match your setup in `/path/to/your/Qt Creator/project/android/gradle.properties`. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/gradle.properties))

Edit paths to match your setup in `/path/to/your/Qt Creator/project/android/local.properties`.([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/local.properties))

On some Qt Creator setups the above `*.properties` files are auto-generated by the build tool - and thus overriden on each build.
If you're using `qmake` you can use [ANDROID_PACKAGE_SOURCE_DIR](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/App.pro#L35) to include your own overrides of these files.

### Firebase configuration
Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/). ([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/App.pro#L42))

## Firebase Messaging specific

> **Note** *Using Messaging is only possible with Qt 5.9+ due to gradle dependencies that require more recent versions of the Android SDK. Upgrading these will also "force" you to upgrade Qt (because of some bugs in Qt Creator) in order for it all to just work™*

To use Messaging on Android there is some additional setup.

You'll need to have recent versions of the Android SDK/NDK toolchain.
```
Android SDK Build-Tools >= 25.0.0
Android NDK >= r10e
```
`gradle >= 2.3.3` [like here](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/android/build.gradle#L19)

> **NOTE** Qt usually require or advice higher versions than stated here. Go with whatever Qt requires!

### Services
Add Messaging specific services to your AndroidManifest.xml file. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/android/AndroidManifest.xml#L47-L63)).


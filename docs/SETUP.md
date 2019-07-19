# Setup
How to set up QtFirebase for inclusion and to utilize Firebase modules in your project.

For a working and up-to-date example that can be compiled in Qt Creator please follow the **Quick start** section found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) README. (The example app links back here in some places - but not all). It's strongly adviced to see how the example app is setup in-order to avoid any missing entries.

## Prerequisities
QtFirebase rely on a relativly large software stack. Many common errors people encounter with QtFirebase is due to little or no knownledge of the complete stack of technology used. It's therefore stongly adviced that developers, as a minimum, know or familiarize themselves with the following software in general before diving into the project.

In general
[Firebase](https://firebase.google.com/docs), [Firebase C++ SDK](https://firebase.google.com/docs/cpp/setup), [Qt](https://qt.io), [QML](https://doc.qt.io/qt-5/qmlapplications.html), [QMake](https://doc.qt.io/qt-5/qmake-manual.html)

Android specific
[Gradle](https://gradle.org/), [Android SDK](https://developer.android.com/studio/releases/sdk-tools), [Android NDK](https://developer.android.com/ndk/), [Java](https://www.oracle.com/technetwork/java/index.html)

iOS specific
[XCode](https://developer.apple.com/xcode/), [Firebase iOS SDK](https://firebase.google.com/download/ios)

## Base setup

### Clone the [QtFirebase](https://github.com/Larpon/QtFirebase) project

Clone into the "extensions" or "vendor" folder or into another folder of your choice.
Here we use the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) structure.

```
cd /path/to/projects/QtFirebaseExample/extensions
git clone git@github.com:Larpon/QtFirebase.git
```

### Download and extract the latest version of [Google's Firebase C++ SDK](https://firebase.google.com/docs/cpp/setup).

```
cd /path/to/download
curl --location https://firebase.google.com/download/cpp --output "firebase_cpp_sdk.zip"
unzip firebase_cpp_sdk.zip -d /path/to/sdk
rm firebase_cpp_sdk.zip
```

### Add symlink OR set `QTFIREBASE_SDK_PATH` variable

If you have multiple projects using QtFirebase it's a space-saver to have the Firebase C++ SDK (~832 MB) in one place.

You can either symlink the Firebase C++ SDK to the default search path OR set the `QTFIREBASE_SDK_PATH` variable to the absolute path of the SDK in your project's `.pro` file for the app build.

#### Symlink method
```
ln -s /path/to/sdk/firebase_cpp_sdk /path/to/projects/QtFirebase/firebase_cpp_sdk
```

#### `QTFIREBASE_SDK_PATH` variable method

Open your project with Qt Creator.
Navigate to the `.pro` (sub)project file in the Project pane.
```
QtFirebaseExample
|_...
|_App
    |_App.pro
```

Locate the lines:
```
# NOTE QTFIREBASE_SDK_PATH can be symlinked to match $$PWD/firebase_cpp_sdk
...
```

Change it to match your path(s) - or leave it to use default `$$PWD/firebase_cpp_sdk`
```
QTFIREBASE_SDK_PATH = /path/to/sdk/firebase_cpp_sdk
```
([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/App.pro#L86-L87))

#### Choose Firebase modules
In your project's `.pro` (sub)project file (before including `qtfirebase.pri`) - choose the modules to include:
```
QTFIREBASE_CONFIG += analytics messaging admob remote_config
```
([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/App.pro#L88))

#### include QtFirebase
Finally include `qtfirebase.pri` in your project's `.pro` file.
```
include(/path/to/QtFirebase/qtfirebase.pri)
```
([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/App.pro#L93-L94))

## Note on building
If you're planing on compiling QtFirebase as part of a library in your project, please be aware that QtFirebase in this case will **omit** registering of it's QML types. This is due to how [Q_COREAPP_STARTUP_FUNCTION](https://doc.qt.io/qt-5/qcoreapplication.html#Q_COREAPP_STARTUP_FUNCTION) works. So for auto registering of QML QtFirebase need to be [build as an app](https://github.com/Larpon/QtFirebaseExample/blob/ada77a8b0cde17f8da8df89015bf111ae2c1f328/App/App.pro#L1). Otherwise you must [register the types manually](https://github.com/Larpon/QtFirebase/blob/2ba1c1e4d7cf174aad2108bcdf19c29acdcf6610/qtfirebase_register.h#L79-L113)

## Desktop specific setup
Currently we haven't tested the parts of Firebase C++ SDK that have desktop support.

QtFirebase provides stub implementations ("empty shells" or "placeholders") for desktop builds and ***no*** firebase libraries are linked to the application - *this may change* depending on what parts of the SDK Google make available for desktop builds in the future.

## Android specific setup
When building QtFirebase for Android targets you need the following extra steps to get everything running.

### General

#### Ensure target device has `Google Services` apk installed
Make sure you have `Google Services` apk installed and updated on the *target* device. Firebase won't work without it.
Furthermore the project needs `gradle` and the Android NDK (>= r10d) to build on Android.

#### API level
Make sure you set your `Android build SDK` to **API level 23** in your Qt Creator project settings.
Firebase still work on devices from API level 14 [according to this source](https://firebase.googleblog.com/2016/11/google-play-services-and-firebase-for-android-will-support-api-level-14-at-minimum.html) - but it apparently need to be build with a higher target API.

#### Gradle setup
Enable gradle in your Qt Creator build options.

For a fully working `build.gradle` file please refer to [this example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/build.gradle)

Depending on what modules you intend to use from Firebase you should edit the dependencies in your project's `gradle.build` to match your preferences. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/android/build.gradle#L166-L194))

Edit paths to match your setup in `/path/to/your/Qt Creator/project/android/gradle.properties`. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/gradle.properties))

Edit paths to match your setup in `/path/to/your/Qt Creator/project/android/local.properties`.([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/local.properties))

On some Qt Creator setups the above `*.properties` files are auto-generated by the build tool - and thus overriden on each build.
If you're using `qmake` you can use [ANDROID_PACKAGE_SOURCE_DIR](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/App.pro#L35) to include your own overrides of these files.

#### Firebase configuration
Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/). ([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/App.pro#L42))

### Firebase Messaging specific
**Note**
*Using Messaging is only possible with Qt 5.9+ due to gradle dependencies that require more recent versions of the Android SDK. Upgrading these will also "force" you to upgrade Qt (because of some bugs in Qt Creator) in order for it all to just work™*

To use Messaging on Android there is some additional setup.

You'll need to have recent versions of the Android SDK/NDK toolchain.
```
Android SDK Build-Tools >= 25.0.0
Android NDK >= r10e
```
`gradle >= 2.3.3` [like here](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/android/build.gradle#L19)

#### Services
Add Messaging specific services to your AndroidManifest.xml file. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/android/AndroidManifest.xml#L47-L63)).



## iOS specifics

#### Download the Firebase iOS Frameworks
Run the included script [`$$PWD/src/ios/download_firebase_ios.sh`](https://github.com/Larpon/QtFirebase/blob/master/src/ios/download_firebase_ios.sh) to download the iOS SDK automatically

... or manually download Firebase.zip from https://firebase.google.com/download/ios and extract it to `$$PWD/src/ios/`.

**PRO TIP**
> If you are upgrading QtFirebase to a newer version - you'll probably want to download the Firebase iOS Frameworks again in order to get the latest matching versions.


### Firebase Messaging specific
To use Messaging on iOS there is some additional setup.

#### Enable Apple Push
1. Go to https://developer.apple.com/account
2. Click on the right side of the page on "Certificates, Identifiers & Profiles"
3. Click on the right side of the page on "Keys, All"
4. Click the "+" icon at the top right of the page
5. Find Service Enable "APNs" and enter Key Description Name "Your keyname". At the bottom of the page click "Continue", then click "Confirm" and download the key.
6. Go to https://console.firebase.google.com/
7. Navigate to **Overview (Gear icon)** -> **Project settings** -> **Cloud Messaging (Tab)**
8. Upload your APNs info


**PRO TIP**
> The following changes can be made permanent with each build from Qt Creator by using various build tool (`qmake`) tricks.
It's currently beyond the scope of this documentation to tell you *how* though.

#### XCode

In your app's `Capabilities` tab.

Remember to enable `Push Notifications`
<img src="https://user-images.githubusercontent.com/23259226/33664253-360c64e2-daa8-11e7-8f0b-07827c6a01a4.png" />

Remember to set `Remote notifications` in `Background Modes`
<img src="https://user-images.githubusercontent.com/23259226/33664370-a344ba64-daa8-11e7-9872-586bbf5927b4.png" />

#### Info.plist
For a fully working `Info.plist` please see [this example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/ios/Info.plist#L66-L71)

#### GoogleService-Info.plist
Include `GoogleService-Info.plist` downloaded from the [Firebase console](https://console.firebase.google.com/). ([Example](https://github.com/Larpon/QtFirebaseExample/blob/1dc13c/App/App.pro#L69))



## Push the *Run* button

If you build for Android or iOS you should see output like the following in the "General Messages" tab of Qt Creator
```
Project MESSAGE: QtFirebase: configuring build for supported Firebase target platform...
Project MESSAGE: No QTFIREBASE_SDK_PATH path sat. Using default (firebase_cpp_sdk) /path/to/projects/QtFirebaseExample/extensions/QtFirebase/firebase_cpp_sdk
Project MESSAGE: QtFirebase Android base
Project MESSAGE: QtFirebase including Analytics
Project MESSAGE: QtFirebase including AdMob
...
Project MESSAGE: This project is using private headers and will therefore be tied to this specific Qt module build version.
Project MESSAGE: Running this project against other versions of the Qt modules may crash at any arbitrary point.
Project MESSAGE: This is not a bug, but a result of using Qt internals. You have been warned!
```

If you are building for Desktop target the output should be something like this:
```
Project MESSAGE: QtFirebase: configuring build for non-supported Firebase target platform...
```

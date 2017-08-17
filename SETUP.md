# Setup
How to setup up QtFirebase for inclusion and to utilize Firebase modules in your project.

For a working and up-to-date example that can be compiled in QtCreator please follow the **Quick start** section found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) README. (The example app links back here).

## Base setup

### Clone the [QtFirebase](https://github.com/Larpon/QtFirebase) project

Clone into the "extensions" or "vendor" folder or into any other folder of your choice.
Here we use the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) structure.

```
cd /path/to/projects/QtFirebaseExample/extensions
git clone git@github.com:Larpon/QtFirebase.git
```
    
### Download and extract the latest version of [Google's Firebase C++ SDK](https://firebase.google.com/docs/cpp/setup).

```
cd /path/to/download
wget https://dl.google.com/firebase/sdk/cpp/firebase_cpp_sdk_4.0.3.zip
unzip firebase_cpp_sdk_4.0.3.zip -d /path/to/sdk
```

### Add symlink OR set `QTFIREBASE_SDK_PATH` variable

If you have multiple projects using QtFirebase it's a space-saver to have the Firebase C++ SDK (~832 MB) in one place.

You can either symlink the Firebase C++ SDK to the default search path OR set the `QTFIREBASE_SDK_PATH` variable to the absolute path of the SDK in your project's `.pro` file for the app build.

#### Symlink method
```
ln -s /path/to/sdk/firebase_cpp_sdk /path/to/projects/QtFirebase/firebase_cpp_sdk
```

#### `QTFIREBASE_SDK_PATH` variable method

Open your project with QtCreator
Navigate to the `.pro` (sub)project file in the Project pane
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
([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/App.pro#L68-L72))

#### Choose Firebase modules
In your project's `.pro` (sub)project file (before including `qtfirebase.pri`) - choose the modules to include:
```
QTFIREBASE_CONFIG += analytics messaging admob remote_config
```
([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/App.pro#L68-L72))

#### include QtFirebase
Finally include `qtfirebase.pri` in your project's `.pro` file.
```
include(/path/to/QtFirebase/qtfirebase.pri)
```
([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/App.pro#L68-L72))


## Desktop specific setup
As Firebase C++ SDK does not support desktop apps - no further setup is needed.

QtFirebase provides "empty shells" or "placeholders" for desktop builds and ***no*** firebase libraries are linked to the application.

## Android specific setup
When building QtFirebase for Android targets you need the following extra steps to get everything running.

### General

#### Ensure target device has `Google Services` apk installed
Make sure you have `Google Services` apk installed and updated on the *target* device. Firebase won't work without it.
Further more the project needs `gradle` and the Android NDK (r10d +) to build on Android.

#### Gradle setup
Enable gradle in your QtCreator build options

Edit lines in your project's `gradle.build` to match your dependencies / modules. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/build.gradle#L164-L187))

Edit paths to match your setup in `/path/to/QtFirebase/src/android/gradle.properties`. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/gradle.properties))

Edit paths to match your setup in `/path/to/QtFirebase/src/android/local.properties`.([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/local.properties))

#### Firebase configuration
Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/)

### Firebase Messaging specific
**Note***
*Using Messaging is only possible with Qt 5.9+ due to some gradle dependencies that needs recent versions the Android SDK/NDK. Upgrading these will also "force" you to upgrade Qt (because of some bugs in QtCreator) in order for it all to work"*

So. If you intend to use Messaging you need some additional setup on Android.

You'll need to have recent versions of your Android SDK/NDK toolchain in order for it to work.
```
Android SDK Build-Tools >= 25.0.0
Android NDK >= r11c
```
`gradle >= 2.3.3` [like here](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/build.gradle#L18)

#### Services
Add Messaging specific services to your AndroidManifest.xml file. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/AndroidManifest.xml#L47-L63)).



## iOS specifics

#### Download the Firebase iOS Frameworks
Either download from the below Link and extract it to `$$PWD/src/ios/Firebase/`
https://firebase.google.com/docs/ios/setup#frameworks

or on Unix-like systems run the included script [`$$PWD/src/ios/download_firebase_ios.sh`](https://github.com/Larpon/QtFirebase/blob/master/src/ios/download_firebase_ios.sh)

#### Add entries in Info.plist
([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/ios/Info.plist#L66-L71))

Include `GoogleService-Info.plist` downloaded from the [Firebase console](https://console.firebase.google.com/). ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/App.pro#L54-L56))

~~#### CocoaPods~~

~~The project currently uses CocoaPods to build on iOS.~~

~~[Install CocoaPods](http://stackoverflow.com/questions/20755044/how-to-install-cocoa-pods) on your Mac host if you haven't already.
Run `pod install`:~~
```
# cd /path/to/QtFirebase/src/ios/CocoaPods
# From our example:
cd /path/to/projects/QtFirebaseExample/extensions/QtFirebase/src/ios/CocoaPods
pod install
```


#### Run `make_ios_joined_statics.sh` from the QtFirebase project root:
```
cd /path/to/QtFirebase/
./make_ios_joined_statics.sh
```

Verify that a set of `lib<name>.a` exists in `/path/to/sdk/firebase_cpp_sdk/libs/ios`
```
cd /path/to/sdk/firebase_cpp_sdk/libs/ios/
ls | grep lib

libadmob.a
libanalytics.a
libapp.a
libremote_config.a
```

This step is important as the `make_ios_joined_statics.sh` uses `libtool` to join each of the static libs used from each supported architecture into one combined static lib to link against. We have yet to find out why this is necessary for the project to run properly.
   
## Push the *Run* button

If you build for Android or iOS you should see output like the following in the "General Messages" tab of QtCreator
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

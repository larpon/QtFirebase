# Setup
How to setup up QtFirebase for inclusion and to utilize Firebase in your project.

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
ln -s /path/to/sdk/firebase_cpp_sdk /path/to/projects/QtFirebaseExample/extensions/QtFirebase/firebase_cpp_sdk
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
QTFIREBASE_CONFIG += analytics admob
# include QtFirebase
include(../extensions/QtFirebase/qtfirebase.pri)
```
Change it to match your path(s)
```
QTFIREBASE_SDK_PATH = /path/to/sdk/firebase_cpp_sdk
QTFIREBASE_CONFIG += analytics admob
# include QtFirebase
include(../extensions/QtFirebase/qtfirebase.pri) # <- /path/to/QtFirebase/qtfirebase.pri
```
    
## Android specifics
Make sure you have `Google Services` installed and updated on the *target* device. Firebase won't work without it.
Further more the project needs gradle and the Android NDK (r10d +) to build on Android.

#### Gradle setup
Enable gradle in your QtCreator build options

Add these lines to your project's `gradle.build`
Edit path in `/path/to/projects/QtFirebaseExample/extensions/QtFirebase/src/android/gradle.properties`
Edit path in `/path/to/projects/QtFirebaseExample/extensions/QtFirebase/src/android/local.properties`

Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/)

Add the services to your xml file from the below link: https://github.com/firebase/quickstart-cpp/blob/e8c20f678a06a28ebb73132abcd79d93b27622d9/messaging/testapp/AndroidManifest.xml


## iOS specifics

Download the Firebase iOS Framework from the below Link and extract it to $$PWD/src/ios/Firebase/
https://firebase.google.com/docs/ios/setup#frameworks
Add some entries in Info.plist
Include `GoogleService-Info.plist` downloaded from the [Firebase console](https://console.firebase.google.com/)

The project currently uses CocoaPods to build on iOS.

* [Install CocoaPods](http://stackoverflow.com/questions/20755044/how-to-install-cocoa-pods) on your Mac host if you haven't already.
* Run `pod install`:
```
# cd /path/to/QtFirebase/src/ios/CocoaPods
# From our example:
cd /path/to/projects/QtFirebaseExample/extensions/QtFirebase/src/ios/CocoaPods
pod install
```
* Run `make_ios_joined_statics.sh` from the QtFirebase project root:
```
cd /path/to/QtFirebase/
./make_ios_joined_statics.sh
```
* Verify that a set of `lib<name>.a` exists in `/path/to/sdk/firebase_cpp_sdk/libs/ios`
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
Project MESSAGE: This project is using private headers and will therefore be tied to this specific Qt module build version.
Project MESSAGE: Running this project against other versions of the Qt modules may crash at any arbitrary point.
Project MESSAGE: This is not a bug, but a result of using Qt internals. You have been warned!
```

If you are building for Desktop target the output should be something like this:
```
Project MESSAGE: QtFirebase: configuring build for non-supported Firebase target platform...
```

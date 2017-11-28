# Setup
How to set up QtFirebase for inclusion and to utilize Firebase modules in your project.

For a working and up-to-date example that can be compiled in Qt Creator please follow the **Quick start** section found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) README. (The example app links back here).

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
Furthermore the project needs `gradle` and the Android NDK (r10d+) to build on Android.

#### Gradle setup
Enable gradle in your Qt Creator build options.

Edit lines in your project's `gradle.build` to match your dependencies / modules. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/build.gradle#L164-L187))

Edit paths to match your setup in `/path/to/QtFirebase/src/android/gradle.properties`. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/gradle.properties))

Edit paths to match your setup in `/path/to/QtFirebase/src/android/local.properties`.([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/local.properties))

#### Firebase configuration
Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/)

### Firebase Messaging specific
**Note**
*Using Messaging is only possible with Qt 5.9+ due to gradle dependencies that require recent versions of the Android SDK/NDK. Upgrading these will also "force" you to upgrade Qt (because of some bugs in Qt Creator) in order for it all to work*

To use Messaging on Android there is some additional setup.

You'll need to have recent versions of the Android SDK/NDK toolchain.
```
Android SDK Build-Tools >= 25.0.0
Android NDK >= r11c
```
`gradle >= 2.3.3` [like here](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/build.gradle#L18)

#### Services
Add Messaging specific services to your AndroidManifest.xml file. ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/android/AndroidManifest.xml#L47-L63)).



## iOS specifics

#### Download the Firebase iOS Frameworks
Run the included script [`$$PWD/src/ios/download_firebase_ios.sh`](https://github.com/Larpon/QtFirebase/blob/master/src/ios/download_firebase_ios.sh)

or download Firebase.zip from https://firebase.google.com/download/ios and extract it to `$$PWD/src/ios/`.

**PRO TIP**
> If you are upgrading QtFirebase to a newer version - you'll probably want to download the Firebase iOS Frameworks again in order to get the latest version

#### Add entries in Info.plist
([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/platforms/ios/Info.plist#L66-L71))

Include `GoogleService-Info.plist` downloaded from the [Firebase console](https://console.firebase.google.com/). ([Example](https://github.com/Larpon/QtFirebaseExample/blob/master/App/App.pro#L54-L56))



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

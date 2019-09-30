# iOS specifics

### Download the Firebase iOS Frameworks
Run the included script [`$$PWD/src/ios/download_firebase_ios.sh`](https://github.com/Larpon/QtFirebase/blob/master/src/ios/download_firebase_ios.sh) to download the iOS SDK automatically

... or manually download Firebase.zip from https://firebase.google.com/download/ios and extract it to `$$PWD/src/ios/`.

**PRO TIP**
> If you are upgrading QtFirebase to a newer version - you'll probably want to download the Firebase iOS Frameworks again in order to get the latest matching versions.


## Firebase Messaging specific
To use Messaging on iOS there is some additional setup.

### Enable Apple Push
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

### XCode

In your app's `Capabilities` tab.

Remember to enable `Push Notifications`
<img src="https://user-images.githubusercontent.com/23259226/33664253-360c64e2-daa8-11e7-8f0b-07827c6a01a4.png" />

Remember to set `Remote notifications` in `Background Modes`
<img src="https://user-images.githubusercontent.com/23259226/33664370-a344ba64-daa8-11e7-9872-586bbf5927b4.png" />

### Info.plist
For a fully working `Info.plist` please see [this example](https://github.com/Larpon/QtFirebaseExample/blob/cc190b/App/platforms/ios/Info.plist#L66-L71)

### GoogleService-Info.plist
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

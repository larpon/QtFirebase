# Setup
How to setup up QtFirebase for inclusion in your project.

For a working and up-to-date example that can be compiled in QtCreator please follow the **Quick start** section found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample) README.

## Base setup
* Download and extract the latest version of [Google's Firebase C++ SDK](https://firebase.google.com/docs/cpp/setup).

## Android
Make sure you have `Google Services` installed and updated on the *target* device. Firebase won't work without it.

### Gradle setup
Enable gradle in your QtCreator build options.

Add these lines to your project's `gradle.build`


Add some lines to `gradle.properties`
Add some lines to `local.properties`
Include `google-services.json` downloaded from the [Firebase console](https://console.firebase.google.com/)
Add the services to your xml file from the below link:
https://github.com/firebase/quickstart-cpp/blob/e8c20f678a06a28ebb73132abcd79d93b27622d9/messaging/testapp/AndroidManifest.xml


## iOS

 Download the Firebase iOS Framework from the below Link and extract it to $$PWD/src/ios/Firebase/
 https://firebase.google.com/docs/ios/setup#frameworks
 Add some entries in Info.plist
 Include `GoogleService-Info.plist` downloaded from the [Firebase console](https://console.firebase.google.com/)
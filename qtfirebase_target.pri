message("QtFirebase: configuring build for supported Firebase target platform...")

isEmpty(QTFIREBASE_SDK_PATH){
    QTFIREBASE_SDK_PATH = $$PWD/firebase_cpp_sdk
    message("No QTFIREBASE_SDK_PATH path set. Using default (firebase_cpp_sdk) $$QTFIREBASE_SDK_PATH")
} else {
    message("Using QTFIREBASE_SDK_PATH ($$QTFIREBASE_SDK_PATH)")
}

QTFIREBASE_SDK_LIBS_PREFIX = "firebase_"

QML_IMPORT_PATH += $$PWD

INCLUDEPATH += $$PWD

INCLUDEPATH += \
    $$QTFIREBASE_SDK_PATH/include \
    \

HEADERS += \
    $$PWD/src/platformutils.h \
    $$PWD/src/qtfirebase.h \
    $$PWD/src/qtfirebaseservice.h \
    \

SOURCES += \
    $$PWD/src/qtfirebase.cpp \
    $$PWD/src/qtfirebaseservice.cpp \
    \

contains(QTPLUGIN,qtfirebase) {
    HEADERS += $$PWD/src/qtfirebase_plugin.h
    SOURCES += $$PWD/src/qtfirebase_plugin.cpp
}

!ios: {
    SOURCES += \
        $$PWD/src/platformutils.cpp \
    \
}

RESOURCES += \
    $$PWD/qtfirebase.qrc \
    \

android: {

    # Setup tips
    #
    # Add services to xml file
    # https://github.com/firebase/quickstart-cpp/blob/e8c20f678a06a28ebb73132abcd79d93b27622d9/messaging/testapp/AndroidManifest.xml
    #
    # Add the 'onNewIntent' function to your Qt Activity
    # https://github.com/firebase/quickstart-cpp/blob/e8c20f678a06a28ebb73132abcd79d93b27622d9/messaging/testapp/src/android/java/com/google/firebase/example/TestappNativeActivity.java

    message("QtFirebase Android base")
    QT += androidextras gui-private

    # Specify the STL variant that is to be used in the app .pro file with the $$QTFIREBASE_STL_VARIANT variable
    # c++:      LLVM libc++ runtime
    # gnustl:   GNU STL
    # stlport:  STLPort runtime
    android-clang {
        QTFIREBASE_STL_VARIANT = c++
    }

    isEmpty(QTFIREBASE_STL_VARIANT){
        QTFIREBASE_STL_VARIANT = gnustl
    }

    # Firebase C++ SDK 9.0.0
    exists($$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/$$QTFIREBASE_STL_VARIANT) {
        message("Using QTFIREBASE_STL_VARIANT ($$QTFIREBASE_STL_VARIANT)")
        QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/$$QTFIREBASE_STL_VARIANT
    } else {
        QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH
    }

    DEPENDPATH += $$QTFIREBASE_SDK_LIBS_PATH
}

ios: {

    # Setup tips
    #
    # Download the Google Firebase Framework link for Ios
    # and extract the files to this folder $$PWD/src/ios/Firebase/
    # or use your own folder QTFIREBASE_FRAMEWORKS_ROOT = YOUR/PWD
    # https://firebase.google.com/docs/ios/setup#frameworks

    message("QtFirebase iOS base")
    QT += gui_private

    QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/ios/universal/

    isEmpty(QTFIREBASE_FRAMEWORKS_ROOT) {
        QTFIREBASE_FRAMEWORKS_ROOT = $$PWD/src/ios/Firebase/
        message("No QTFIREBASE_FRAMEWORKS_ROOT path set. Using default $$QTFIREBASE_FRAMEWORKS_ROOT")
    } else {
        message("Using QTFIREBASE_FRAMEWORKS_ROOT ($$QTFIREBASE_FRAMEWORKS_ROOT)")
    }

    !exists($$QTFIREBASE_FRAMEWORKS_ROOT) {
        error("$$QTFIREBASE_FRAMEWORKS_ROOT path does not exist. To download iOS SDK run download_firebase_ios.sh\
 in $$PWD/src/ios directory using command line or set the proper path to SDK by setting QTFIREBASE_FRAMEWORKS_ROOT in your project's .pro file")
    }

    QMAKE_LFLAGS += -ObjC

    HEADERS += \
        $$PWD/src/ios/AppDelegate.h \
        \

    OBJECTIVE_SOURCES += \
        $$PWD/src/ios/platformutils.mm \
        $$PWD/src/ios/AppDelegate.mm \
        \

    LIBS += \
        -ObjC \
        -lsqlite3 \
        -lz \
        -L$$QTFIREBASE_SDK_LIBS_PATH \
        -framework MediaPlayer \
        -framework CoreMotion \
        -framework CoreTelephony \
        -framework MessageUI \
        -framework GLKit \
        -framework AddressBook \
        -framework UIKit \
        -framework SystemConfiguration \
        \

    INCLUDEPATH += \
        $$QTFIREBASE_SDK_PATH/include \
        $$QTFIREBASE_FRAMEWORKS_ROOT/../ \
        $$PWD/src \
        $$PWD/src/ios \
        \
}

# NOTE the order of linking is important!

PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}app.a
LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}app

# AdMob
contains(DEFINES,QTFIREBASE_BUILD_ADMOB) {
    message( "QtFirebase including AdMob" )

    ios: {
        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Google-Mobile-Ads-SDK/GoogleMobileAds.xcframework/ios-arm64_armv7 \
            -framework GoogleMobileAds \
            -framework AdSupport \
            -framework StoreKit \
            \
    }

    HEADERS += \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobtestdevices.h \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmob.h \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobrequest.h \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobbase.h \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobbanner.h \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobinterstitial.h \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobrewardedvideoad.h \
        \

    SOURCES += \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobtestdevices.cpp \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmob.cpp \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobrequest.cpp \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobbase.cpp \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobbanner.cpp \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobinterstitial.cpp \
        $$PWD/src/qtfirebaseadmob/qtfirebaseadmobrewardedvideoad.cpp \
        \

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}admob.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}admob
}

# Remote Config
contains(DEFINES,QTFIREBASE_BUILD_REMOTE_CONFIG) {
    message( "QtFirebase including RemoteConfig" )

    ios: {
        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseRemoteConfig/FirebaseRemoteConfig.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseRemoteConfig/FirebaseABTesting.xcframework/ios-arm64_armv7 \
            -framework FirebaseABTesting \ # Required for Firebase iOS >= 4.5.0
            -framework FirebaseRemoteConfig \
            \
    }

    HEADERS += $$PWD/src/qtfirebaseremoteconfig.h
    SOURCES += $$PWD/src/qtfirebaseremoteconfig.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}remote_config.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}remote_config
}

# Messaging
contains(DEFINES,QTFIREBASE_BUILD_MESSAGING) {
    message( "QtFirebase including Messaging" )

    ios: {
        message( "QtFirebase Messaging requires iOS v10.x+ to build." )
        message( "Remember to set QMAKE_IOS_DEPLOYMENT_TARGET in your .pro" )
        # Removed due to Qt 5.12 - see https://github.com/Larpon/QtFirebase/issues/106
        # QMAKE_IOS_DEPLOYMENT_TARGET = 10.0

        exists($$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseMessaging/FirebaseInstanceID.xcframework) {
            LIBS += \
                -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseMessaging/FirebaseInstanceID.xcframework/ios-arm64_armv7 \
                -framework FirebaseInstanceID \
                \
        }

        exists($$QTFIREBASE_FRAMEWORKS_ROOT/FirebasePerformance/Protobuf.xcframework) {
            LIBS += \
                -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebasePerformance/Protobuf.xcframework/ios-arm64_armv7 \
                -framework Protobuf \
                \
        }

        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseMessaging/FirebaseMessaging.xcframework/ios-arm64_armv7 \
            -framework FirebaseMessaging \
            -framework UserNotifications \
            \
    }

    HEADERS += $$PWD/src/qtfirebasemessaging.h
    SOURCES += $$PWD/src/qtfirebasemessaging.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}messaging.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}messaging
}

# Analytics
contains(DEFINES,QTFIREBASE_BUILD_ANALYTICS) {
    message( "QtFirebase including Analytics" )

    ios: {
        # -framework FirebaseCoreDiagnostics \
        #-framework GoogleDataTransportCCTSupport \

        LIBS += \
            -framework StoreKit \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/FirebaseAnalytics.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/FirebaseCore.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/GoogleUtilities.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/nanopb.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/PromisesObjC.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/FirebaseInstallations.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/GoogleDataTransport.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/GoogleAppMeasurement.xcframework/ios-arm64_armv7 \
            -framework FirebaseAnalytics \
            -framework FirebaseCore \                        
            -framework GoogleDataTransport \
            -framework GoogleAppMeasurement \
            -framework GoogleUtilities \
            -framework nanopb \
            -framework PromisesObjC \
            -framework FirebaseInstallations \
            \
    }

    HEADERS += $$PWD/src/qtfirebaseanalytics.h
    SOURCES += $$PWD/src/qtfirebaseanalytics.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}analytics.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}analytics
}

# Auth
contains(DEFINES,QTFIREBASE_BUILD_AUTH) {
    message( "QtFirebase including Auth" )

    ios: {
        LIBS += \            
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAuth/FirebaseAuth.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAuth/GTMSessionFetcher.xcframework/ios-arm64_armv7 \
            -framework FirebaseAuth \
            -framework GTMSessionFetcher \
            -framework SafariServices \ # Required for Firebase iOS >= 4.4.0
        \
    }

    HEADERS += $$PWD/src/qtfirebaseauth.h
    SOURCES += $$PWD/src/qtfirebaseauth.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}auth.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}auth
}


# Database
contains(DEFINES,QTFIREBASE_BUILD_DATABASE) {
    message( "QtFirebase including Database" )

    ios: {
        LIBS += \
            -licucore \            
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseDatabase/FirebaseDatabase.xcframework/ios-arm64_armv7 \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseDatabase/leveldb-library.xcframework/ios-arm64_armv7 \
            -framework FirebaseDatabase \
            -framework leveldb-library \
        \
    }

    HEADERS += $$PWD/src/qtfirebasedatabase.h
    SOURCES += $$PWD/src/qtfirebasedatabase.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}database.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}database
}

LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}app

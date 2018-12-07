message("QtFirebase: configuring build for supported Firebase target platform...")

isEmpty(QTFIREBASE_SDK_PATH){
    QTFIREBASE_SDK_PATH = $$PWD/firebase_cpp_sdk
    message("No QTFIREBASE_SDK_PATH path sat. Using default (firebase_cpp_sdk) $$QTFIREBASE_SDK_PATH")
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
    QTFIREBASE_STL_VARIANT = c++
    QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/$$QTFIREBASE_STL_VARIANT

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

    QTFIREBASE_FRAMEWORKS_ROOT = $$PWD/src/ios/Firebase/

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
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/AdMob \
            -framework GoogleMobileAds \
            -framework AdSupport \
            -framework StoreKit \
            \
    }

    HEADERS += $$PWD/src/qtfirebaseadmob.h
    SOURCES += $$PWD/src/qtfirebaseadmob.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/lib$${QTFIREBASE_SDK_LIBS_PREFIX}admob.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -l$${QTFIREBASE_SDK_LIBS_PREFIX}admob
}

# Remote Config
contains(DEFINES,QTFIREBASE_BUILD_REMOTE_CONFIG) {
    message( "QtFirebase including RemoteConfig" )

    ios: {
        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/RemoteConfig \
            -framework FirebaseABTesting \ # Required for Firebase iOS >= 4.5.0
            -framework FirebaseRemoteConfig \
            -framework Protobuf \
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
        message( "QtFirebase Messaging requires iOS v10.x to build. Setting QMAKE_IOS_DEPLOYMENT_TARGET = 10.0" )
        QMAKE_IOS_DEPLOYMENT_TARGET = 10.0

        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Messaging \
            -framework FirebaseMessaging \
            -framework Protobuf \
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
        LIBS += \
            -framework StoreKit \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Analytics \
            -framework FirebaseAnalytics \
            -framework FirebaseCore \
            -framework FirebaseCoreDiagnostics \
            -framework FirebaseInstanceID \
            -framework GoogleAppMeasurement \
            -framework GoogleUtilities \
            -framework nanopb \
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
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Auth \
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
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Database \
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

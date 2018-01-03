message("QtFirebase: configuring build for supported Firebase target platform...")

isEmpty(QTFIREBASE_SDK_PATH){
    QTFIREBASE_SDK_PATH = $$PWD/firebase_cpp_sdk
    message("No QTFIREBASE_SDK_PATH path sat. Using default (firebase_cpp_sdk) $$QTFIREBASE_SDK_PATH")
} else {
    message("Using QTFIREBASE_SDK_PATH ($$QTFIREBASE_SDK_PATH)")
}

QML_IMPORT_PATH += $$PWD

INCLUDEPATH += $$PWD

INCLUDEPATH += \
    $$QTFIREBASE_SDK_PATH/include \
    \

HEADERS += \
    $$PWD/src/platformutils.h \
    $$PWD/src/qtfirebase.h \
    $$PWD/src/qtfirebaseservice.h \
    $$PWD/src/ios/socialsignin.h

SOURCES += \
    $$PWD/src/qtfirebase.cpp \
    $$PWD/src/qtfirebaseservice.cpp \
    $$PWD/src/ios/socialsignin.mm

OTHER_FILES += \
    $$PWD/src/android/GoogleSignInActivity.java \
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

    contains(QTFIREBASE_CONFIG,"googleauth") {
        message("CONTAINS googleauth")
        isEmpty(android_src){
            message("ERROR: No android_src path found. Please set a path to the android src folder like below in your pro or pri:")
            message("Example: android_src = pwd/projects/project_name/android")
        } else {
            # The java file will be symlinked to your android src folder.
            contains(QMAKE_HOST.os,Windows) {
                isEmpty(QMAKE_PRE_LINK){
                    QMAKE_PRE_LINK += mklink $$shell_quote($$system_path($${android_src}/src/GoogleSignInActivity.java)) $$shell_quote($$system_path($$PWD/src/android/GoogleSignInActivity.java))
                } else {
                    QMAKE_PRE_LINK += & mklink $$shell_quote($$system_path($${android_src}/src/GoogleSignInActivity.java)) $$shell_quote($$system_path($$PWD/src/android/GoogleSignInActivity.java))
                }
            } else {
                # check if the developer has already linked something
                isEmpty(QMAKE_PRE_LINK){
                    QMAKE_PRE_LINK += ln -s $$quote($$PWD/src/android/GoogleSignInActivity.java) $$quote($${android_src}/src/GoogleSignInActivity.java)
                } else {
                    QMAKE_PRE_LINK += && ln -s $$quote($$PWD/src/android/GoogleSignInActivity.java) $$quote($${android_src}/src/GoogleSignInActivity.java)
                }
            }
        }

        HEADERS += \
            $$PWD/src/qtfirebaseintentfilter.h \
            $$PWD/src/qtfirebasejnisetup.h \
            \

        SOURCES += \
            $$PWD/src/qtfirebaseintentfilter.cpp \
            $$PWD/src/qtfirebasejnisetup.cpp \
            \
    }

    QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/gnustl

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
        \

    INCLUDEPATH += \
        $$QTFIREBASE_SDK_PATH/include \
        $$PWD/src \
        $$PWD/src/ios \
        \
}

# NOTE the order of linking is important!

PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libapp.a
LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lapp

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

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libadmob.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -ladmob
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

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libremote_config.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lremote_config
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

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libmessaging.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lmessaging
}

# Analytics
contains(DEFINES,QTFIREBASE_BUILD_ANALYTICS) {
    message( "QtFirebase including Analytics" )

    ios: {
        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Analytics \
            -framework FirebaseAnalytics \
            -framework FirebaseCore \
            -framework FirebaseCoreDiagnostics \
            -framework FirebaseInstanceID \
            -framework FirebaseNanoPB \
            -framework GoogleToolboxForMac \
            -framework nanopb \
            \
    }

    HEADERS += $$PWD/src/qtfirebaseanalytics.h
    SOURCES += $$PWD/src/qtfirebaseanalytics.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libanalytics.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lanalytics
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

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libauth.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lauth
}

# Google Auth
contains(DEFINES,QTFIREBASE_BUILD_GOOGLE_AUTH) {
    message( "QtFirebase including Google Auth" )

    ios: {
        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Auth \
            -framework FirebaseAuth \
            -framework GTMSessionFetcher \
            -framework SafariServices \ # Required for Firebase iOS >= 4.4.0
        \

        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Invites \
            -framework GoogleSignIn\
            -framework GTMOAuth2\
        \

        GOOGLEKIT_BUNDLE.files = $$QTFIREBASE_FRAMEWORKS_ROOT/Invites/Resources/GoogleSignIn.bundle
        QMAKE_BUNDLE_DATA += GOOGLEKIT_BUNDLE
    }

    HEADERS += $$PWD/src/qtfirebasegoogleauth.h
    SOURCES += $$PWD/src/qtfirebasegoogleauth.cpp

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libauth.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lauth
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

    PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libdatabase.a
    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -ldatabase
}

LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lapp


message( "QtFirebase: configuring build for supported Firebase target platform..." )

isEmpty(QTFIREBASE_SDK_PATH){
    QTFIREBASE_SDK_PATH = $$PWD/firebase_cpp_sdk
    message("No QTFIREBASE_SDK_PATH path sat. Using default (firebase_cpp_sdk) $$QTFIREBASE_SDK_PATH")
} else {
    message("Using QTFIREBASE_SDK_PATH ($$QTFIREBASE_SDK_PATH)")
}

QML_IMPORT_PATH += $$PWD

INCLUDEPATH += $$PWD

INCLUDEPATH += \
    $$QTFIREBASE_SDK_PATH/include

HEADERS += \
    $$PWD/qtfirebase.h \
    $$PWD/src/platformutils.h \
    $$PWD/src/qtfirebase.h \
    $$PWD/src/qtfirebase_plugin.h

SOURCES += \
    $$PWD/src/qtfirebase.cpp \
    $$PWD/src/qtfirebase_plugin.cpp

!ios: {
    SOURCES += \
        $$PWD/src/platformutils.cpp
}

RESOURCES += \
    $$PWD/qtfirebase.qrc

android: {
    message( "QtFirebase Android base" )
    QT += androidextras gui-private

    QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/gnustl

    DEPENDPATH += $$QTFIREBASE_SDK_LIBS_PATH
}

ios: {
    message( "QtFirebase iOS base" )
    QT += gui_private

    QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/ios/

    QTFIREBASE_FRAMEWORKS_ROOT = $$PWD/src/ios/CocoaPods/Pods

    QMAKE_IOS_DEPLOYMENT_TARGET = 7.0

    QMAKE_LFLAGS += -ObjC

    OBJECTIVE_SOURCES += \
        $$PWD/src/ios/platformutils.mm

    HEADERS += \
        $$QTFIREBASE_FRAMEWORKS_ROOT/GoogleToolboxForMac/GTMDefines.h \
        $$QTFIREBASE_FRAMEWORKS_ROOT/GoogleToolboxForMac/Foundation/GTMNSData+zlib.h \

    OBJECTIVE_SOURCES += \
        $$QTFIREBASE_FRAMEWORKS_ROOT/GoogleToolboxForMac/Foundation/GTMNSData+zlib.m

    LIBS += \
        -lsqlite3 -lz \
        #-F$$QTFIREBASE_FRAMEWORKS_ROOT/Google-Mobile-Ads-SDK/Frameworks \
        -F$$QTFIREBASE_FRAMEWORKS_ROOT/GoogleInterchangeUtilities/Frameworks/frameworks \
        -F$$QTFIREBASE_FRAMEWORKS_ROOT/GoogleSymbolUtilities/Frameworks/frameworks \
        -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseCore/Frameworks/frameworks \
        #-F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/Frameworks/frameworks \
        -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseInstanceID/Frameworks/frameworks \
        #-framework GoogleMobileAds \
        -framework GoogleInterchangeUtilities \
        -framework GoogleSymbolUtilities \
        -framework FirebaseCore \
        #-framework FirebaseAnalytics \
        -framework FirebaseInstanceID \
        -framework MediaPlayer \
        -framework CoreMotion \
        -framework CoreTelephony \
        -framework MessageUI \
        -framework GLKit \
        -framework AddressBook

    #OBJECTIVE_SOURCES += \
    #    $$PWD/ios/appdelegate.mm

    DISTFILES += \
        $$PWD/src/ios/CocoaPods/Podfile

    INCLUDEPATH += \
        $$QTFIREBASE_FRAMEWORKS_ROOT/Headers/Public \
        $$QTFIREBASE_FRAMEWORKS_ROOT/Headers/Public/FirebaseAnalytics

}

# NOTE the order of linking is important!

# Analytics
contains(DEFINES,QTFIREBASE_BUILD_ANALYTICS) {
    message( "QtFirebase including Analytics" )

    ios: {
        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/FirebaseAnalytics/Frameworks/frameworks \
            -framework FirebaseAnalytics
    }

    HEADERS += $$PWD/src/qtfirebaseanalytics.h
    SOURCES += $$PWD/src/qtfirebaseanalytics.cpp

    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lanalytics
}

# AdMob
contains(DEFINES,QTFIREBASE_BUILD_ADMOB) {
    message( "QtFirebase including AdMob" )

    ios: {
        LIBS += \
            -F$$QTFIREBASE_FRAMEWORKS_ROOT/Google-Mobile-Ads-SDK/Frameworks \
            -framework GoogleMobileAds \
    }

    HEADERS += $$PWD/src/qtfirebaseadmob.h
    SOURCES += $$PWD/src/qtfirebaseadmob.cpp

    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -ladmob
}

LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lapp


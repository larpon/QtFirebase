message( "QtFirebase: configuring stub build for non-supported Firebase target platform..." )

DEFINES += QTFIREBASE_STUB_BUILD

QTFIREBASE_STUB_PATH = $$PWD
INCLUDEPATH += $$QTFIREBASE_STUB_PATH

QML_IMPORT_PATH += $$PWD

HEADERS += \
    $$QTFIREBASE_STUB_PATH/src/qtfirebase.h \
    $$QTFIREBASE_STUB_PATH/src/qtfirebaseservice.h \
    \

contains(QTPLUGIN,qtfirebase) {
    HEADERS += $$QTFIREBASE_STUB_PATH/src/qtfirebase_plugin.h
    SOURCES += $$QTFIREBASE_STUB_PATH/src/qtfirebase_plugin.cpp
}

# Analytics
contains(DEFINES,QTFIREBASE_BUILD_ANALYTICS) {
    HEADERS += $$QTFIREBASE_STUB_PATH/src/qtfirebaseanalytics.h
}

# Messaging
contains(DEFINES,QTFIREBASE_BUILD_MESSAGING) {
    HEADERS += $$QTFIREBASE_STUB_PATH/src/qtfirebasemessaging.h
}

# AdMob
contains(DEFINES,QTFIREBASE_BUILD_ADMOB) {
    HEADERS += $$QTFIREBASE_STUB_PATH/src/qtfirebaseadmob.h
}

# Remote Config
contains(DEFINES,QTFIREBASE_BUILD_REMOTE_CONFIG) {
    HEADERS += $$QTFIREBASE_STUB_PATH/src/qtfirebaseremoteconfig.h
}

# Auth
contains(DEFINES,QTFIREBASE_BUILD_AUTH) {
    HEADERS += $$QTFIREBASE_STUB_PATH/src/qtfirebaseauth.h
}

# Database
contains(DEFINES,QTFIREBASE_BUILD_AUTH) {
    HEADERS += $$QTFIREBASE_STUB_PATH/src/qtfirebasedatabase.h
}

SOURCES += $$QTFIREBASE_STUB_PATH/src/qtfirebase.cpp

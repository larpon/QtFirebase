message( "QtFirebase: configuring build for non-supported Firebase target platform..." )

DEFINES += QTFIREBASE_FAKE_BUILD

QTFIREBASE_FAKE_PATH = $$PWD/fake
INCLUDEPATH += $$QTFIREBASE_FAKE_PATH

QML_IMPORT_PATH += $$PWD

HEADERS += \
    #$$QTFIREBASE_FAKE_PATH/qtfirebase.h \
    $$QTFIREBASE_FAKE_PATH/src/qtfirebase.h

contains(QTPLUGIN,qtfirebase) {
    HEADERS += $$QTFIREBASE_FAKE_PATH/src/qtfirebase_plugin.h
    SOURCES += $$PWD/src/qtfirebase_plugin.cpp
}

# Analytics
contains(DEFINES,QTFIREBASE_BUILD_ANALYTICS) {
    HEADERS += $$QTFIREBASE_FAKE_PATH/src/qtfirebaseanalytics.h
}

# AdMob
contains(DEFINES,QTFIREBASE_BUILD_ADMOB) {
    HEADERS += $$QTFIREBASE_FAKE_PATH/src/qtfirebaseadmob.h
}


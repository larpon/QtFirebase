TEMPLATE = lib

TARGET = qtfirebase

QT += qml quick widgets

CONFIG += qt plugin c++11

isEmpty(QTFIREBASE_SDK_PATH){
    QTFIREBASE_SDK_PATH = $$PWD/firebase_cpp_sdk
    message("No QTFIREBASE_SDK_PATH path sat. Using default (firebase_cpp_sdk) $$QTFIREBASE_SDK_PATH")
}

QML_IMPORT_PATH = $$PWD

TARGET = $$qtLibraryTarget($$TARGET)
uri = QtFirebase

DEFINES += QTFIREBASE_BUILD_ADMOB


INCLUDEPATH += \
    $$PWD \
    $$PWD/src \
    $$QTFIREBASE_SDK_PATH/include

HEADERS += \
    $$PWD/src/qtfirebase_plugin.h \
    $$PWD/src/call_once.h \
    $$PWD/src/singleton.h \
    $$PWD/src/platformutils.h \
    $$PWD/src/qtfirebase.h

SOURCES += \
    $$PWD/src/qtfirebase_plugin.cpp \
    $$PWD/src/qtfirebase.cpp

!ios: {
    SOURCES += \
        $$PWD/src/platformutils.cpp
}

RESOURCES += \
    qtfirebase.qrc



android: {
    QT += androidextras gui-private

    QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/gnustl

    LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lapp
    DEPENDPATH += $$QTFIREBASE_SDK_LIBS_PATH
}



# AdMob
contains(DEFINES,QTFIREBASE_BUILD_ADMOB) {

    HEADERS += $$PWD/src/qtfirebaseadmob.h
    SOURCES += $$PWD/src/qtfirebaseadmob.cpp

    android: {
        LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -ladmob
    }

    ios: {
        LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -ladmob
    }
}


DISTFILES = qmldir \
    qtfirebase.pri \
    LICENSE

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

# Add the path to your firebase c++ lib on QTFIREBASE_SDK_PATH to your pro / pri file
# example: QTFIREBASE_SDK_PATH = /home/user/Projects/libraries/firebase/firebase_cpp_sdk/

QTFIREBASE_VERSION = 1.5.0
exists(.git) {
    include(qtfirebase_version.pri)
    message("QtFirebase $$QTFIREBASE_VERSION git $$QTFIREBASE_GIT_VERSION/$$QTFIREBASE_GIT_BRANCH")
}

!contains(QTFIREBASE_CONFIG,"noautoregister") {
    DEFINES += QTFIREBASE_AUTO_REGISTER
    HEADERS += $$PWD/qtfirebase_register.h
    SOURCES += $$PWD/qtfirebase_register.cpp
}

contains(QTFIREBASE_CONFIG,"analytics") {
    DEFINES += QTFIREBASE_BUILD_ANALYTICS
}

contains(QTFIREBASE_CONFIG,"messaging") {
    DEFINES += QTFIREBASE_BUILD_MESSAGING
}

contains(QTFIREBASE_CONFIG,"admob") {
    DEFINES += QTFIREBASE_BUILD_ADMOB
}

contains(QTFIREBASE_CONFIG,"remote_config") {
    DEFINES += QTFIREBASE_BUILD_REMOTE_CONFIG
}

contains(QTFIREBASE_CONFIG,"auth") {
    DEFINES += QTFIREBASE_BUILD_AUTH
}

contains(QTFIREBASE_CONFIG,"database") {
    DEFINES += QTFIREBASE_BUILD_DATABASE
}


DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md

# Currently supported Firebase targets
android|ios {
    include(qtfirebase_target.pri)
} else {
    include(stub/qtfirebase_stub.pri)
}

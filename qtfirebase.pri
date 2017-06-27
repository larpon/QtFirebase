
!contains(QTFIREBASE_CONFIG,"noautoregister") {
    DEFINES += QTFIREBASE_AUTO_REGISTER
    HEADERS += $$PWD/qtfirebase_register.h
    SOURCES += $$PWD/qtfirebase_register.cpp
}

contains(QTFIREBASE_CONFIG,"analytics") {
    DEFINES += QTFIREBASE_BUILD_ANALYTICS
}

contains(QTFIREBASE_CONFIG,"admob") {
    DEFINES += QTFIREBASE_BUILD_ADMOB
}

contains(QTFIREBASE_CONFIG,"remote_config") {
    DEFINES += QTFIREBASE_BUILD_REMOTE_CONFIG
}


DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md

# Currently supported Firebase targets
android|ios {
    include(qtfirebase_target.pri)
} else {
    include(qtfirebase_dummy.pri)
}

exists(.git) {
    unix {
        GIT_BRANCH_NAME = $$system(git rev-parse --abbrev-ref HEAD)
        message("QtFirebase branch $$GIT_BRANCH_NAME")
    }
}

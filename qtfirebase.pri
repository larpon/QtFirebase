contains(QTFIREBASE_CONFIG,"analytics") {
    DEFINES += QTFIREBASE_BUILD_ANALYTICS
}

contains(QTFIREBASE_CONFIG,"admob") {
    DEFINES += QTFIREBASE_BUILD_ADMOB
}

# Currently supported Firebase targets
android|ios {
    include(qtfirebase_target.pri)
} else {
    include(qtfirebase_dummy.pri)
}

#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#include <QDebug>
#include <QWindow>
#include <QGuiApplication>

#if defined(Q_OS_ANDROID)
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include "jni.h"
#endif

#if defined(Q_OS_IOS)
#include <qpa/qplatformnativeinterface.h>
#endif

class PlatformUtils
{
public:
    PlatformUtils();

    static bool  googleServiceAvailable();

    #if defined(Q_OS_IOS)
    static void* getNativeWindow();
    #elif defined(Q_OS_ANDROID)
    static jobject getNativeWindow();
    #else
    static void getNativeWindow();
    #endif
};

#endif // PLATFORMUTILS_H

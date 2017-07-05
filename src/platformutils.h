#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#include <QDebug>
#include <QWindow>
#include <QGuiApplication>

#if defined(__ANDROID__)
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <jni.h>
#endif

#if defined(__ANDROID__) || defined(Q_OS_IOS)
#include <qpa/qplatformnativeinterface.h>
#endif

class PlatformUtils
{
public:
    PlatformUtils();

    #if defined(Q_OS_IOS)
    static void* getNativeWindow();
    #elif defined(__ANDROID__)
    static jobject getNativeWindow();
    #else
    static void getNativeWindow();
    #endif
};

#endif // PLATFORMUTILS_H

#include "platformutils.h"

PlatformUtils::PlatformUtils()
{

}

#if defined(__ANDROID__)
jobject PlatformUtils::getNativeWindow()
{
    QPlatformNativeInterface *interface = QGuiApplication::platformNativeInterface();

    jobject activity = (jobject)interface->nativeResourceForIntegration("QtActivity");

    // Another way?
    //jobject activity = (jobject)QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");

    return activity;
}

#else
void PlatformUtils::getNativeWindow()
{
    qDebug() << "Not available";
}
#endif

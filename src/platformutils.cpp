#include "platformutils.h"

PlatformUtils::PlatformUtils()
{

}

#if defined(Q_OS_ANDROID)
jobject PlatformUtils::getNativeWindow()
{
    QAndroidJniEnvironment env;

    QAndroidJniObject activity = QtAndroid::androidActivity();

    jobject globalActivity = env->NewGlobalRef(activity.object());

    return globalActivity;
}
#else
void PlatformUtils::getNativeWindow()
{
    qDebug() << "Not available";
}
#endif

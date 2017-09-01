#include "platformutils.h"

#include <QDebug>

PlatformUtils::PlatformUtils()
{

}

GoogleServiceAvailability PlatformUtils::getGoogleServiceAvailability()
{
#if defined(Q_OS_ANDROID)
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();

    auto availablity = ::google_play_services::CheckAvailability(env, activity.object());
    qDebug() << "PlatformUtils::googleServiceAvailable() result :" << availablity << " (0 is kAvailabilityAvailable)";
    return GoogleServiceAvailability(availablity);
#endif
    return GoogleServiceUnavailableOther;
}

bool PlatformUtils::googleServicesAvailable()
{

#if defined(Q_OS_ANDROID)
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();

    auto availablity = ::google_play_services::CheckAvailability(env, activity.object());
    qDebug() << "PlatformUtils::googleServicesAvailable() result :" << availablity << " (0 is kAvailabilityAvailable)";
    return ::google_play_services::kAvailabilityAvailable == availablity;
#endif
    return false;
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

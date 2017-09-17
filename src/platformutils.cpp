#include "platformutils.h"

/*
 * Google Play Services (only available under Android)
 */
#if defined(Q_OS_ANDROID)
GooglePlayServices::Availability GooglePlayServices::getAvailability()
{
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();

    auto availablity = ::google_play_services::CheckAvailability(env, activity.object());
    qDebug() << "GooglePlayServices::getAvailability result :" << availablity << " (0 is kAvailabilityAvailable)";
    return Availability(availablity);
}

bool GooglePlayServices::available()
{
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();

    auto availablity = ::google_play_services::CheckAvailability(env, activity.object());
    qDebug() << "GooglePlayServices::available() result :" << availablity << " (0 is kAvailabilityAvailable)";
    return ::google_play_services::kAvailabilityAvailable == availablity;
}
#endif

/*
 * PlatformUtils
 */
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

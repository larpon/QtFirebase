#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <QDebug>
#include <QWindow>
#include <QGuiApplication>

#if defined(Q_OS_ANDROID)
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include "google_play_services/availability.h"
#endif

#if defined(Q_OS_IOS)
#include <qpa/qplatformnativeinterface.h>
#endif

/*
 * Google Play Services (currently only available under Android)
 * TODO move to separate .h/.cpp and expose as QML type (+ dummies on unsupported platforms)
 */
#if defined(Q_OS_ANDROID)
class GooglePlayServices
{
public:
    enum Availability
    {
          /// Google Play Services are available.
          AvailabilityAvailable = ::google_play_services::kAvailabilityAvailable,

          /// Google Play Services is disabled in Settings.
          AvailabilityUnavailableDisabled = ::google_play_services::kAvailabilityUnavailableDisabled,

          /// Google Play Services is invalid.
          AvailabilityUnavailableInvalid = ::google_play_services::kAvailabilityUnavailableInvalid,

          /// Google Play Services is not installed.
          AvailabilityUnavailableMissing = ::google_play_services::kAvailabilityUnavailableMissing,

          /// Google Play Services does not have the correct permissions.
          AvailabilityUnavailablePermissions = ::google_play_services::kAvailabilityUnavailablePermissions,

          /// Google Play Services need to be updated.
          AvailabilityUnavailableUpdateRequired = ::google_play_services::kAvailabilityUnavailableUpdateRequired,

          /// Google Play Services is currently updating.
          AvailabilityUnavailableUpdating = ::google_play_services::kAvailabilityUnavailableUpdating,

          /// Some other error occurred.
          AvailabilityUnavailableOther = ::google_play_services::kAvailabilityUnavailableOther,
    };

    static Availability getAvailability();
    static bool available();
};
#endif

/*
 * PlatformUtils
 */
class PlatformUtils
{
public:

    #if defined(Q_OS_IOS)
    PlatformUtils();
    ~PlatformUtils();
    static void* getNativeWindow();
    #elif defined(Q_OS_ANDROID)
    static jobject getNativeWindow();
    #else
    static void getNativeWindow();
    #endif

private:
    #if defined(Q_OS_ANDROID)
    static jobject nativeWindow;
    #endif
};

#endif // PLATFORM_UTILS_H

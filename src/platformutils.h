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

#include "google_play_services/availability.h"

enum GoogleServiceAvailability
{
      /// Gooogle Play services are available.
      GoogleServiceAvailable = ::google_play_services::kAvailabilityAvailable,

      /// Google Play services is disabled in Settings.
      GoogleServiceUnavailableDisabled = ::google_play_services::kAvailabilityUnavailableDisabled,

      /// Google Play services is invalid.
      GoogleServiceUnavailableInvalid = ::google_play_services::kAvailabilityUnavailableInvalid,

      /// Google Play services is not installed.
      GoogleServiceUnavailableMissing = ::google_play_services::kAvailabilityUnavailableMissing,

      /// Google Play services does not have the correct permissions.
      GoogleServiceUnavailablePermissions = ::google_play_services::kAvailabilityUnavailablePermissions,

      /// Google Play services need to be updated.
      GoogleServiceUnavailableUpdateRequired = ::google_play_services::kAvailabilityUnavailableUpdateRequired,

      /// Google Play services is currently updating.
      GoogleServiceUnavailableUpdating = ::google_play_services::kAvailabilityUnavailableUpdating,

      /// Some other error occurred.
      GoogleServiceUnavailableOther = ::google_play_services::kAvailabilityUnavailableOther,
};

class PlatformUtils
{
public:
    PlatformUtils();

    static GoogleServiceAvailability  getGoogleServiceAvailability();
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

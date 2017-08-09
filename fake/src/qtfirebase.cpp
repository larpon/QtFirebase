#include "qtfirebaseanalytics.h"
#include "qtfirebaseremoteconfig.h"
#include "qtfirebaseadmob.h"

#ifdef QTFIREBASE_BUILD_ANALYTICS
QtFirebaseAnalytics* QtFirebaseAnalytics::self = nullptr;
#endif

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG
QtFirebaseRemoteConfig* QtFirebaseRemoteConfig::self = nullptr;
#endif

#ifdef QTFIREBASE_BUILD_ADMOB
QtFirebaseAdMob *QtFirebaseAdMob::self = nullptr;
#endif

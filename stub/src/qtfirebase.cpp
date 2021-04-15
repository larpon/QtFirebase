#include "qtfirebaseanalytics.h"
#include "qtfirebaseremoteconfig.h"
#include "qtfirebaseadmob.h"
#include "qtfirebaseauth.h"
#include "qtfirebasedatabase.h"
#include "qtfirebasemessaging.h"

#ifdef QTFIREBASE_BUILD_ANALYTICS
QtFirebaseAnalytics* QtFirebaseAnalytics::self = nullptr;
#endif

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG
QtFirebaseRemoteConfig* QtFirebaseRemoteConfig::self = nullptr;
#endif

#ifdef QTFIREBASE_BUILD_ADMOB
QtFirebaseAdMob *QtFirebaseAdMob::self = nullptr;
#endif

#ifdef QTFIREBASE_BUILD_AUTH
QtFirebaseAuth *QtFirebaseAuth::self = nullptr;
#endif

#ifdef QTFIREBASE_BUILD_DATABASE
QtFirebaseDatabase *QtFirebaseDatabase::self = nullptr;
#endif

#ifdef QTFIREBASE_BUILD_MESSAGING
QtFirebaseMessaging *QtFirebaseMessaging::self = nullptr;
#endif

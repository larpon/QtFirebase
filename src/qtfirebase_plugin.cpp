#include "qtfirebase_plugin.h"

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ANALYTICS)
#include "src/qtfirebaseanalytics.h"
# endif // QTFIREBASE_BUILD_ANALYTICS

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_MESSAGING)
#include "src/qtfirebasemessaging.h"
# endif // QTFIREBASE_BUILD_MESSAGING


#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ADMOB)
#include "src/qtfirebaseadmob/qtfirebaseadmob.h"
# endif // QTFIREBASE_BUILD_ADMOB

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_REMOTE_CONFIG)
#include "src/qtfirebaseremoteconfig.h"
# endif // QTFIREBASE_BUILD_REMOTE_CONFIG

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_AUTH)
#include "src/qtfirebaseauth.h"
# endif // QTFIREBASE_BUILD_AUTH

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_DATABASE)
#include "src/qtfirebasedatabase.h"
# endif // QTFIREBASE_BUILD_DATABASE

#include <qqml.h>

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_DATABASE)
static QObject *QtFirebaseDatabaseProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return qFirebaseDatabase;
}
#endif


void QtFirebasePlugin::registerTypes(const char *uri)
{
    // @uri QtFirebase

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ANALYTICS)
    qmlRegisterType<QtFirebaseAnalytics>(uri, 1, 0, "Analytics");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_MESSAGING)
    qmlRegisterType<QtFirebaseMessaging>(uri, 1, 0, "Messaging");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ADMOB)
    qmlRegisterType<QtFirebaseAdMob>(uri, 1, 0, "AdMob");
    qmlRegisterType<QtFirebaseAdMobRequest>(uri, 1, 0, "AdMobRequest");
    qmlRegisterType<QtFirebaseAdMobBanner>(uri, 1, 0, "AdMobBanner");
    qmlRegisterType<QtFirebaseAdMobInterstitial>(uri, 1, 0, "AdMobInterstitial");
    qmlRegisterType<QtFirebaseAdMobRewardedVideoAd>(uri, 1, 0, "AdMobRewardedVideoAd");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_REMOTE_CONFIG)
    qmlRegisterType<QtFirebaseRemoteConfig>(uri, 1, 0, "RemoteConfig");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_AUTH)
    qmlRegisterType<QtFirebaseAuth>(uri, 1, 0, "Auth");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_DATABASE)
    qmlRegisterSingletonType<QtFirebaseDatabase>(uri, 1, 0, "Database", QtFirebaseDatabaseProvider);
    qmlRegisterUncreatableType<QtFirebaseDatabaseQuery>(uri, 1, 0, "DatabaseQuery", "Get query object from DbRequest, do not create it");
    qmlRegisterType<QtFirebaseDatabaseRequest>(uri, 1, 0, "DatabaseRequest");
    qmlRegisterUncreatableType<QtFirebaseDataSnapshot>(uri, 1, 0, "DataSnapshot", "Get snapshot object from DbRequest, do not create it");
#endif
}


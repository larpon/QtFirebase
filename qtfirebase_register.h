#ifndef QTFIREBASE_REGISTER_TYPES_H
#define QTFIREBASE_REGISTER_TYPES_H

#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>

#include "qqml.h"

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ANALYTICS)

#if defined(QTFIREBASE_STUB_BUILD)
#include "stub/src/qtfirebaseanalytics.h"
#else
#include "src/qtfirebaseanalytics.h"
#endif

# endif // QTFIREBASE_BUILD_ANALYTICS

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_MESSAGING)

#if defined(QTFIREBASE_STUB_BUILD)
#include "stub/src/qtfirebasemessaging.h"
#else
#include "src/qtfirebasemessaging.h"
#endif

# endif // QTFIREBASE_BUILD_MESSAGING

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ADMOB)

#if defined(QTFIREBASE_STUB_BUILD)
#include "stub/src/qtfirebaseadmob.h"
#else
#include "src/qtfirebaseadmob/qtfirebaseadmob.h"
#include "src/qtfirebaseadmob/qtfirebaseadmobrequest.h"
#include "src/qtfirebaseadmob/qtfirebaseadmobbanner.h"
#include "src/qtfirebaseadmob/qtfirebaseadmobinterstitial.h"
#include "src/qtfirebaseadmob/qtfirebaseadmobrewardedvideoad.h"
#endif

# endif // QTFIREBASE_BUILD_ADMOB


#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_REMOTE_CONFIG)

#if defined(QTFIREBASE_STUB_BUILD)
#include "stub/src/qtfirebaseremoteconfig.h"
#else
#include "src/qtfirebaseremoteconfig.h"
#endif

# endif // QTFIREBASE_BUILD_REMOTE_CONFIG


#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_AUTH)

#if defined(QTFIREBASE_STUB_BUILD)
#include "stub/src/qtfirebaseauth.h"
#else
#include "src/qtfirebaseauth.h"
#endif

# endif // QTFIREBASE_BUILD_AUTH


#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_DATABASE)

#if defined(QTFIREBASE_STUB_BUILD)
#include "stub/src/qtfirebasedatabase.h"
#else
#include "src/qtfirebasedatabase.h"
#endif

static QObject *QtFirebaseDatabaseProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return qFirebaseDatabase;
}

#endif // QTFIREBASE_BUILD_DATABASE

static void registerQtFirebase() {

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ANALYTICS)
    qmlRegisterType<QtFirebaseAnalytics>("QtFirebase", 1, 0, "Analytics");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_MESSAGING)
    qmlRegisterType<QtFirebaseMessaging>("QtFirebase", 1, 0, "Messaging");
#endif
    
#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ADMOB)
    qmlRegisterType<QtFirebaseAdMob>("QtFirebase", 1, 0, "AdMob");
    qmlRegisterType<QtFirebaseAdMobRequest>("QtFirebase", 1, 0, "AdMobRequest");
    qmlRegisterType<QtFirebaseAdMobBanner>("QtFirebase", 1, 0, "AdMobBanner");
    qmlRegisterType<QtFirebaseAdMobInterstitial>("QtFirebase", 1, 0, "AdMobInterstitial");
    qmlRegisterType<QtFirebaseAdMobRewardedVideoAd>("QtFirebase", 1, 0, "AdMobRewardedVideoAd");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_REMOTE_CONFIG)
    qmlRegisterType<QtFirebaseRemoteConfig>("QtFirebase", 1, 0, "RemoteConfig");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_AUTH)
    qmlRegisterType<QtFirebaseAuth>("QtFirebase", 1, 0, "Auth");
#endif

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_DATABASE)
    qmlRegisterSingletonType<QtFirebaseDatabase>("QtFirebase", 1, 0, "Database", QtFirebaseDatabaseProvider);
    qmlRegisterUncreatableType<QtFirebaseDatabaseQuery>("QtFirebase", 1, 0, "DatabaseQuery", "Get query object from DatabaseRequest, do not create it");
    qmlRegisterType<QtFirebaseDatabaseRequest>("QtFirebase", 1, 0, "DatabaseRequest");
    qmlRegisterUncreatableType<QtFirebaseDataSnapshot>("QtFirebase", 1, 0, "DataSnapshot", "Get snapshot object from DatabaseRequest, do not create it");
#endif
}

Q_COREAPP_STARTUP_FUNCTION(registerQtFirebase)

#endif // QTFIREBASE_REGISTER_TYPES_H

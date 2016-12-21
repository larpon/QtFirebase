#ifndef QTFIREBASE_REGISTER_TYPES_H
#define QTFIREBASE_REGISTER_TYPES_H

#include <QQmlEngine>
#include <QJSEngine>

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ANALYTICS)
#include "src/qtfirebaseanalytics.h"
# endif // QTFIREBASE_BUILD_ANALYTICS

#if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ADMOB)
#include "src/qtfirebaseadmob.h"
# endif // QTFIREBASE_BUILD_ADMOB

#include "qqml.h"

class QtFirebaseRegisterHelper {

public:
    QtFirebaseRegisterHelper() {
        #if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ANALYTICS)
        qmlRegisterType<QtFirebaseAnalytics>("QtFirebase", 1, 0, "Analytics");
        #endif

        #if defined(QTFIREBASE_BUILD_ALL) || defined(QTFIREBASE_BUILD_ADMOB)
        qmlRegisterType<QtFirebaseAdMob>("QtFirebase", 1, 0, "AdMob");
        qmlRegisterType<QtFirebaseAdMobRequest>("QtFirebase", 1, 0, "AdMobRequest");
        qmlRegisterType<QtFirebaseAdMobBanner>("QtFirebase", 1, 0, "AdMobBanner");
        qmlRegisterType<QtFirebaseAdMobInterstitial>("QtFirebase", 1, 0, "AdMobInterstitial");
        #endif
    }
};

static QtFirebaseRegisterHelper qtFirebaseRegisterHelper;

#endif // QTFIREBASE_REGISTER_TYPES_H

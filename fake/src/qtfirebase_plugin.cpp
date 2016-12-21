#include "qtfirebase_plugin.h"

#include "src/qtfirebaseadmob.h"

#include <qqml.h>

void QtFirebasePlugin::registerTypes(const char *uri)
{
    // @uri QtFirebase
    qmlRegisterType<QtFirebaseAdMob>(uri, 1, 0, "AdMob");
    qmlRegisterType<QtFirebaseAdMobBanner>(uri, 1, 0, "AdMobBanner");
    qmlRegisterType<QtFirebaseAdMobInterstitial>(uri, 1, 0, "AdMobInterstitial");
}


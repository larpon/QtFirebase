#include "qtfirebaseintentfilter.h"

using namespace FirebaseIntent;

FirebaseIntentfilter::FirebaseIntentfilter(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_ANDROID
    // so we can pass that around through QMetaObject::invokeMethod
    qRegisterMetaType<QAndroidJniObject>("QAndroidJniObject");
#endif
}

FirebaseIntentfilter &FirebaseIntentfilter::instance()
{
    static FirebaseIntentfilter s_instance;
    return s_instance;
}

#include <QObject>
#include <QDebug>

#include <QAndroidJniObject>
#include <jni.h>

#include "src/qtfirebaseintentfilter.h"

extern "C"
{

void Java_de_vonaffenfels_Mobile_ApeActivity_googleAuthIntent(JNIEnv *, jobject, jobject intent)
{
    // Not passing a jobject around otherwise it crashes with "accessed stale local reference"
    // Also since we don't do that that often here (compared to ad above) the performance impact is negligible
    QMetaObject::invokeMethod(&FirebaseIntent::FirebaseIntentfilter::instance(), "googleAuthIntent", Qt::AutoConnection,
                              Q_ARG(QAndroidJniObject, QAndroidJniObject(intent)));
}

void Java_de_vonaffenfels_Mobile_ApeActivity_googleAuthActivityResult(JNIEnv *, jobject, jint requestCode, jint resultCode, jobject data)
{
    // Not passing a jobject around otherwise it crashes with "accessed stale local reference"
    QMetaObject::invokeMethod(&FirebaseIntent::FirebaseIntentfilter::instance(),
                              "googleAuthActivityResult", Qt::AutoConnection,
                              Q_ARG(int, requestCode),
                              Q_ARG(int, resultCode),
                              Q_ARG(QAndroidJniObject, QAndroidJniObject(data)));
}

void Java_com_blackgrain_android_googleauth_GoogleSignInActivity_setCurrentUser(JNIEnv *, jobject, jstring token)
{
    QMetaObject::invokeMethod(&FirebaseIntent::FirebaseIntentfilter::instance(),
                              "setCurrentUser", Qt::AutoConnection,
                              Q_ARG(QString, QAndroidJniObject(token).toString()));
}

void Java_com_blackgrain_android_googleauth_GoogleSignInActivity_signedOut(JNIEnv *, jobject)
{
    QMetaObject::invokeMethod(&FirebaseIntent::FirebaseIntentfilter::instance(),
                              "signedOut", Qt::AutoConnection);
}

} // extern C

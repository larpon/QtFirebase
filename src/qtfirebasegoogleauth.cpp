#include "qtfirebasegoogleauth.h"

#include <QDebug>
#include <QtAndroid>
#include <QAndroidJniEnvironment>

#include "src/qtfirebaseintentfilter.h"

using namespace FirebaseIntent;

namespace auth = ::firebase::auth;

QtFirebaseGoogleAuth *QtFirebaseGoogleAuth::self = 0;

QtFirebaseGoogleAuth::QtFirebaseGoogleAuth(QObject *parent) : QtFirebaseService(parent),
    m_auth(nullptr)
{
    if(self == 0)
    {
        self = this;
        qDebug() << self << "::QtFirebaseGoogleAuth" << "singleton";
    }

    connect(&FirebaseIntentfilter::instance(), &FirebaseIntentfilter::googleAuthActivityResult, this, [this](int requestCode, int resultCode, const QAndroidJniObject &data) {
        m_javaGoogleAuth.callMethod<void>("activityResult",
                                          "(Landroid/content/Intent;)V",
                                          data.object());
    });

    connect(&FirebaseIntentfilter::instance(), &FirebaseIntentfilter::setCurrentUser, this, [this](QString token) {
        qDebug() << "FirebaseIntentfilter::setCurrentUser";
        firebaseSignIn(token);
    });

    connect(&FirebaseIntentfilter::instance(), &FirebaseIntentfilter::signedOut, this, [this]() {
        qDebug() << "FirebaseIntentfilter::signedOut";
        // todo
    });
}

void QtFirebaseGoogleAuth::init()
{
    if(!qFirebase->ready()) {
        qDebug() << self << "::init" << "base not ready";
        return;
    }
}

void QtFirebaseGoogleAuth::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID + QStringLiteral(".auth")))
        return;

    qDebug() << "onFutureEvent"  << (__QTFIREBASE_ID + QStringLiteral(".auth"));
}

void QtFirebaseGoogleAuth::login()
{
    if(m_clientId.isEmpty()){
        // todo give info to set a client id
        qDebug() << "give info to set a client id";
        return;
    }

    if(!initJava()){
        // todo give info java is not initialized correctly
        qDebug() << "todo give info java is not initialized correctly";
        return;
    } else {
        m_javaGoogleAuth.callMethod<void>("login");
    }
}

void QtFirebaseGoogleAuth::logout()
{
    if(!initJava()){
        // todo give info java is not initialized correctly
        return;
    }

    if (m_javaGoogleAuth.isValid()) {
        m_javaGoogleAuth.callMethod<void>("logout");
    } else {
        // todo?
    }
}

bool QtFirebaseGoogleAuth::initJava()
{
    qDebug() << "QtFirebaseGoogleAuth::initJava";

    QAndroidJniEnvironment env;

    if (!m_javaGoogleAuth.isValid()) {

        m_javaGoogleAuth = QAndroidJniObject("com/blackgrain/android/googleauth/GoogleSignInActivity",
                                             "(Ljava/lang/String;JLandroid/app/Activity;)V",
                                             QAndroidJniObject::fromString(m_clientId).object(),
                                             jlong(static_cast<QObject *>(this)),
                                             QtAndroid::androidActivity().object());

        if (env->ExceptionCheck()) {
            qWarning() << "ExceptionCheck failed: com/blackgrain/android/googleauth/GoogleSignInActivity";
            env->ExceptionDescribe();
            env->ExceptionClear();
            return false;
        }

        if (!m_javaGoogleAuth.isValid()) {
            qWarning() << "No exception while creating Android Google Auth but we still got a null object, something's wrong here";
            return false;
        } else {
            return true;
        }
    } else {
        return true;
    }
}

void QtFirebaseGoogleAuth::firebaseSignIn(QString googleToken)
{
    if(!m_auth) {
        firebase::App* app = qFirebase->firebaseApp();
        firebase::InitResult* ir = nullptr;
        m_auth = firebase::auth::Auth::GetAuth(app, ir);
    }

    qDebug() << "------------------------------------------------------------------";

    auth::Credential credential = auth::GoogleAuthProvider::GetCredential(googleToken.toUtf8(), nullptr);

    m_result = m_auth->SignInWithCredential(credential);

    m_result.OnCompletion([this](const firebase::Future<firebase::auth::User*>& result){
        m_user = m_auth->current_user();
        qDebug() << "firebaseSignIn" << QString::fromStdString(m_user->display_name());
        qDebug() << "firebaseSignIn" << QString::fromStdString(m_user->email());
        qDebug() << "firebaseSignIn" << m_user->is_email_verified();
        qDebug() << "firebaseSignIn" << QString::fromStdString(m_user->phone_number());
        qDebug() << "firebaseSignIn" << QString::fromStdString(m_user->photo_url());
        qDebug() << "firebaseSignIn" << QString::fromStdString(m_user->uid());
        qDebug() << "firebaseSignIn" << QString::fromStdString(m_user->provider_id());
    });
    }

    void QtFirebaseGoogleAuth::setClientId(const QString &clientId)
    {
        if (m_clientId != clientId) {
            m_clientId = clientId;
            emit clientIdChanged();
        }
    }

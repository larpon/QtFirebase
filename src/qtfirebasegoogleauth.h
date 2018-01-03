#ifndef QTFIREBASE_GOOGLE_AUTH_H
#define QTFIREBASE_GOOGLE_AUTH_H

#include "qtfirebaseservice.h"
#include "firebase/app.h"
#include "firebase/auth.h"

#ifdef QTFIREBASE_GOOGLE_AUTH_H
#include "src/qtfirebase.h"
#if defined(qFirebaseGoogleAuth)
#undef qFirebaseGoogleAuth
#endif
#define qFirebaseGoogleAuth (static_cast<QtFirebaseGoogleAuth *>(QtFirebaseGoogleAuth::instance()))

// TODOS Android https://developers.google.com/+/web/api/rest/oauth
// Firebase console enable logins

namespace auth = ::firebase::auth;

class QtFirebaseGoogleAuth : public QtFirebaseService
{
    Q_OBJECT
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)

public:
    static QtFirebaseGoogleAuth *instance()
    {
        if(self == nullptr)
        {
            self = new QtFirebaseGoogleAuth(0);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    Q_INVOKABLE void login();
    Q_INVOKABLE void logout();

    bool initJava();
    void firebaseSignIn(QString googleToken);

    QString clientId() const { return m_clientId; }
    void setClientId(const QString &clientId);

    // photoUrl
    QString photoUrl;

protected:
    explicit QtFirebaseGoogleAuth(QObject *parent = 0);

signals:
    void clientIdChanged();

private:
    void init() override;
    void onFutureEvent(QString eventId, firebase::FutureBase future) override;

    static QtFirebaseGoogleAuth *self;
    firebase::auth::Auth* m_auth;
    firebase::Future<auth::User*> m_result;
    firebase::auth::User* m_user;

   // QAndroidJniObject m_javaGoogleAuth;

    QString m_clientId;
    bool isLoggedIn = false;

    Q_DISABLE_COPY(QtFirebaseGoogleAuth)
};

#endif //QTFIREBASE_GOOGLE_AUTH_H

#endif // QTFIREBASE_GOOGLE_AUTH_H

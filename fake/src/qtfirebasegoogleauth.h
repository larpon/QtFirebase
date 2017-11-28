#ifndef QTFIREBASE_GOOGLE_AUTH_H
#define QTFIREBASE_GOOGLE_AUTH_H
#include <QObject>
#include <QAndroidJniObject>

#ifdef QTFIREBASE_BUILD_GOOGLE_AUTH
#include "src/qtfirebase.h"
#if defined(qFirebaseGoogleAuth)
#undef qFirebaseGoogleAuth
#endif
#define qFirebaseGoogleAuth (static_cast<QtFirebaseGoogleAuth *>(QtFirebaseGoogleAuth::instance()))

class QtFirebaseGoogleAuth: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)

public:
    static QtFirebaseGoogleAuth *instance()
    {
        if(self == nullptr)
        {
            self = new QtFirebaseGoogleAuth(0);
        }
        return self;
    }

public:
    Q_INVOKABLE void login();

    QString clientId() const { return QStringLiteral(""); }
    void setClientId(const QString &clientId) { Q_UNUSED(clientId); }

signals:
    void clientIdChanged();


protected:
    static QtFirebaseGoogleAuth *self;
    explicit QtFirebaseGoogleAuth(QObject *parent = 0){Q_UNUSED(parent);}
    Q_DISABLE_COPY(QtFirebaseGoogleAuth)
};

#endif //QTFIREBASE_BUILD_GOOGLE_AUTH

#endif // QTFIREBASE_GOOGLE_AUTH_H

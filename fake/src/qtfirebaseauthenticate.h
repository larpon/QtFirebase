#ifndef QTFIREBASE_AUTHENTICATE_H
#define QTFIREBASE_AUTHENTICATE_H

#ifdef QTFIREBASE_BUILD_AUTHENTICATE

#include "src/qtfirebase.h"

#if defined(qFirebaseAuthenticate)
#undef qFirebaseAuthenticate
#endif
#define qFirebaseAuthenticate (static_cast<QtFirebaseAuthenticate*>(QtFirebaseAuthenticate::instance()))

#include <QObject>
#include <QVariantMap>
#include <QVariantList>

class QtFirebaseAuthenticate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit QtFirebaseAuthenticate(QObject* parent = 0) { Q_UNUSED(parent); }
    ~QtFirebaseAuthenticate() {}

    static QtFirebaseAuthenticate *instance() {
        if(self == 0) {
            self = new QtFirebaseAuthenticate(0);
        }
        return self;
    }

    enum AuthenticationService
    {
        Google,
        Facebook,
        Github,
        Twitter
    };
    Q_ENUM(AuthenticationService)

    bool checkInstance(const char *function) { Q_UNUSED(function); return false; }

    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready); }

    bool busy() { return false; }
    void setBusy(bool busy) { Q_UNUSED(busy); }

    Q_INVOKABLE void login(AuthenticationService loginService);

private slots:
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);

signals:
    void readyChanged();
    void busyChanged();

private:
    static QtFirebaseAuthenticate *self;
    Q_DISABLE_COPY(QtFirebaseAuthenticate)
};

#endif // QTFIREBASE_BUILD_AUTHENTICATE
#endif // QTFIREBASE_AUTHENTICATE_H

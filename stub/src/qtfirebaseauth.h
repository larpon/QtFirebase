#ifndef QTFIREBASE_AUTH_H
#define QTFIREBASE_AUTH_H
#include <QObject>

#ifdef QTFIREBASE_BUILD_AUTH
#include "qtfirebase.h"
#if defined(qFirebaseAuth)
#undef qFirebaseAuth
#endif
#define qFirebaseAuth (static_cast<QtFirebaseAuth *>(QtFirebaseAuth::instance()))

class QtFirebaseAuth: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool signedIn READ signedIn NOTIFY signedInChanged)
public:
    static QtFirebaseAuth *instance()
    {
        if(self == nullptr)
        {
            self = new QtFirebaseAuth(nullptr);
        }
        return self;
    }
    enum Error
    {
        AuthErrorNone,
        AuthErrorUnimplemented,
        AuthErrorFailure
    };
    Q_ENUM(Error)

    enum Action
    {
        AuthActionRegister,
        AuthActionSignIn,
        AuthActionSignOut
    };
    Q_ENUM(Action)


public slots:
    //Control
    void registerUser(const QString& email, const QString& pass){ Q_UNUSED(email) Q_UNUSED(pass) }
    void signIn(const QString& email, const QString& pass){ Q_UNUSED(email) Q_UNUSED(pass) }
    void signOut(){}
    void sendPasswordResetEmail(const QString& email){ Q_UNUSED(email) }
    void deleteUser(){}

    //Status
    bool signedIn() const{return false;}
    bool running() const{return false;}
    int errorId() const{return AuthErrorNone;}
    QString errorMsg() const{return QString();}

    //Data
    QString email() const{return QString();}
    QString displayName() const{return QString();}
    bool emailVerified() const{return false;}
    QString photoUrl() const{return QString();}
    QString uid() const{return QString();}
signals:
    void signedInChanged();
    void runningChanged();
    void completed(bool success, int actionId);
    void passwordResetEmailSent();

protected:
    static QtFirebaseAuth *self;
    explicit QtFirebaseAuth(QObject *parent = nullptr){ Q_UNUSED(parent) }
    Q_DISABLE_COPY(QtFirebaseAuth)

};

#endif //QTFIREBASE_BUILD_AUTH

#endif // QTFIREBASE_AUTH_H

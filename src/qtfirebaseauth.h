#ifndef QTFIREBASE_AUTH_H
#define QTFIREBASE_AUTH_H

#include "qtfirebaseservice.h"
#include "firebase/auth.h"

#ifdef QTFIREBASE_BUILD_AUTH
#include "src/qtfirebase.h"
#if defined(qFirebaseAuth)
#undef qFirebaseAuth
#endif
#define qFirebaseAuth (static_cast<QtFirebaseAuth *>(QtFirebaseAuth::instance()))

class QtFirebaseAuth : public QtFirebaseService
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
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }
    ~QtFirebaseAuth() override;

    enum Error
    {
        ErrorNone = firebase::auth::kAuthErrorNone,
        ErrorUnimplemented = firebase::auth::kAuthErrorUnimplemented,
        ErrorFailure = firebase::auth::kAuthErrorFailure
    };
    Q_ENUM(Error)

    enum Action
    {
        ActionRegister,
        ActionSignIn,
        ActionSignOut,
        ActionDeleteUser
    };
    Q_ENUM(Action)



public slots:
    //Control
    void registerUser(const QString& email, const QString& pass);
    void signIn(const QString& email, const QString& pass);
    void signOut();
    void sendPasswordResetEmail(const QString& email);
    void deleteUser();

    //Status
    bool signedIn() const;
    bool running() const;
    int errorId() const;
    QString errorMsg() const;

    //Data
    QString email() const;
    QString displayName() const;
    bool emailVerified() const;
    QString photoUrl() const;
    QString uid() const;

signals:
    void signedInChanged();
    void runningChanged();
    void completed(bool success, int actionId);
    void passwordResetEmailSent();

protected:
    explicit QtFirebaseAuth(QObject *parent = nullptr);

private:
    void clearError();
    void setComplete(bool complete);
    void setSignIn(bool value);
    void setError(int errId, const QString& errMsg = QString());
    void init() override;
    void onFutureEvent(QString eventId, firebase::FutureBase future) override;

    static QtFirebaseAuth *self;
    firebase::auth::Auth* m_auth;

    bool m_complete;
    bool m_signedIn;
    int m_errId;
    QString m_errMsg;
    int m_action;

    Q_DISABLE_COPY(QtFirebaseAuth)
};

#endif //QTFIREBASE_BUILD_AUTH

#endif // QTFIREBASE_AUTH_H

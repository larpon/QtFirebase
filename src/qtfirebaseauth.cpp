#include "qtfirebaseauth.h"

namespace auth = ::firebase::auth;

QtFirebaseAuth *QtFirebaseAuth::self = 0;

QtFirebaseAuth::QtFirebaseAuth(QObject *parent) : QtFirebaseService(parent),
    m_auth(nullptr)
    ,m_complete(false)
    ,m_signedIn(false)
    ,m_errId(kAuthErrorNone)
    ,m_action(ActSignIn)
{
    if(self == 0)
    {
        self = this;
        qDebug() << self << "::QtFirebaseAuth" << "singleton";
    }
    startInit();
}

void QtFirebaseAuth::timerEvent(QTimerEvent *e)
{

}

void QtFirebaseAuth::clearError()
{
    setError(kAuthErrorNone);
}

void QtFirebaseAuth::setError(int errId, const QString &errMsg)
{
    m_errId = errId;
    m_errMsg = errMsg;
}

void QtFirebaseAuth::registerUser(const QString &email, const QString &pass)
{
    if(isRunning())
        return;

    clearError();
    setComplete(false);
    m_action = ActRegister;
    firebase::Future<auth::User*> future =
           m_auth->CreateUserWithEmailAndPassword(email.toUtf8().constData(), pass.toUtf8().constData());
    qFirebase->addFuture(__QTFIREBASE_ID + ".auth.register", future);
}

bool QtFirebaseAuth::isSignedIn() const
{
    return m_signedIn;
}

void QtFirebaseAuth::signIn(const QString &email, const QString &pass)
{
    if(isRunning())
        return;

    m_action = ActSignIn;
    clearError();
    setComplete(false);

    if(isSignedIn())
    {
        signOut();
    }
    firebase::Future<auth::User*> future =
                  m_auth->SignInWithEmailAndPassword(email.toUtf8().constData(), pass.toUtf8().constData());

    qFirebase->addFuture(__QTFIREBASE_ID + ".auth.signin", future);
}

bool QtFirebaseAuth::isRunning() const
{
    return !m_complete;
}

void QtFirebaseAuth::signOut()
{
    m_action = ActSignOut;
    m_auth->SignOut();
    clearError();
    setComplete(false);
    setSignIn(false);
    setComplete(true);
}

int QtFirebaseAuth::errorId() const
{
    return m_errId;
}

QString QtFirebaseAuth::errorMsg() const
{
    return m_errMsg;
}

QString QtFirebaseAuth::getEmail() const
{
    if(isSignedIn())
    {
        return m_auth->current_user()->email().c_str();
    }
    return QString();
}

QString QtFirebaseAuth::getDisplayName() const
{
    if(isSignedIn())
    {
        return m_auth->current_user()->display_name().c_str();
    }
    return QString();
}

bool QtFirebaseAuth::emailVerified() const
{
    if(isSignedIn())
    {
        return m_auth->current_user()->is_email_verified();
    }
    return false;
}

QString QtFirebaseAuth::getPhotoUrl() const
{
    if(isSignedIn())
    {
        return m_auth->current_user()->photo_url().c_str();
    }
    return QString();
}

QString QtFirebaseAuth::getUid() const
{
    if(isSignedIn())
    {
        return m_auth->current_user()->uid().c_str();
    }
    return QString();
}

void QtFirebaseAuth::setComplete(bool complete)
{
    if(m_complete!=complete)
    {
        m_complete = complete;
        emit runningChanged();
        if(m_complete)
            emit completed(m_errId==kAuthErrorNone, m_action);
    }
}

void QtFirebaseAuth::setSignIn(bool value)
{
    if(m_signedIn!=value)
    {
        m_signedIn = value;
        emit signedInChanged();
    }
}

void QtFirebaseAuth::init()
{
    if(!qFirebase->ready()) {
        qDebug() << self << "::init" << "base not ready";
        return;
    }

    if(!ready() && !initializing()) {
        setInitializing(true);
        m_auth = auth::Auth::GetAuth(qFirebase->firebaseApp());
        qDebug() << self << "::init" << "native initialized";
        setInitializing(false);
        setReady(true);

        auth::User* user = m_auth->current_user();
        if(user!=nullptr)
            setSignIn(true);
        else
            setSignIn(false);
        setComplete(true);
    }
}

void QtFirebaseAuth::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID+".auth"))
        return;

    qDebug()<<self<<"::onFutureEvent"<<eventId;

    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent register user failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        setError(kAuthErrorFailure, "Unknown error");
    }
    else if(future.error()==auth::kAuthErrorNone)
    {
        clearError();
        if(eventId == __QTFIREBASE_ID+".auth.register")
        {
            if(future.result_void() == nullptr)
            {
                setError(kAuthErrorFailure, "Registered user is null");
                qDebug()<<"Registered user is null";
            }
            else
            {
                auth::User* user = result<auth::User*>(future.result_void())
                                                 ? *(result<auth::User*>(future.result_void()))
                                                 : nullptr;
                if(user!=nullptr)
                {
                    qFirebase->addFuture(__QTFIREBASE_ID + ".auth.sendemailverify", user->SendEmailVerification());
                }
            }
        }
        else if(eventId == __QTFIREBASE_ID+".auth.sendemailverify")
        {
            qDebug()<<"Verification email sent successfully";
        }
        else if(eventId == __QTFIREBASE_ID+".auth.signin")
        {

            qDebug()<<"Sign in successful";
            auth::User* user = result<auth::User*>(future.result_void())
                                             ? *(result<auth::User*>(future.result_void()))
                                             : nullptr;
            if(user!=nullptr)
            {
                setSignIn(true);
                /*qDebug()<<"Email:"<<user->email().c_str();
                qDebug()<<"Display name:"<<user->display_name().c_str();
                qDebug()<<"Photo url:"<<user->photo_url().c_str();
                qDebug()<<"provider_id:"<<user->provider_id().c_str();
                qDebug()<<"is_anonymous:"<<user->is_anonymous();
                qDebug()<<"is_email_verified:"<<user->is_email_verified();*/
            }
        }
    }
    else
    {
        if(eventId == __QTFIREBASE_ID+".auth.register")
        {
            qDebug()<<"Registering user completed with error:"<<future.error()<<future.error_message();
        }
        else if(eventId == __QTFIREBASE_ID+".auth.sendemailverify")
        {
            qDebug()<<"Verification email send error:"<<future.error()<<future.error_message();
        }
        else if(eventId == __QTFIREBASE_ID+".auth.signin")
        {
            setSignIn(false);
            qDebug()<<"Sign in error:"<<future.error()<<future.error_message();
        }
        setError(future.error(), future.error_message());
    }
    setComplete(true);
}

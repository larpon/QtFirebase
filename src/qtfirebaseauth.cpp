#include "qtfirebaseauth.h"

namespace auth = ::firebase::auth;

QtFirebaseAuth *QtFirebaseAuth::self = 0;

QtFirebaseAuth::QtFirebaseAuth(QObject *parent) : QtFirebaseService(parent)
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

void QtFirebaseAuth::registerUser(const QString &email, const QString &pass)
{
    //TODO check no current register future executing
    firebase::Future<auth::User*> future =
           m_auth->CreateUserWithEmailAndPassword(email.toUtf8().constData(), pass.toUtf8().constData());
    qFirebase->addFuture(__QTFIREBASE_ID + ".auth.register", future);
}

bool QtFirebaseAuth::isSignedIn()
{
    auth::User* user = m_auth->current_user();
    if(user!=nullptr)
        return true;

    return false;
}

void QtFirebaseAuth::signIn(const QString &email, const QString &pass)
{
    //TODO check user not signed in
    //TODO check no current sign future executing
    firebase::Future<auth::User*> future =
                  m_auth->SignInWithEmailAndPassword(email.toUtf8().constData(), pass.toUtf8().constData());

    qFirebase->addFuture(__QTFIREBASE_ID + ".auth.signin", future);
}

void QtFirebaseAuth::signOut()
{
    m_auth->SignOut();
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
        return;
    }
    else if(future.error()==auth::kAuthErrorNone)
    {
        if(eventId == __QTFIREBASE_ID+".auth.register")
        {
            qDebug()<<"Register future complete";
            if(future.result_void() == nullptr)
            {
                qDebug()<<"Registered user is null";
            }
            else
            {
                //const auth::User* const* ppUser = static_cast<const auth::User* const*>(future.result_void());
                /*const auth::User* const* ppUser = static_cast<const auth::User* const*>(future.result_void());
                if(ppUser!=nullptr)
                {
                    auth::User const * user = *ppUser;
                    if(user!=nullptr)
                    {
                        qDebug()<<"User ok, email: "<<user->email().c_str();
                        qFirebase->addFuture(__QTFIREBASE_ID + ".auth.sendemailverify", user->SendEmailVerification());
                    }
                }*/
                auth::User* user = result<auth::User*>(future.result_void())
                                                 ? *(result<auth::User*>(future.result_void()))
                                                 : nullptr;
                if(user!=nullptr)
                {
                    qDebug()<<"User ok, email: "<<user->email().c_str();
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
                qDebug()<<"Email:"<<user->email().c_str();
                qDebug()<<"Display name:"<<user->display_name().c_str();
                qDebug()<<"Photo url:"<<user->photo_url().c_str();
                qDebug()<<"provider_id:"<<user->provider_id().c_str();
                qDebug()<<"is_anonymous:"<<user->is_anonymous();
                qDebug()<<"is_email_verified:"<<user->is_email_verified();
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
            qDebug()<<"Sign in error:"<<future.error()<<future.error_message();
        }
    }
}

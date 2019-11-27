#include "qtfirebaseadmob.h"

//#include <QGuiApplication>
#include <qqmlfile.h>
#include <QString>

namespace admob = ::firebase::admob;

/*
 * AdMob
 * (Base AdMob Object)
 */

QtFirebaseAdMob *QtFirebaseAdMob::self = nullptr;

QtFirebaseAdMob::QtFirebaseAdMob(QObject* parent) : QObject(parent)
{
    if(self == nullptr) {
        self = this;
        qDebug() << self << "::QtFirebaseAdMob" << "singleton";
    }

    _ready = false;
    _initializing = false;

    __testDevices = nullptr;

    if(!qFirebase->ready()) {
        connect(qFirebase,&QtFirebase::readyChanged, this, &QtFirebaseAdMob::init);
        qFirebase->requestInit();
    }
}

QtFirebaseAdMob::~QtFirebaseAdMob()
{
    if(_ready) {
        qDebug() << this << "::~QtFirebaseAdMob" << "shutting down";
        setReady(false);
        emit shutdown();
        //admob::Terminate(); // TODO causes crash see https://github.com/firebase/quickstart-cpp/issues/19
        self = nullptr;
    }
}

bool QtFirebaseAdMob::checkInstance(const char *function) const
{
    bool b = (QtFirebaseAdMob::self != nullptr);
    if(!b)
        qWarning("QtFirebaseAdMob::%s: Please instantiate the QtFirebaseAdMob object first", function);
    return b;
}

bool QtFirebaseAdMob::ready() const
{
    return _ready;
}

void QtFirebaseAdMob::setReady(bool ready)
{
    if(_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

QString QtFirebaseAdMob::appId() const
{
    return _appId;
}

void QtFirebaseAdMob::setAppId(const QString &adMobAppId)
{
    if(_appId != adMobAppId) {
        _appId = adMobAppId;

        if(!ready())
            init();

        emit appIdChanged();
    }
}

QVariantList QtFirebaseAdMob::testDevices() const
{
    return qFirebaseAdMobTestDevices->testDevices();
}

void QtFirebaseAdMob::setTestDevices(const QVariantList &testDevices)
{
    qFirebaseAdMobTestDevices->setTestDevices(testDevices);
}

void QtFirebaseAdMob::init()
{
    if(!qFirebase->ready()) {
        qDebug() << this << "::init" << "base not ready";
        return;
    }

    if(!_ready && !_initializing) {

        // Notify the user of the pre-requisity
        if(_appId.isEmpty()) {
            qWarning() << this << "::init" << "failed to initialize Firebase AdMob." << "No AdMob app ID set";
            return;
        }

        _initializing = true;

        __appIdByteArray = _appId.toLatin1();
        __appId = __appIdByteArray.data();

        // Initialize the Firebase AdMob library with AdMob app ID.
        qDebug() << this << "::init" << "initializing with App ID" << __appId;

        firebase::App* app = qFirebase->firebaseApp();
        firebase::InitResult ir = admob::Initialize(*app, __appId);

        qDebug() << this << "::init" << "initialized";

        if(ir != firebase::kInitResultSuccess) {
            qWarning() << this << "::init" << "failed to initialize Firebase AdMob";
            _initializing = false;
            return;
        }

        qDebug() << this << "::init" << "initialized";
        _initializing = false;
        setReady(true);
    }
}



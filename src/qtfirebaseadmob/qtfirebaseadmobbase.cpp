#include "qtfirebaseadmobbase.h"

QtFirebaseAdMobBase::QtFirebaseAdMobBase(QObject *parent)
    : QObject(parent)
    , __QTFIREBASE_ID(QString().asprintf("%8p", static_cast<void*>(this)))
    , _ready(false)
    , _initializing(false)
    , _loaded(false)
    , _isFirstInit(true)
    , _adUnitId()
    , __adUnitIdByteArray()
    , _visible(false)
    , _request(nullptr)
    , _nativeUIElement(nullptr)
    , _initTimer(new QTimer(this))

{
    //connect(this,&QtFirebaseAdMobBase::visibleChanged, this, &QtFirebaseAdMobBase::onVisibleChanged);
    connect(_initTimer, &QTimer::timeout, this, &QtFirebaseAdMobBase::init);
    _initTimer->start(500);
}

QtFirebaseAdMobBase::~QtFirebaseAdMobBase()
{
    if(_ready) {
        qDebug() << this << "::~QtFirebaseAdMobBase" << "shutting down";
        setReady(false);
    }
}

void QtFirebaseAdMobBase::onInitialized()
{

}

bool QtFirebaseAdMobBase::ready() const {
    return _ready;
}

void QtFirebaseAdMobBase::setReady(bool ready) {
    if(_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

bool QtFirebaseAdMobBase::loaded() const {
    return _loaded;
}

void QtFirebaseAdMobBase::setLoaded(bool loaded) {
    if(_loaded != loaded) {
        _loaded = loaded;
        emit loadedChanged();
    }
}

QString QtFirebaseAdMobBase::adUnitId() const {
    return _adUnitId;
}

void QtFirebaseAdMobBase::setAdUnitId(const QString &adUnitId) {
    if(_adUnitId != adUnitId) {
        _adUnitId = adUnitId;
        __adUnitIdByteArray = _adUnitId.toLatin1();
        emit adUnitIdChanged();
    }
}

bool QtFirebaseAdMobBase::visible() const {
    return _visible;
}

QtFirebaseAdMobRequest *QtFirebaseAdMobBase::request() const {
    return _request;
}

void QtFirebaseAdMobBase::setRequest(QtFirebaseAdMobRequest *request) {
    if(_request != request) {
        _request = request;
        emit requestChanged();
    }
}

void QtFirebaseAdMobBase::init()
{
    if(!qFirebase->ready()) {
        qDebug() << this << "::init" << "base not ready";
        return;
    }

    if(!qFirebaseAdMob->ready()) {
        qDebug() << this << "::init" << "AdMob base not ready";
        return;
    }

    if(_adUnitId.isEmpty()) {
        qDebug() << this << "::init" << "adUnitId must be set in order to initialize";
        return;
    }

    if(_isFirstInit && !PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "no native ui element. Waiting for it...";
        return;
    }

    // TODO test if this actually nessecary anymore
    if(!_nativeUIElement && !PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "no native ui element";
        return;
    }

    if(!_nativeUIElement && PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "setting native ui element";
        _nativeUIElement = PlatformUtils::getNativeWindow();
    }   

    if(!_ready && !_initializing) {
        _initializing = true;

        auto future = initInternal();

        if(future.status() != firebase::kFutureStatusInvalid) // if the status is invalid then it is already handled elsewhere
        {
            future.OnCompletion([this](const firebase::FutureBase& completed_future)
            {
                // We are probably in a different thread right now.
                #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
                QMetaObject::invokeMethod(this, [this, completed_future]() {
                #endif
                    if(completed_future.error() != firebase::admob::kAdMobErrorNone) {
                        qDebug() << this << "::init" << "initializing failed." << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                        emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
                        _initializing = false;
                    }
                    else {
                        qDebug() << this << "::init" << "initialized";
                        _initializing = false;
                        _isFirstInit = false;
                        onInitialized();
                        setReady(true);
                    }
                #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
                });
                #endif
            });
        }
    }
}

void QtFirebaseAdMobBase::load()
{
    if(!_ready) {
        qDebug() << this << "::load" << "not ready";
        return;
    }

    if(_request == nullptr) {
        qDebug() << this << "::load() no request data sat. Not loading";
        return;
    }

    qDebug() << this << "::load() getting request data";
    setLoaded(false);
    emit loading();
    firebase::FutureBase future = loadInternal();

    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != firebase::admob::kAdMobErrorNone) {
                qWarning() << this << "::load" << "load failed" << "ERROR" << "code:" << completed_future.error() << "message:" << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
            }
            else {
                qDebug() << this << "::load loaded";
                setLoaded(true);
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });
}


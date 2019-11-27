#include "qtfirebaseadmobinterstitial.h"

QtFirebaseAdMobInterstitial::QtFirebaseAdMobInterstitial(QObject* parent)
    : QtFirebaseAdMobBase(parent)
    , _interstitial(nullptr)
    , _interstitialAdListener(nullptr)
{
    connect(this, &QtFirebaseAdMobInterstitial::presentationStateChanged, this, &QtFirebaseAdMobInterstitial::onPresentationStateChanged);
}

QtFirebaseAdMobInterstitial::~QtFirebaseAdMobInterstitial()
{
    if(_ready) {
        _interstitial->SetListener(nullptr);
        delete _interstitialAdListener;

        qDebug() << this << "::~QtFirebaseAdMobInterstitial" << "Destroyed Interstitial";        
        delete _interstitial;
    }

    _initTimer->stop();
    delete _initTimer;
}

void QtFirebaseAdMobInterstitial::setVisible(bool visible)
{
    if(!_ready) {
        qDebug() << this << "::setVisible native part not ready";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::setVisible native part not loaded - so not changing visiblity to" << visible;
        return;
    }

    if(!_visible && visible) {
        show(); // NOTE show will change _visible and emit signal
    } else {
        // NOTE An interstitial can't be hidden by any other than the user
        qInfo() << this << "::setVisible" << visible << " - interstitials can't be hidden programmatically. Not hidding";
    }
}

firebase::FutureBase QtFirebaseAdMobInterstitial::initInternal()
{
    _interstitial = new firebase::admob::InterstitialAd();

    //__adUnitIdByteArray = _adUnitId.toLatin1();
    //__adUnitId = __adUnitIdByteArray.data();

    // A reference to an iOS UIView or an Android Activity.
    // This is the parent UIView or Activity of the Interstitial view.
    qDebug() << this << "::init initializing with AdUnitID" << __adUnitIdByteArray.constData();
    auto future = _interstitial->Initialize(static_cast<firebase::admob::AdParent>(_nativeUIElement), __adUnitIdByteArray.constData());
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
            } else {
                _initTimer->stop();
                // NOTE don't delete it as we need it on iOS to re-initalize the whole mill after every load and show
                //delete _initTimer;

                qDebug() << this << "::init initialized";
                _initializing = false;
                _isFirstInit = false;

                // Add listener (can't be done before it's initialized)
                auto cpy = _interstitialAdListener;
                _interstitialAdListener = new QtFirebaseAdMobInterstitialAdListener(this);
                _interstitial->SetListener(_interstitialAdListener);
                if(cpy) {
                    delete cpy;
                }

                setReady(true);
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });

    return firebase::FutureBase(); // invalid future because we already handled it
}

void QtFirebaseAdMobInterstitial::onPresentationStateChanged(int state)
{
    if(state == PresentationStateCoveringUI) {
        if(!_visible) {
            _visible = true;
            emit visibleChanged();
        }
    }

    if(state == PresentationStateHidden) {
        if(_visible) {
            _visible = false;
            emit visibleChanged();
        }

        setLoaded(false);
        qDebug() << this << "::onPresentationStateChanged() loaded false";

        // NOTE iOS necessities
#if defined(Q_OS_IOS)

        setReady(false);
        qDebug() << this << "::onPresentationStateChanged() ready false";

        // Will be newed when init() is called
        //delete _interstitial; // NOTE Crashes the app when used

        // NOTE Auto re-initializing because of this: https://firebase.google.com/docs/admob/ios/interstitial#only_show_gadinterstitial_once
        qDebug() << this << "::onPresentationStateChanged() re-initializing one-time use GADInterstitial";
        _initTimer->start(500);

#endif

        emit closed();
    }
}

firebase::FutureBase QtFirebaseAdMobInterstitial::loadInternal()
{
    firebase::admob::AdRequest request = _request->asAdMobRequest();
    return _interstitial->LoadAd(request);
}

void QtFirebaseAdMobInterstitial::show()
{
    if(!_ready) {
        qDebug() << this << "::show" << "not ready - so not showing";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::show" << "not loaded - so not showing";
        return;
    }

    firebase::FutureBase future = _interstitial->Show();
    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != firebase::admob::kAdMobErrorNone) {
                qDebug() << this << "::show" << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });
}

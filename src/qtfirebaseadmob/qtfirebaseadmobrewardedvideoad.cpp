#include "qtfirebaseadmobrewardedvideoad.h"

QtFirebaseAdMobRewardedVideoAd::QtFirebaseAdMobRewardedVideoAd(QObject* parent)
    : QtFirebaseAdMobBase(parent)
{
    connect(this, &QtFirebaseAdMobRewardedVideoAd::presentationStateChanged, this, &QtFirebaseAdMobRewardedVideoAd::onPresentationStateChanged);
}

QtFirebaseAdMobRewardedVideoAd::~QtFirebaseAdMobRewardedVideoAd()
{
    if(_ready) {
        firebase::admob::rewarded_video::Destroy();
        qDebug() << this << "::~QtFirebaseAdMobRewardedVideoAd" << "Destroyed";
    }
    _initTimer->stop();
}

void QtFirebaseAdMobRewardedVideoAd::setVisible(bool visible)
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

firebase::FutureBase QtFirebaseAdMobRewardedVideoAd::initInternal()
{
    auto future = firebase::admob::rewarded_video::Initialize();

    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != firebase::admob::kAdMobErrorNone)
            {
                qDebug() << this << "::init initializing failed." << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
                _initializing = false;
            }
            else
            {
                _initTimer->stop();
                qDebug() << this << "::init initialized";
                _initializing = false;
                _isFirstInit = false;
                firebase::admob::rewarded_video::SetListener(this);
                setReady(true);
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });

    return firebase::FutureBase();
}

void QtFirebaseAdMobRewardedVideoAd::onPresentationStateChanged(int state)
{
    switch (state) {
    case QtFirebaseAdMobRewardedVideoAd::PresentationStateCoveringUI: {
        if(!_visible) {
            _visible = true;
            emit visibleChanged();
        }
    }
        break;
    case QtFirebaseAdMobRewardedVideoAd::PresentationStateHidden: {
        if(_visible) {
            _visible = false;
            emit visibleChanged();
        }

        setLoaded(false);
        qDebug() << this << "::onPresentationStateChanged() loaded false";

        // NOTE iOS necessities
        /*#if defined(Q_OS_IOS)

        setReady(false);
        qDebug() << this << "::onPresentationStateChanged() ready false";

        // Will be newed when init() is called
        //delete _interstitial; // NOTE Crashes the app when used

        // NOTE Auto re-initializing because of this: https://firebase.google.com/docs/admob/ios/interstitial#only_show_gadinterstitial_once
        qDebug() << this << "::onPresentationStateChanged() re-initializing one-time use GADInterstitial";
        _initTimer->start(500);

        #endif*/

        emit closed();
    }
        break;
    }
}

firebase::FutureBase QtFirebaseAdMobRewardedVideoAd::loadInternal()
{
    firebase::admob::AdRequest request = _request->asAdMobRequest();
    return firebase::admob::rewarded_video::LoadAd( __adUnitIdByteArray.constData(), request);
}

void QtFirebaseAdMobRewardedVideoAd::show()
{
    if(!_ready) {
        qDebug() << this << "::show() not ready - so not showing";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::show() not loaded - so not showing";
        return;
    }

    if(!_nativeUIElement && !PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "no native ui element";
        return;
    }

    if(!_nativeUIElement && PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "setting native ui element";
        _nativeUIElement = PlatformUtils::getNativeWindow();
    }

    firebase::FutureBase future = firebase::admob::rewarded_video::Show(static_cast<firebase::admob::AdParent>(_nativeUIElement));

    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != firebase::admob::kAdMobErrorNone)
            {
                qDebug() << this << "::show " << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });
}

void QtFirebaseAdMobRewardedVideoAd::OnRewarded(firebase::admob::rewarded_video::RewardItem reward)
{
    QString type = QString::fromStdString(reward.reward_type);
    qDebug() << this << QString(QStringLiteral("Rewarding user of %1 with amount %2")).arg(type).arg(QString::number(static_cast<double>(reward.amount)));

    emit rewarded(type, reward.amount);
}

void QtFirebaseAdMobRewardedVideoAd::OnPresentationStateChanged(firebase::admob::rewarded_video::PresentationState state)
{
    if(state == firebase::admob::rewarded_video::kPresentationStateHidden)
    {
        qDebug() << this << "kPresentationStateHidden";
    }
    else if(state == firebase::admob::rewarded_video::kPresentationStateCoveringUI)
    {
        qDebug() << this << "kPresentationStateCoveringUI";
    }
    else if(state == firebase::admob::rewarded_video::kPresentationStateVideoHasStarted)
    {
        qDebug() << this << "kPresentationStateVideoHasStarted";
    }
    else if(state == firebase::admob::rewarded_video::kPresentationStateVideoHasCompleted)
    {
        qDebug() << this << "kPresentationStateVideoHasCompleted";
    }

    int pState = QtFirebaseAdMobRewardedVideoAd::PresentationStateHidden;

    if(state == firebase::admob::rewarded_video::kPresentationStateHidden) {
        pState = QtFirebaseAdMobRewardedVideoAd::PresentationStateHidden;
    } else if(state == firebase::admob::rewarded_video::kPresentationStateCoveringUI) {
        pState = QtFirebaseAdMobRewardedVideoAd::PresentationStateCoveringUI;
    } else if(state == firebase::admob::rewarded_video::kPresentationStateVideoHasStarted) {
        pState = QtFirebaseAdMobRewardedVideoAd::PresentationStateVideoHasStarted;
    } else if(state == firebase::admob::rewarded_video::kPresentationStateVideoHasCompleted) {
        pState = QtFirebaseAdMobRewardedVideoAd::PresentationStateVideoHasCompleted;
    }
    emit presentationStateChanged(pState);
}


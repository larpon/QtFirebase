#ifndef QTFIREBASE_ADMOB_H
#define QTFIREBASE_ADMOB_H

#ifdef QTFIREBASE_BUILD_ADMOB

#include "src/qtfirebase.h"

#if defined(qFirebaseAdMob)
#undef qFirebaseAdMob
#endif
#define qFirebaseAdMob (static_cast<QtFirebaseAdMob *>(QtFirebaseAdMob::instance()))

#include "qtfirebaseadmobtestdevices.h"
#include "qtfirebaseadmobrequest.h"

#include "firebase/admob.h"

#include <QDebug>
#include <QObject>

/*
 * AdMob (Base singleton object)
 */
class QtFirebaseAdMob : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)

    Q_PROPERTY(QVariantList testDevices READ testDevices WRITE setTestDevices NOTIFY testDevicesChanged)

public:
    enum Error
    {
        ErrorUnknown = firebase::admob::kAdMobErrorNone-1,
        ErrorNone = firebase::admob::kAdMobErrorNone,
        ErrorAlreadyInitialized = firebase::admob::kAdMobErrorAlreadyInitialized,
        ErrorInternalError = firebase::admob::kAdMobErrorInternalError,
        ErrorInvalidRequest = firebase::admob::kAdMobErrorInvalidRequest,
        ErrorLoadInProgress = firebase::admob::kAdMobErrorLoadInProgress,
        ErrorNetworkError = firebase::admob::kAdMobErrorNetworkError,
        ErrorNoFill = firebase::admob::kAdMobErrorNoFill,
        ErrorNoWindowToken = firebase::admob::kAdMobErrorNoWindowToken,
        ErrorUninitialized = firebase::admob::kAdMobErrorUninitialized
    };
    Q_ENUM(Error)

    explicit QtFirebaseAdMob(QObject* parent = nullptr);
    ~QtFirebaseAdMob();

    static QtFirebaseAdMob *instance() {
        if(self == nullptr) {
            self = new QtFirebaseAdMob(nullptr);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }
    bool checkInstance(const char *function) const;

    bool ready() const;
    void setReady(bool ready);

    QString appId() const;
    void setAppId(const QString &appId);

    QVariantList testDevices() const;
    void setTestDevices(const QVariantList &testDevices);

signals:
    void shutdown();
    void readyChanged();
    void appIdChanged();
    void testDevicesChanged();

private slots:
    void init();

private:
    static QtFirebaseAdMob *self;
    Q_DISABLE_COPY(QtFirebaseAdMob)

    bool _ready;
    bool _initializing;

    QString _appId;
    QByteArray __appIdByteArray;
    const char *__appId;

    QVariantList _testDevices;
    QByteArrayList __testDevicesByteArrayList;
    const char** __testDevices;

};

/*
 * AdMobRewardedVideoAd
 *
 * Similar to interstitial but also contains time on the Ad,
 * when time is out user can close the banner and get reward.
 * Reward is a value of float type (can be coins/starts/points in your game)
 * To setup create rewarded Ad on your adMob account page and put identifier to setAdUnitId.
 * The value of reward should be set in the account page as well.
 * Limitation: reward type does not supported now.
 *  Example QML code
 *
 * AdMobRewardedVideoAd {
 *     adUnitId: "your banner id"
 *
 *     onReadyChanged: if(ready) load()
 *
 *     onClosed: load()
 *
 *     request: AdMobRequest {
 *              }
 *
 *     onRewarded: {
 *         console.log("Rewarded with reward:" + type + ", value:" + value);
 *     }
 *
 *     onError: {
 *         console.log("Rewarded failed with error code",code,"and message",message)
 *     }
 *  }
 */
/*
class QtFirebaseAdMobRewardedVideoAd : public QtFirebaseAdMobBase, public firebase::admob::rewarded_video::Listener
{
    Q_OBJECT

public:
    enum PresentationState
    {
        PresentationStateHidden = firebase::admob::rewarded_video::kPresentationStateHidden,
        PresentationStateCoveringUI = firebase::admob::rewarded_video::kPresentationStateCoveringUI,
        PresentationStateVideoHasStarted = firebase::admob::rewarded_video::kPresentationStateVideoHasStarted,
        PresentationStateVideoHasCompleted = firebase::admob::rewarded_video::kPresentationStateVideoHasCompleted
    };
    Q_ENUM(PresentationState)

    QtFirebaseAdMobRewardedVideoAd(QObject* parent = nullptr);
    ~QtFirebaseAdMobRewardedVideoAd() override;

    void setVisible(bool visible) override;

signals:
    void closed();
    void presentationStateChanged(int state);
    void rewarded(const QString &type, float value);

public slots:
    void show() override;

private slots:
    void onPresentationStateChanged(int state);

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

    void OnRewarded(firebase::admob::rewarded_video::RewardItem reward) override;
    void OnPresentationStateChanged(firebase::admob::rewarded_video::PresentationState state) override;

};

*/

#endif // QTFIREBASE_BUILD_ADMOB
#endif // QTFIREBASE_ADMOB_H

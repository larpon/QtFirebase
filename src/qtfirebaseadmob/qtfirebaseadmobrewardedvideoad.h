#ifndef QTFIREBASE_ADMOB_REWARDED_VIDEO_AD_H
#define QTFIREBASE_ADMOB_REWARDED_VIDEO_AD_H


#include "src/qtfirebase.h"

#include "qtfirebaseadmobbase.h"
//#include "qtfirebaseadmobrequest.h"
#include "qtfirebaseadmobtestdevices.h"
#include "qtfirebaseadmobrequest.h"

#include "firebase/admob/rewarded_video.h"

#include <QDebug>
#include <QObject>

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

#endif // QTFIREBASE_ADMOB_REWARDED_VIDEO_AD_H

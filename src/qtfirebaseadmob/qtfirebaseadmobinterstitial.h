#ifndef QTFIREBASE_ADMOB_INTERSTITIAL_H
#define QTFIREBASE_ADMOB_INTERSTITIAL_H

#include "src/qtfirebase.h"

#include "qtfirebaseadmob.h"
#include "qtfirebaseadmobbase.h"
#include "qtfirebaseadmobrequest.h"
#include "qtfirebaseadmobtestdevices.h"

#include "firebase/admob/interstitial_ad.h"

#include <QDebug>
#include <QObject>

/*
 * AdMobInterstitial
 */

class QtFirebaseAdMobInterstitialAdListener;

class QtFirebaseAdMobInterstitial : public QtFirebaseAdMobBase
{
    Q_OBJECT

public:
    enum PresentationState
    {
        PresentationStateHidden = firebase::admob::InterstitialAd::kPresentationStateHidden,
        PresentationStateCoveringUI  = firebase::admob::InterstitialAd::kPresentationStateCoveringUI
    };
    Q_ENUM(PresentationState)

    QtFirebaseAdMobInterstitial(QObject* parent = nullptr);
    ~QtFirebaseAdMobInterstitial() override;

    void setVisible(bool visible) override;

signals:
    void closed();
    void presentationStateChanged(int state);

public slots:
    void show() override;

private slots:
    void onPresentationStateChanged(int state);

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

private:
    firebase::admob::InterstitialAd* _interstitial;
    QtFirebaseAdMobInterstitialAdListener* _interstitialAdListener;

};

// A listener class to an InterstitialAd.
class QtFirebaseAdMobInterstitialAdListener : public firebase::admob::InterstitialAd::Listener {

    friend class QtFirebaseAdMobInterstitial;

public:
    QtFirebaseAdMobInterstitialAdListener(QtFirebaseAdMobInterstitial* qtFirebaseAdMobInterstitial) {
        _qtFirebaseAdMobInterstitial = qtFirebaseAdMobInterstitial;
    }

    void OnPresentationStateChanged(firebase::admob::InterstitialAd* interstitial_ad,
        firebase::admob::InterstitialAd::PresentationState state) override {
        Q_UNUSED(interstitial_ad)
        qDebug() << _qtFirebaseAdMobInterstitial << "::OnPresentationStateChanged";
        qDebug("InterstitialAd PresentationState has changed to %d.", state);

        int pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;

        if(state == firebase::admob::InterstitialAd::PresentationState::kPresentationStateHidden) {
            pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;
        } else if(state == firebase::admob::InterstitialAd::PresentationState::kPresentationStateCoveringUI) {
             pState = QtFirebaseAdMobInterstitial::PresentationStateCoveringUI;
        }
        emit _qtFirebaseAdMobInterstitial->presentationStateChanged(pState);
    }

private:
    QtFirebaseAdMobInterstitial* _qtFirebaseAdMobInterstitial;
};

#endif // QTFIREBASE_ADMOB_INTERSTITIAL_H

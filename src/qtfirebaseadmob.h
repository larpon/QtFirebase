#ifndef QTFIREBASE_ADMOB_H
#define QTFIREBASE_ADMOB_H

#ifdef QTFIREBASE_BUILD_ADMOB

#include "src/qtfirebase.h"

#if defined(qFirebaseAdMob)
#undef qFirebaseAdMob
#endif
#define qFirebaseAdMob (static_cast<QtFirebaseAdMob *>(QtFirebaseAdMob::instance()))

#include "firebase/admob.h"
#include "firebase/admob/types.h"
#include "firebase/admob/banner_view.h"
#include "firebase/admob/native_express_ad_view.h"
#include "firebase/admob/interstitial_ad.h"
#include "firebase/admob/rewarded_video.h"

#include <QDebug>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QDateTime>

/*
 * AdMob (Base singleton object)
 */

class QtFirebaseAdMob : public QObject
{
    // friend classes so they can read the __testDevices
    friend class QtFirebaseAdMobRequest;
    friend class QtFirebaseAdMobBanner;
    friend class QtFirebaseAdMobInterstitial;

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

    enum Gender
    {
        GenderUnknown,
        GenderFemale,
        GenderMale
    };
    Q_ENUM(Gender)

    enum ChildDirectedTreatment
    {
        ChildDirectedTreatmentUnknown,
        ChildDirectedTreatmentTagged,
        ChildDirectedTreatmentNotTagged
    };
    Q_ENUM(ChildDirectedTreatment)

    explicit QtFirebaseAdMob(QObject* parent = 0);
    ~QtFirebaseAdMob();

    static QtFirebaseAdMob *instance() {
        if(self == 0) {
            self = new QtFirebaseAdMob(0);
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
 * AdMob Request
 */

class QtFirebaseAdMobRequest : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int gender READ gender WRITE setGender NOTIFY genderChanged)
    Q_PROPERTY(int childDirectedTreatment READ childDirectedTreatment WRITE setChildDirectedTreatment NOTIFY childDirectedTreatmentChanged)
    Q_PROPERTY(QDateTime birthday READ birthday WRITE setBirthday NOTIFY birthdayChanged)
    Q_PROPERTY(QVariantList keywords READ keywords WRITE setKeywords NOTIFY keywordsChanged)
    Q_PROPERTY(QVariantList extras READ extras WRITE setExtras NOTIFY extrasChanged)
    Q_PROPERTY(QVariantList testDevices READ testDevices WRITE setTestDevices NOTIFY testDevicesChanged)

public:
    QtFirebaseAdMobRequest(QObject* parent = 0);

    int gender() const;
    void setGender(int gender);

    int childDirectedTreatment() const;
    void setChildDirectedTreatment(int childDirectedTreatment);

    QDateTime birthday() const;
    void setBirthday(const QDateTime &birthday);

    QVariantList keywords() const;
    void setKeywords(const QVariantList &keywords);

    QVariantList extras() const;
    void setExtras(const QVariantList &extras);

    QVariantList testDevices() const;
    void setTestDevices(QVariantList &testDevices);

    firebase::admob::AdRequest asAdMobRequest();
signals:
    void genderChanged();
    void childDirectedTreatmentChanged();
    void birthdayChanged();
    void keywordsChanged();
    void extrasChanged();
    void testDevicesChanged();

private:

    int _gender;

    int _childDirectedTreatment;

    QDateTime _birthday;

    QVariantList _keywords;
    // Stores the actual char* data so it remains valid throughout a request cycle
    QByteArrayList __keywordsByteArrayList;
    const char** __keywords;

    QVariantList _extras;
    // Stores the actual char* data so it remains valid throughout a request cycle
    QList<QPair<QByteArray,QByteArray>> __extrasList;
    firebase::admob::KeyValuePair* __extras;

    QVariantList _testDevices;
    // Stores the actual char* data so it remains valid throughout a request cycle
    QByteArrayList __testDevicesByteArrayList;
    const char** __testDevices;

    firebase::admob::AdRequest _admobRequest;
};

/*
 * Base class for admob ads.
 */

class QtFirebaseAdMobBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

    Q_PROPERTY(QString adUnitId READ adUnitId WRITE setAdUnitId NOTIFY adUnitIdChanged)

    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

    Q_PROPERTY(QtFirebaseAdMobRequest* request READ request WRITE setRequest NOTIFY requestChanged)

protected:
    QtFirebaseAdMobBase(QObject* parent = 0);

    virtual firebase::FutureBase initInternal() = 0;
    virtual firebase::FutureBase loadInternal() = 0;

public:
    bool ready() const;
    virtual void setReady(bool ready);

    bool loaded() const;
    virtual void setLoaded(bool loaded);

    QString adUnitId() const;
    virtual void setAdUnitId(const QString &adUnitId);

    bool visible() const;
    virtual void setVisible(bool visible) = 0;

    QtFirebaseAdMobRequest *request() const;
    virtual void setRequest(QtFirebaseAdMobRequest *request);

signals:
    void readyChanged();
    void loadedChanged();
    void adUnitIdChanged();
    void requestChanged();
    void loading();
    void error(int code, QString message);
    void visibleChanged();

public slots:
    virtual void load();
    virtual void show() = 0;

protected slots:
    void init();

protected:
    QString __QTFIREBASE_ID;
    bool _ready;
    bool _initializing;
    bool _loaded;

    bool _isFirstInit;

    QString _adUnitId;
    QByteArray __adUnitIdByteArray;
    //const char *__adUnitId; // TODO use __adUnitIdByteArray.constData() instead ?

    bool _visible;

    QtFirebaseAdMobRequest* _request;

    void *_nativeUIElement;

    QTimer *_initTimer;

};


class QtFirebaseAdMobBannerBase : public QtFirebaseAdMobBase
{
    Q_OBJECT

    Q_PROPERTY(int x READ getX WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ getY WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ getHeight WRITE setHeight NOTIFY heightChanged)

public:
    enum Position
    {
        PositionTopCenter,
        PositionTopLeft,
        PositionTopRight,
        PositionBottomCenter,
        PositionBottomLeft,
        PositionBottomRight
    };
    Q_ENUM(Position)

    explicit QtFirebaseAdMobBannerBase(QObject *parent = 0);


    virtual int getX() const;
    virtual void setX(int x);

    virtual int getY() const;
    virtual void setY(int y);

    virtual int getWidth() const;
    virtual void setWidth(int width);

    virtual int getHeight() const;
    virtual void setHeight(int height);

    virtual void setVisible(bool visible) override;

public slots:
    virtual void show() override;
    virtual void hide();
    virtual void moveTo(int x, int y);
    virtual void moveTo(Position position);

protected slots:
    virtual void onApplicationStateChanged(Qt::ApplicationState state);

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();

protected:
    virtual firebase::FutureBase moveToInternal(int x, int y) = 0;
    virtual firebase::FutureBase moveToInternal(Position position) = 0;

    virtual firebase::FutureBase setXInternal(int x) = 0;
    virtual firebase::FutureBase setYInternal(int y) = 0;

    virtual firebase::FutureBase setVisibleInternal(bool visible) = 0;

protected:
    int _x;
    int _y;
    int _width;
    int _height;

};

/*
 * AdMobBanner
 */

class QtFirebaseAdMobBanner : public QtFirebaseAdMobBannerBase
{
    Q_OBJECT

public:
    QtFirebaseAdMobBanner(QObject* parent = 0);
    ~QtFirebaseAdMobBanner();

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

    firebase::FutureBase moveToInternal(int x, int y) override;
    firebase::FutureBase moveToInternal(Position position) override;

    firebase::FutureBase setXInternal(int x) override;
    firebase::FutureBase setYInternal(int y) override;

    firebase::FutureBase setVisibleInternal(bool visible) override;

private:
    firebase::admob::BannerView* _banner;
};

/*
 * AdMobNativeExpressAd
 */
class QtFirebaseAdMobNativeExpressAd : public QtFirebaseAdMobBannerBase
{
    Q_OBJECT

public:

    QtFirebaseAdMobNativeExpressAd(QObject* parent = 0);
    ~QtFirebaseAdMobNativeExpressAd();

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

    firebase::FutureBase moveToInternal(int x, int y) override;
    firebase::FutureBase moveToInternal(Position position) override;

    firebase::FutureBase setXInternal(int x) override;
    firebase::FutureBase setYInternal(int y) override;

    firebase::FutureBase setVisibleInternal(bool visible) override;

private:
    firebase::admob::NativeExpressAdView* _nativeAd;

};

/*
 * AdMobInterstitial
 */

class QtFirebaseAdMobInterstitialAdListener;

class QtFirebaseAdMobInterstitial : public QtFirebaseAdMobBase
{
    Q_OBJECT
    Q_ENUMS(PresentationState)

public:
    enum PresentationState
    {
        PresentationStateHidden,
        PresentationStateCoveringUI
    };

    QtFirebaseAdMobInterstitial(QObject* parent = 0);
    ~QtFirebaseAdMobInterstitial();

    void setVisible(bool visible) override;

signals:
    void closed();
    void presentationStateChanged(PresentationState state);

public slots:
    void show() override;
    void setPresentationState(PresentationState state);

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

private:
    firebase::admob::InterstitialAd* _interstitial;
    QtFirebaseAdMobInterstitialAdListener* _interstitialAdListener;

};
Q_DECLARE_METATYPE(QtFirebaseAdMobInterstitial::PresentationState)

// A listener class to an InterstitialAd.
class QtFirebaseAdMobInterstitialAdListener : public firebase::admob::InterstitialAd::Listener {

    friend class QtFirebaseAdMobInterstitial;

public:
    QtFirebaseAdMobInterstitialAdListener(QtFirebaseAdMobInterstitial* qtFirebaseAdMobInterstitial) {
        _qtFirebaseAdMobInterstitial = qtFirebaseAdMobInterstitial;
    }

    void OnPresentationStateChanged(firebase::admob::InterstitialAd* interstitial_ad,
        firebase::admob::InterstitialAd::PresentationState state) override {
        Q_UNUSED(interstitial_ad); // TODO
        qDebug() << _qtFirebaseAdMobInterstitial << "::OnPresentationStateChanged";
        qDebug("InterstitialAd PresentationState has changed to %d.", state);

        auto pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;

        if(state == firebase::admob::InterstitialAd::PresentationState::kPresentationStateHidden) {
            pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;
        } else if(state == firebase::admob::InterstitialAd::PresentationState::kPresentationStateCoveringUI) {
             pState = QtFirebaseAdMobInterstitial::PresentationStateCoveringUI;
        }
        _qtFirebaseAdMobInterstitial->setPresentationState(pState);
    }

private:
    QtFirebaseAdMobInterstitial* _qtFirebaseAdMobInterstitial;
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

class QtFirebaseAdMobRewardedVideoAd : public QtFirebaseAdMobBase, public firebase::admob::rewarded_video::Listener
{
    Q_OBJECT

public:
    QtFirebaseAdMobRewardedVideoAd(QObject* parent = 0);
    ~QtFirebaseAdMobRewardedVideoAd();

    void setVisible(bool visible) override;

signals:
    void closed();
    void presentationStateChanged(QtFirebaseAdMobInterstitial::PresentationState state);
    void rewarded(const QString &type, float value);

public slots:
    void show() override;

private slots:
    void setPresentationState(QtFirebaseAdMobInterstitial::PresentationState state);

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

    void OnRewarded(firebase::admob::rewarded_video::RewardItem reward) override;
    void OnPresentationStateChanged(firebase::admob::rewarded_video::PresentationState state) override;

};

#endif // QTFIREBASE_BUILD_ADMOB
#endif // QTFIREBASE_ADMOB_H

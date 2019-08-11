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
#include "firebase/admob/interstitial_ad.h"
#include "firebase/admob/rewarded_video.h"

#include <QDebug>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QDateTime>
#include <QThread>
#include <QMetaObject>

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
    ~QtFirebaseAdMobBase();

    virtual firebase::FutureBase initInternal() = 0;
    virtual firebase::FutureBase loadInternal() = 0;
    virtual void onInitialized();

public:
    enum PresentationState
    {
        PresentationStateHidden,
        PresentationStateCoveringUI,
        PresentationStateVideoHasStarted,
        PresentationStateVideoHasCompleted
    };
    Q_ENUM(PresentationState)

    bool ready() const;
    virtual void setReady(bool ready);

    bool loaded() const;
    virtual void setLoaded(bool loaded);

    QString adUnitId() const;
    virtual void setAdUnitId(const QString &adUnitId);

    bool visible() const;
    virtual void setVisible(bool visible);

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
    void closed();
    void presentationStateChanged(int state);

public slots:
    virtual void load();
    virtual void show() = 0;

protected slots:
    void init();

private slots:
    void onPresentationStateChanged(int state);

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

class QtFirebaseAdMobBannerViewListener;

class QtFirebaseAdMobBanner : public QtFirebaseAdMobBannerBase
{
    Q_OBJECT
    Q_PROPERTY(QRect boundingBox READ boundingBox NOTIFY boundingBoxChanged)
    Q_PROPERTY(PresentationState presentationState READ presentationState NOTIFY presentationStateChanged)

    friend class QtFirebaseAdMobBannerViewListener;
public:
    enum PresentationState {
        PresentationStateHidden = 0,
        PresentationStateVisibleWithoutAd,
        PresentationStateVisibleWithAd,
        PresentationStateOpenedPartialOverlay,
        PresentationStateCoveringUI
    };
    Q_ENUM(PresentationState)

    QtFirebaseAdMobBanner(QObject* parent = 0);
    ~QtFirebaseAdMobBanner();

    QRect boundingBox() const
    {
        return m_boundingBox;
    }

    PresentationState presentationState() const
    {
        return m_presentationState;
    }

signals:
    void presentationStateChanged(PresentationState state);
    void boundingBoxChanged(const QRect& boundingBox);

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

    firebase::FutureBase moveToInternal(int x, int y) override;
    firebase::FutureBase moveToInternal(Position position) override;

    firebase::FutureBase setXInternal(int x) override;
    firebase::FutureBase setYInternal(int y) override;

    firebase::FutureBase setVisibleInternal(bool visible) override;
    void onInitialized() override;

    void setBoundingBox(QRect boundingBox)
    {
        if (m_boundingBox == boundingBox)
            return;

        m_boundingBox = boundingBox;
        emit boundingBoxChanged(m_boundingBox);
    }

    void setPresentationState(PresentationState presentationState)
    {
        if (m_presentationState == presentationState)
            return;

        m_presentationState = presentationState;
        emit presentationStateChanged(m_presentationState);
    }

private:
    firebase::admob::BannerView* _banner;
    QtFirebaseAdMobBannerViewListener* _bannerViewListener;
    QRect m_boundingBox;
    PresentationState m_presentationState;
};

// A listener class to an BannerView.
class QtFirebaseAdMobBannerViewListener : public firebase::admob::BannerView::Listener {

    friend class QtFirebaseAdMobBanner;

public:
    QtFirebaseAdMobBannerViewListener(QtFirebaseAdMobBanner* qtFirebaseAdMobBanner) {
        _qtFirebaseAdMobBanner = qtFirebaseAdMobBanner;
    }

    void OnPresentationStateChanged(firebase::admob::BannerView* banner_view, firebase::admob::BannerView::PresentationState state) override {
        Q_UNUSED(banner_view); // TODO
        qDebug() << _qtFirebaseAdMobBanner << "::OnPresentationStateChanged";
        qDebug("BannerView PresentationState has changed to %d.", state);

        switch(state) {
            case firebase::admob::BannerView::kPresentationStateHidden:
                qDebug("BannerView PresentationState has changed to kPresentationStateHidden");
                break;
            case firebase::admob::BannerView::kPresentationStateVisibleWithoutAd:
                qDebug("BannerView PresentationState has changed to PresentationStateVisibleWithoutAd");
                break;
            case firebase::admob::BannerView::kPresentationStateVisibleWithAd:
                qDebug("BannerView PresentationState has changed to PresentationStateVisibleWithAd");
                break;
            case firebase::admob::BannerView::kPresentationStateOpenedPartialOverlay:
                qDebug("BannerView PresentationState has changed to PresentationStateOpenedPartialOverlay");
                break;
            case firebase::admob::BannerView::kPresentationStateCoveringUI:
                qDebug("BannerView PresentationState has changed to PresentationStateCoveringUI");
                break;
            default:
                qDebug("BannerView PresentationState has changed to Unknown");
        }

        QMetaObject::invokeMethod(_qtFirebaseAdMobBanner, [this, state]() {
            _qtFirebaseAdMobBanner->setPresentationState((QtFirebaseAdMobBanner::PresentationState) state);
        });
    }

    void OnBoundingBoxChanged(firebase::admob::BannerView *banner_view, firebase::admob::BoundingBox box) override {
        Q_UNUSED(banner_view); // TODO
        qDebug() << _qtFirebaseAdMobBanner << "::OnBoundingBoxChanged";
        QRect boundingBox(box.x, box.y, box.width, box.height);
        qDebug() << "BannerView BoundingBox has changed to" << boundingBox;

        QMetaObject::invokeMethod(_qtFirebaseAdMobBanner, [this, boundingBox]() {
            _qtFirebaseAdMobBanner->setBoundingBox(boundingBox);
        });
    }

private:
    QtFirebaseAdMobBanner* _qtFirebaseAdMobBanner;
};

/*
 * AdMobInterstitial
 */

class QtFirebaseAdMobInterstitialAdListener;

class QtFirebaseAdMobInterstitial : public QtFirebaseAdMobBase
{
    Q_OBJECT

public:
    QtFirebaseAdMobInterstitial(QObject* parent = 0);
    ~QtFirebaseAdMobInterstitial();

public slots:
    void show() override;

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
        Q_UNUSED(interstitial_ad); // TODO
        qDebug() << _qtFirebaseAdMobInterstitial << "::OnPresentationStateChanged";
        qDebug("InterstitialAd PresentationState has changed to %d.", state);

        int pState = QtFirebaseAdMobBase::PresentationStateHidden;

        if(state == firebase::admob::InterstitialAd::PresentationState::kPresentationStateHidden) {
            pState = QtFirebaseAdMobBase::PresentationStateHidden;
        } else if(state == firebase::admob::InterstitialAd::PresentationState::kPresentationStateCoveringUI) {
             pState = QtFirebaseAdMobBase::PresentationStateCoveringUI;
        }
        _qtFirebaseAdMobInterstitial->presentationStateChanged(pState);
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

signals:
    void rewarded(const QString &type, float value);

public slots:
    void show() override;

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

    void OnRewarded(firebase::admob::rewarded_video::RewardItem reward) override;
    void OnPresentationStateChanged(firebase::admob::rewarded_video::PresentationState state) override;

};

#endif // QTFIREBASE_BUILD_ADMOB
#endif // QTFIREBASE_ADMOB_H

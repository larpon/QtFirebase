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
        ErrorUnknown,
        ErrorAlreadyInitialized,
        ErrorInternalError,
        ErrorInvalidRequest,
        ErrorLoadInProgress,
        ErrorNetworkError,
        ErrorNoFill,
        ErrorNoWindowToken,
        ErrorUninitialized
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
    bool checkInstance(const char *function);

    QtFirebaseAdMob::Error convertAdMobErrorCode(int admobErrorCode);

    bool ready();
    void setReady(bool ready);

    QString appId();
    void setAppId(const QString &appId);

    QVariantList testDevices();
    void setTestDevices(QVariantList testDevices);

signals:
    void readyChanged();
    void appIdChanged();
    void testDevicesChanged();

private slots:
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);

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

    QVariantList testDevices();
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
};

/*
 * AdMobBanner
 */
class QtFirebaseAdMobBanner : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

    Q_PROPERTY(QString adUnitId READ adUnitId WRITE setAdUnitId NOTIFY adUnitIdChanged)

    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

    Q_PROPERTY(int x READ getX WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ getY WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ getHeight WRITE setHeight NOTIFY heightChanged)

    Q_PROPERTY(QtFirebaseAdMobRequest* request READ request WRITE setRequest NOTIFY requestChanged)

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

    QtFirebaseAdMobBanner(QObject* parent = 0);
    ~QtFirebaseAdMobBanner();

    bool ready();
    void setReady(bool ready);

    bool loaded();
    void setLoaded(bool loaded);

    QString adUnitId();
    void setAdUnitId(const QString &adUnitId);

    bool visible();
    void setVisible(bool visible);

    int getX();
    void setX(const int &x);

    int getY();
    void setY(const int &y);

    int getWidth();
    void setWidth(const int &width);

    int getHeight();
    void setHeight(const int &height);

    QtFirebaseAdMobRequest* request() const;
    void setRequest(QtFirebaseAdMobRequest *request);

signals:
    void readyChanged();
    void loadedChanged();
    void adUnitIdChanged();

    void visibleChanged();
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void requestChanged();
    void loading();

    void error(QtFirebaseAdMob::Error code, QString message);

public slots:
    void load();
    void show();
    void hide();
    void moveTo(int x, int y);
    void moveTo(int position);

private slots:
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);
    void onApplicationStateChanged(Qt::ApplicationState state);

private:
    QString __QTFIREBASE_ID;
    bool _ready;
    bool _initializing;
    bool _loaded;

    bool _isFirstInit;

    bool _visible;

    int _x;
    int _y;
    int _width;
    int _height;

    QtFirebaseAdMobRequest* _request;

    QString _adUnitId;
    QByteArray __adUnitIdByteArray;
    //const char *__adUnitId; // TODO use __adUnitIdByteArray.constData() instead ?

    void *_nativeUIElement;

    QTimer *_initTimer;

    firebase::admob::BannerView* _banner;

};


/*
 * AdMobInterstitial
 */
class QtFirebaseAdMobInterstitial : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

    Q_PROPERTY(QString adUnitId READ adUnitId WRITE setAdUnitId NOTIFY adUnitIdChanged)

    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

    Q_PROPERTY(QtFirebaseAdMobRequest* request READ request WRITE setRequest NOTIFY requestChanged)

public:
    QtFirebaseAdMobInterstitial(QObject* parent = 0);
    ~QtFirebaseAdMobInterstitial();

    bool ready();
    void setReady(bool ready);

    bool loaded();
    void setLoaded(bool loaded);

    QString adUnitId();
    void setAdUnitId(const QString &adUnitId);

    bool visible() const;
    void setVisible(bool visible);

    QtFirebaseAdMobRequest* request() const;
    void setRequest(QtFirebaseAdMobRequest *request);

signals:
    void readyChanged();
    void loadedChanged();
    void adUnitIdChanged();
    void requestChanged();
    void loading();
    void error(QtFirebaseAdMob::Error code, QString message);
    void closed();
    void visibleChanged();

public slots:
    void load();
    void show();

private slots:
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);

private:
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

    firebase::admob::InterstitialAd* _interstitial;
};

#endif // QTFIREBASE_BUILD_ADMOB


#endif // QTFIREBASE_ADMOB_H

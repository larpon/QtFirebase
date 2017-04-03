#ifndef QTFIREBASE_ADMOB_H
#define QTFIREBASE_ADMOB_H

#ifdef QTFIREBASE_BUILD_ADMOB

#include "src/qtfirebase.h"

#if defined(qFirebaseAdMob)
#undef qFirebaseAdMob
#endif
#define qFirebaseAdMob (static_cast<QtFirebaseAdMob *>(QtFirebaseAdMob::instance()))

#include <QList>
#include <QObject>
#include <QVariant>
#include <QDateTime>

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

    explicit QtFirebaseAdMob(QObject* parent = 0) { Q_UNUSED(parent); }

    static QtFirebaseAdMob *instance() {
        if(self == 0) {
            self = new QtFirebaseAdMob(0);
        }
        return self;
    }
    bool checkInstance(const char *function) {  Q_UNUSED(function); return false; }

    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready); }

    QString appId() { return ""; }
    void setAppId(const QString &appId) { Q_UNUSED(appId); }

    QVariantList testDevices() { return QVariantList(); }
    void setTestDevices(QVariantList testDevices) { Q_UNUSED(testDevices); }

signals:
    void readyChanged();
    void appIdChanged();
    void testDevicesChanged();

private:
    static QtFirebaseAdMob *self;
    Q_DISABLE_COPY(QtFirebaseAdMob)

};


/*
 * AdMobRequest
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
    QtFirebaseAdMobRequest(QObject* parent = 0) { Q_UNUSED(parent); }
    ~QtFirebaseAdMobRequest() {}

    int gender() const { return 0; }
    void setGender(int gender) {Q_UNUSED(gender);}

    int childDirectedTreatment() const { return 0; }
    void setChildDirectedTreatment(int childDirectedTreatment) {Q_UNUSED(childDirectedTreatment);}

    QDateTime birthday() const { return QDateTime(); }
    void setBirthday(const QDateTime &birthday) {Q_UNUSED(birthday);}

    QVariantList keywords() const { return QVariantList(); }
    void setKeywords(const QVariantList &keywords) {Q_UNUSED(keywords);}

    QVariantList extras() const { return QVariantList(); }
    void setExtras(const QVariantList &extras) {Q_UNUSED(extras);}

    QVariantList testDevices() { return QVariantList(); }
    void setTestDevices(QVariantList &testDevices) {Q_UNUSED(testDevices);}

signals:
    void genderChanged();
    void childDirectedTreatmentChanged();
    void birthdayChanged();
    void keywordsChanged();
    void extrasChanged();
    void testDevicesChanged();
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
    QtFirebaseAdMobBanner(QObject* parent = 0) { Q_UNUSED(parent); }
    ~QtFirebaseAdMobBanner() {}

    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready); }

    bool loaded() { return false; }
    void setLoaded(bool loaded) { Q_UNUSED(loaded); }

    QString adUnitId() { return ""; }
    void setAdUnitId(const QString &adUnitId) { Q_UNUSED(adUnitId); }

    bool visible() { return false; }
    void setVisible(bool visible) { Q_UNUSED(visible); }

    int getX() { return 0; }
    void setX(const int &x) { Q_UNUSED(x); }

    int getY() { return 0; }
    void setY(const int &y) { Q_UNUSED(y); }

    int getWidth() {return 0; }
    void setWidth(const int &width) { Q_UNUSED(width); }

    int getHeight() { return 0; }
    void setHeight(const int &height) { Q_UNUSED(height); }

    QtFirebaseAdMobRequest* request()  { return 0; }
    void setRequest(QtFirebaseAdMobRequest *request) { Q_UNUSED(request); }

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

    void error(int errorCode);

public slots:
    void load() {}
    void show() {}
    void hide() {}
    void moveTo(int x, int y) { Q_UNUSED(x); Q_UNUSED(y); }

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

    Q_PROPERTY(QtFirebaseAdMobRequest* request READ request WRITE setRequest NOTIFY requestChanged)

public:
    QtFirebaseAdMobInterstitial(QObject* parent = 0) { Q_UNUSED(parent); }
    ~QtFirebaseAdMobInterstitial() {}

    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready); }

    bool loaded() { return false; }
    void setLoaded(bool loaded) { Q_UNUSED(loaded); }

    QString adUnitId() { return ""; }
    void setAdUnitId(const QString &adUnitId) { Q_UNUSED(adUnitId); }

    bool visible() { return false; }
    void setVisible(bool visible) { Q_UNUSED(visible); }

    QtFirebaseAdMobRequest* request()  { return 0; }
    void setRequest(QtFirebaseAdMobRequest *request) { Q_UNUSED(request); }

signals:
    void readyChanged();
    void loadedChanged();
    void adUnitIdChanged();
    void visibleChanged();
    void error(int errorCode);
    void closed();
    void loading();
    void requestChanged();

public slots:
    void load() {}
    void show() {}

};

/*
 * AdMobRewardedVideoAd
 */

class QtFirebaseAdMobRewardedVideoAd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(QString adUnitId READ adUnitId WRITE setAdUnitId NOTIFY adUnitIdChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QtFirebaseAdMobRequest* request READ request WRITE setRequest NOTIFY requestChanged)

public:
    enum PresentationState
    {
        PresentationStateHidden,
        PresentationStateCoveringUI
    };
    Q_ENUM(PresentationState)

    QtFirebaseAdMobRewardedVideoAd(QObject* parent = 0){Q_UNUSED(parent);}
    ~QtFirebaseAdMobRewardedVideoAd(){}

    bool ready()const {return false;}
    void setReady(bool ready){Q_UNUSED(ready);}

    bool loaded()const {return false;}
    void setLoaded(bool loaded){Q_UNUSED(loaded);}

    QString adUnitId()const {return QString();}
    void setAdUnitId(const QString &adUnitId){Q_UNUSED(adUnitId);}

    bool visible() const{return false;}
    void setVisible(bool visible){Q_UNUSED(visible);}

    QtFirebaseAdMobRequest* request() const{return nullptr;}
    void setRequest(QtFirebaseAdMobRequest *request){Q_UNUSED(request);}

signals:
    void readyChanged();
    void loadedChanged();
    void adUnitIdChanged();
    void requestChanged();
    void loading();
    void error(int code, QString message);
    void closed();
    void visibleChanged();
    void presentationStateChanged(int state);
    void rewarded(QString type, float value);

public slots:
    void load(){}
    void show(){}
};

#endif // QTFIREBASE_BUILD_ADMOB


#endif // QTFIREBASE_ADMOB_H

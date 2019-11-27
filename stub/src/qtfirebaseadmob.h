#ifndef QTFIREBASE_ADMOB_H
#define QTFIREBASE_ADMOB_H

#ifdef QTFIREBASE_BUILD_ADMOB

#include "qtfirebase.h"

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

    explicit QtFirebaseAdMob(QObject* parent = nullptr) { Q_UNUSED(parent) }

    static QtFirebaseAdMob *instance() {
        if(self == nullptr) {
            self = new QtFirebaseAdMob(nullptr);
        }
        return self;
    }
    bool checkInstance(const char *function) {  Q_UNUSED(function) return false; }

    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready) }

    QString appId() { return QStringLiteral(""); }
    void setAppId(const QString &appId) { Q_UNUSED(appId) }

    QVariantList testDevices() { return QVariantList(); }
    void setTestDevices(QVariantList testDevices) { Q_UNUSED(testDevices) }

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

    QtFirebaseAdMobRequest(QObject* parent = nullptr) { Q_UNUSED(parent) }
    ~QtFirebaseAdMobRequest() {}

    int gender() const { return 0; }
    void setGender(int gender) { Q_UNUSED(gender) }

    int childDirectedTreatment() const { return 0; }
    void setChildDirectedTreatment(int childDirectedTreatment) { Q_UNUSED(childDirectedTreatment) }

    QDateTime birthday() const { return QDateTime(); }
    void setBirthday(const QDateTime &birthday) { Q_UNUSED(birthday) }

    QVariantList keywords() const { return QVariantList(); }
    void setKeywords(const QVariantList &keywords) { Q_UNUSED(keywords) }

    QVariantList extras() const { return QVariantList(); }
    void setExtras(const QVariantList &extras) { Q_UNUSED(extras) }

    QVariantList testDevices() { return QVariantList(); }
    void setTestDevices(QVariantList &testDevices) { Q_UNUSED(testDevices) }

signals:
    void genderChanged();
    void childDirectedTreatmentChanged();
    void birthdayChanged();
    void keywordsChanged();
    void extrasChanged();
    void testDevicesChanged();
};

/*
 * AdMobBase
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
    QtFirebaseAdMobBase(QObject* parent = nullptr)  { Q_UNUSED(parent) }
    virtual ~QtFirebaseAdMobBase(){}

public:
    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready) }

    bool loaded() { return false; }
    void setLoaded(bool loaded) { Q_UNUSED(loaded) }

    QString adUnitId() { return QStringLiteral(""); }
    void setAdUnitId(const QString &adUnitId) { Q_UNUSED(adUnitId) }

    bool visible() { return false; }
    void setVisible(bool visible) { Q_UNUSED(visible) }

    QtFirebaseAdMobRequest* request()  { return nullptr; }
    void setRequest(QtFirebaseAdMobRequest *request) { Q_UNUSED(request) }

signals:
    void adUnitIdChanged();
    void error(int code, QString message);
    void loadedChanged();
    void loading();
    void readyChanged();
    void requestChanged();
    void visibleChanged();

public slots:
    virtual void load() {}
    virtual void show() {}

};

/*
 * AdMobBanner
 */

class QtFirebaseAdMobBanner : public QtFirebaseAdMobBase
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

    QtFirebaseAdMobBanner(QObject* parent = nullptr) { Q_UNUSED(parent) }
    ~QtFirebaseAdMobBanner() {}

    int getX() { return 0; }
    void setX(const int &x) { Q_UNUSED(x) }

    int getY() { return 0; }
    void setY(const int &y) { Q_UNUSED(y) }

    int getWidth() {return 0; }
    void setWidth(const int &width) { Q_UNUSED(width) }

    int getHeight() { return 0; }
    void setHeight(const int &height) { Q_UNUSED(height) }

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();

public slots:
    void hide() {}
    void moveTo(int x, int y) { Q_UNUSED(x) Q_UNUSED(y) }
    void moveTo(int position) { Q_UNUSED(position) }
};

/*
 * AdMobInterstitial
 */

class QtFirebaseAdMobInterstitial : public QtFirebaseAdMobBase
{
    Q_OBJECT

public:
    enum PresentationState
    {
        PresentationStateHidden,
        PresentationStateCoveringUI
    };
    Q_ENUM(PresentationState)

    QtFirebaseAdMobInterstitial(QObject* parent = nullptr){ Q_UNUSED(parent) }
    ~QtFirebaseAdMobInterstitial() {}

signals:
    void closed();
    void presentationStateChanged(int state);
};

/*
 * AdMobRewardedVideoAd
 */

class QtFirebaseAdMobRewardedVideoAd : public QtFirebaseAdMobBase
{
    Q_OBJECT

public:
    enum PresentationState
    {
        PresentationStateHidden,
        PresentationStateCoveringUI
    };
    Q_ENUM(PresentationState)

    QtFirebaseAdMobRewardedVideoAd(QObject* parent = nullptr){ Q_UNUSED(parent) }
    ~QtFirebaseAdMobRewardedVideoAd(){}

signals:
    void closed();
    void visibleChanged();
    void presentationStateChanged(int state);
    void rewarded(QString type, float value);
};

#endif // QTFIREBASE_BUILD_ADMOB
#endif // QTFIREBASE_ADMOB_H

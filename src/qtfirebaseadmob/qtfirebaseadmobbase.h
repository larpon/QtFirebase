#ifndef QTFIREBASE_ADMOB_BASE_H
#define QTFIREBASE_ADMOB_BASE_H

#include <QObject>
#include <QTimer>

#include "src/qtfirebase.h"

#include "qtfirebaseadmob.h"
#include "qtfirebaseadmobrequest.h"

#include "firebase/admob.h"

class QtFirebaseAdMobBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

    Q_PROPERTY(QString adUnitId READ adUnitId WRITE setAdUnitId NOTIFY adUnitIdChanged)

    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

    Q_PROPERTY(QtFirebaseAdMobRequest* request READ request WRITE setRequest NOTIFY requestChanged)

protected:
    QtFirebaseAdMobBase(QObject* parent = nullptr);
    ~QtFirebaseAdMobBase();

    virtual firebase::FutureBase initInternal() = 0;
    virtual firebase::FutureBase loadInternal() = 0;
    virtual void onInitialized();

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

#endif // QTFIREBASE_ADMOB_BASE_H

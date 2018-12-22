#ifndef QTFIREBASE_REMOTE_CONFIG_H
#define QTFIREBASE_REMOTE_CONFIG_H

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG

#include "src/qtfirebase.h"

#if defined(qFirebaseRemoteConfig)
#undef qFirebaseRemoteConfig
#endif
#define qFirebaseRemoteConfig (static_cast<QtFirebaseRemoteConfig *>(QtFirebaseRemoteConfig::instance()))

#include "firebase/remote_config.h"

#include <memory> //For std::unique_ptr
#include <QDebug>
#include <QObject>

class QtFirebaseRemoteConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(long long cacheExpirationTime READ cacheExpirationTime WRITE setCacheExpirationTime NOTIFY cacheExpirationTimeChanged)

public:
    explicit QtFirebaseRemoteConfig(QObject *parent = 0);
    ~QtFirebaseRemoteConfig();

    enum FetchFailure
    {
        FetchFailureReasonInvalid = firebase::remote_config::kFetchFailureReasonInvalid,
        FetchFailureReasonThrottled = firebase::remote_config::kFetchFailureReasonThrottled,
        FetchFailureReasonError = firebase::remote_config::kFetchFailureReasonError,
    };
    Q_ENUM(FetchFailure)

    static QtFirebaseRemoteConfig *instance() {
        if(self == 0) {
            self = new QtFirebaseRemoteConfig(0);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    bool checkInstance(const char *function);
    bool ready();

    QVariantMap parameters() const;
    void setParameters(const QVariantMap& map);

    long long cacheExpirationTime() const;
    void setCacheExpirationTime(long long timeMs);

public slots:
    void addParameter(const QString &name, long long defaultValue);
    void addParameter(const QString &name, double defaultValue);
    void addParameter(const QString &name, const QString& defaultValue);
    void addParameter(const QString &name, bool defaultValue);
    QVariant getParameterValue(const QString &name) const;

    //If the data in the cache was fetched no longer than cacheExpirationTime ago,
    //this method will return the cached data. If not, a fetch from the
    //Remote Config Server will be attempted.
    //cacheExpirationTime in QtFirebase should be set in milliseconds
    //See for details
    //https://firebase.google.com/docs/reference/android/com/google/firebase/remoteconfig/FirebaseRemoteConfig#fetch()
    void fetch();
    void fetchNow();//calls fetch with cacheExpirationTime = 0

signals:
    void readyChanged();
    void error(int code, QString message);
    void parametersChanged();
    void cacheExpirationTimeChanged();

private slots:
    void addParameterInternal(const QString &name, const QVariant &defaultValue);
    void delayedInit();
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);
    void onFutureEventInit(firebase::FutureBase& future);
    void onFutureEventFetch(firebase::FutureBase& future);

private:
    void setReady(bool ready);
    void fetch(long long cacheExpirationInSeconds);

    static QtFirebaseRemoteConfig *self;
    Q_DISABLE_COPY(QtFirebaseRemoteConfig)

    QString __QTFIREBASE_ID;

    bool _ready;
    bool _initializing;
    QVariantMap _parameters;
    long long _cacheExpirationTime;
    ::firebase::ModuleInitializer _initializer;

    QString _appId;
    QByteArray __appIdByteArray;

    QByteArrayList __defaultsByteArrayList;
    const char *__appId;
};

#endif //QTFIREBASE_BUILD_REMOTE_CONFIG
#endif // QTFIREBASE_REMOTE_CONFIG_H

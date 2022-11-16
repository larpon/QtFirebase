#ifndef QTFIREBASE_REMOTE_CONFIG_H
#define QTFIREBASE_REMOTE_CONFIG_H

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG

#include "src/qtfirebase.h"

#include <firebase/remote_config.h>

#ifdef qFirebaseRemoteConfig
#undef qFirebaseRemoteConfig
#endif
#define qFirebaseRemoteConfig (QtFirebaseRemoteConfig::instance())

class QtFirebaseRemoteConfig : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QtFirebaseRemoteConfig)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(quint64 cacheExpirationTime READ cacheExpirationTime WRITE setCacheExpirationTime NOTIFY cacheExpirationTimeChanged)
    static QtFirebaseRemoteConfig *self;
public:
    static QtFirebaseRemoteConfig *instance(QObject *parent = nullptr);

    enum FetchFailure {
        FetchFailureReasonInvalid = firebase::remote_config::kFetchFailureReasonInvalid,
        FetchFailureReasonThrottled = firebase::remote_config::kFetchFailureReasonThrottled,
        FetchFailureReasonError = firebase::remote_config::kFetchFailureReasonError,
    };
    Q_ENUM(FetchFailure)

    explicit QtFirebaseRemoteConfig(QObject *parent = nullptr);
    virtual ~QtFirebaseRemoteConfig();

    bool ready() const { return _ready; }

    QVariantMap parameters() const { return _parameters; }
    quint64 cacheExpirationTime() const { return _cacheExpirationTime; }
    void setParameters(const QVariantMap &);
    void setCacheExpirationTime(quint64 ms);

    Q_INVOKABLE QVariant getParameterValue(const QString &name) const { return _parameters[name]; }

public slots:
    void addParameter(const QString &name, bool defaultValue) { addParameterInternal(name, defaultValue); }
    void addParameter(const QString &name, long long defaultValue) { addParameterInternal(name, defaultValue); }
    void addParameter(const QString &name, double defaultValue) { addParameterInternal(name, defaultValue); }
    void addParameter(const QString &name, const QString &defaultValue) { addParameterInternal(name, defaultValue); }

    void fetch() { fetch(_cacheExpirationTime / 1000); }
    void fetchNow() { fetch(0); }

signals:
    void readyChanged();
    void parametersChanged();
    void cacheExpirationTimeChanged();

    void error(int code, QString message);
private slots:
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);
    void onFutureEventInit(const firebase::FutureBase& future);
    void onFutureEventFetch(firebase::FutureBase& future);

private:
    void setReady(bool);
    void addParameterInternal(const QString &name, const QVariant &defaultValue) { _parameters[name] = defaultValue; }
    void fetch(quint64 cacheExpirationInSeconds);
private:
    QString __QTFIREBASE_ID;

    bool _ready;
    bool _initializing;
    QVariantMap _parameters;
    quint64 _cacheExpirationTime;
    ::firebase::ModuleInitializer _initializer;

    QString _appId;
    QByteArray __appIdByteArray;

    QByteArrayList __defaultsByteArrayList;
    const char *__appId;
};

#endif // QTFIREBASE_BUILD_REMOTE_CONFIG
#endif // QTFIREBASE_REMOTE_CONFIG_H

#ifndef QTFIREBASE_REMOTE_CONFIG_H
#define QTFIREBASE_REMOTE_CONFIG_H

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG

#include "src/qtfirebase.h"

#include <firebase/remote_config.h>

#include <QObject>
#include <QVariant>
#include <QVariantMap>

#ifdef qFirebaseRemoteConfig
#undef qFirebaseRemoteConfig
#endif
#define qFirebaseRemoteConfig (QtFirebaseRemoteConfig::instance())

class QtFirebaseRemoteConfig : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QtFirebaseRemoteConfig)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)
    Q_PROPERTY(quint64 cacheExpirationTime READ cacheExpirationTime WRITE setCacheExpirationTime NOTIFY cacheExpirationTimeChanged)
    Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters NOTIFY parametersChanged)

    static QtFirebaseRemoteConfig *self;
public:
    static QtFirebaseRemoteConfig *instance(QObject *parent = nullptr) {
        if (!self)
            self = new QtFirebaseRemoteConfig(parent);
        return self;
    }

    static bool checkInstance(const char *function = nullptr) { Q_UNUSED(function) return self; }

    enum FetchFailure {
        FetchFailureReasonInvalid = firebase::remote_config::kFetchFailureReasonInvalid,
        FetchFailureReasonThrottled = firebase::remote_config::kFetchFailureReasonThrottled,
        FetchFailureReasonError = firebase::remote_config::kFetchFailureReasonError,
    };
    Q_ENUM(FetchFailure)

    explicit QtFirebaseRemoteConfig(QObject *parent = nullptr);
    virtual ~QtFirebaseRemoteConfig();

    bool ready() const { return _ready; }
    bool fetching() const { return _fetching; }
    quint64 cacheExpirationTime() const { return _cacheExpirationTime; }
    QVariantMap parameters() const { return _parameters; }

    void setCacheExpirationTime(quint64 ms);
    void setParameters(const QVariantMap &);

    Q_INVOKABLE QVariant getParameterValue(const QString &name) const { return _parameters[name]; }

public slots:
    void addParameter(const QString &name, bool defaultValue) { addParameterInternal(name, defaultValue); }
    void addParameter(const QString &name, long long defaultValue) { addParameterInternal(name, defaultValue); }
    void addParameter(const QString &name, double defaultValue) { addParameterInternal(name, defaultValue); }
    void addParameter(const QString &name, const QString &defaultValue) { addParameterInternal(name, defaultValue); }

    void fetchNow() { fetch(0); }
    void fetch() { fetch(_cacheExpirationTime / 1000); }
signals:

    void readyChanged();
    void fetchingChanged();
    void cacheExpirationTimeChanged();
    void parametersChanged();

    void googlePlayServicesError(); ///< For Firebase CPP SDK < 8.0.0 only

    void futuresError(int code, QString message); ///< System error.
    void error(int code, QString message); ///< Error of last fetch.

private slots:
    void init();
    void onFutureEvent(const QString &eventId, firebase::FutureBase);
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    void onFutureEventInit();
    void onFutureEventDefaults();
    void onFutureEventActivate();
#endif
    void onFutureEventFetch();
private:
    void setReady(bool = true);
    void setFetching(bool = true);
    void setParametersAndFetching(const QVariantMap &, bool);
    void addParameterInternal(const QString &name, const QVariant &defaultValue) { _parameters[name] = defaultValue; }
    void fetch(quint64 cacheExpirationInSeconds);

    void updateParameters();
private:
    const QString __QTFIREBASE_ID;

    bool _ready = false;
    bool _fetching = false;
    quint64 _cacheExpirationTime = firebase::remote_config::kDefaultCacheExpiration * 1000;
    QVariantMap _parameters;

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    quint64 _cacheExpirationInSeconds = 0;
    firebase::remote_config::RemoteConfig *_rc = nullptr;
#endif
};

#endif // QTFIREBASE_BUILD_REMOTE_CONFIG
#endif // QTFIREBASE_REMOTE_CONFIG_H

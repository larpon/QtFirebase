#ifndef QTFIREBASE_REMOTE_CONFIG_H
#define QTFIREBASE_REMOTE_CONFIG_H

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG

#include "src/qtfirebase.h"

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

    explicit QtFirebaseRemoteConfig(QObject *parent = nullptr) : QObject(parent) { }
    virtual ~QtFirebaseRemoteConfig() { }

    bool ready() const { return false; }
    bool fetching() const { return false; }
    quint64 cacheExpirationTime() const { return 0; }
    QVariantMap parameters() const { return QVariantMap(); }

    void setCacheExpirationTime(quint64) { }
    void setParameters(const QVariantMap &) { }

    Q_INVOKABLE QVariant getParameterValue(const QString &) const { return QVariant(); }

public slots:
    void addParameter(const QString &, bool) { }
    void addParameter(const QString &, long long) { }
    void addParameter(const QString &, double) { }
    void addParameter(const QString &, const QString &) { }

    void fetchNow() { }
    void fetch() { }
signals:

    void readyChanged();
    void fetchingChanged();
    void cacheExpirationTimeChanged();
    void parametersChanged();

    void googlePlayServicesError(); ///< For Firebase CPP SDK < 8.0.0 only

    void futuresError(int code, QString message); ///< System error.
    void error(int code, QString message); ///< Error of last fetch.
};

#endif // QTFIREBASE_BUILD_REMOTE_CONFIG
#endif // QTFIREBASE_REMOTE_CONFIG_H

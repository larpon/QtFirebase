#ifndef QTFIREBASE_REMOTE_CONFIG_H
#define QTFIREBASE_REMOTE_CONFIG_H

#include <QObject>

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG

#include "src/qtfirebase.h"
#include <QVariantMap>

#if defined(qFirebaseRemoteConfig)
#undef qFirebaseRemoteConfig
#endif
#define qFirebaseRemoteConfig (static_cast<QtFirebaseRemoteConfig *>(QtFirebaseRemoteConfig::instance()))

class QtFirebaseRemoteConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(long long cacheExpirationTime READ cacheExpirationTime WRITE setCacheExpirationTime NOTIFY cacheExpirationTimeChanged)

public:
    enum Error
    {
        kFetchFailureReasonInvalid,
        kFetchFailureReasonThrottled,
        kFetchFailureReasonError,
    };
    Q_ENUM(Error)

    explicit QtFirebaseRemoteConfig(QObject *parent = 0){ Q_UNUSED(parent); }

    ~QtFirebaseRemoteConfig() {}

    static QtFirebaseRemoteConfig *instance() {
            if(self == 0) {
                self = new QtFirebaseRemoteConfig(0);
            }
            return self;
        }

    bool checkInstance(const char *function);
    bool ready(){return false;}

    QVariantMap parameters() const{return QVariantMap();}
    void setParameters(const QVariantMap& map){Q_UNUSED(map);}

    long long cacheExpirationTime() const{return 0;}
    void setCacheExpirationTime(long long timeMs){Q_UNUSED(timeMs);}

public slots:
    void addParameter(const QString &name, long long defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    void addParameter(const QString &name, double defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    void addParameter(const QString &name, const QString& defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    void addParameter(const QString &name, bool defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    QVariant getParameterValue(const QString) const{ return QString(); }

    void fetch(){}
    void fetchNow(){}

signals:
    void readyChanged();
    void error(Error code, QString message);
    void parametersChanged();
    void cacheExpirationTimeChanged();

private:
    static QtFirebaseRemoteConfig *self;
    Q_DISABLE_COPY(QtFirebaseRemoteConfig)
};

#endif //QTFIREBASE_BUILD_REMOTE_CONFIG
#endif // QTFIREBASE_REMOTE_CONFIG_H

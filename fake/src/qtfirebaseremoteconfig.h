#ifndef QTFIREBASEREMOTECONFIG_H
#define QTFIREBASEREMOTECONFIG_H

#include <QObject>

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG

#include "src/qtfirebase.h"
#include <QDebug>

#if defined(qFirebaseRemoteConfig)
#undef qFirebaseRemoteConfig
#endif
#define qFirebaseRemoteConfig (static_cast<QtFirebaseRemoteConfig *>(QtFirebaseRemoteConfig::instance()))

class QtFirebaseRemoteConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
public:
    enum Error
    {
        kFetchFailureReasonInvalid,
        kFetchFailureReasonThrottled,
        kFetchFailureReasonError,
    };
    Q_ENUM(Error)

    explicit QtFirebaseRemoteConfig(QObject *parent = 0){Q_UNUSED(parent)}
    ~QtFirebaseRemoteConfig() {}
    QtFirebaseRemoteConfig *instance() {
            if(self == 0) {
                self = new QtFirebaseRemoteConfig(0);
                qDebug() << self << "::instance" << "singleton";
            }
            return self;
        }
    bool checkInstance(const char *function);

    bool ready(){return false;}
    void setReady(bool ready){Q_UNUSED(ready)}

    bool loaded(){return false;}
    void setLoaded(bool loaded){Q_UNUSED(loaded)}

    void addParameter(const QString &name, long long defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    void addParameter(const QString &name, double defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    void addParameter(const QString &name, const QString& defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    void addParameter(const QString &name, bool defaultValue){Q_UNUSED(name); Q_UNUSED(defaultValue);}
    QVariant getParameterValue(const QString &name) const {Q_UNUSED(name);return QVariant();}
    void requestConfig(long long cacheExpirationInSeconds = 0){Q_UNUSED(cacheExpirationInSeconds);}
signals:
    void readyChanged();
    void loadedChanged();
    void error(Error code, QString message);

private:
    static QtFirebaseRemoteConfig *self;
    Q_DISABLE_COPY(QtFirebaseRemoteConfig)
};

#endif //QTFIREBASE_BUILD_REMOTE_CONFIG

#endif // QTFIREBASEREMOTECONFIG_H

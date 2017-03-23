#ifndef QTFIREBASEREMOTECONFIG_H
#define QTFIREBASEREMOTECONFIG_H

#include <QObject>

#ifdef QTFIREBASE_BUILD_REMOTE_CONFIG

#include "src/qtfirebase.h"

#if defined(qFirebaseRemoteConfig)
#undef qFirebaseRemoteConfig
#endif
#define qFirebaseRemoteConfig (static_cast<QtFirebaseRemoteConfig *>(QtFirebaseRemoteConfig::instance()))

#include "firebase/remote_config.h"

class QtFirebaseRemoteConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
public:
    explicit QtFirebaseRemoteConfig(QObject *parent = 0);

    enum Error
    {
        kFetchFailureReasonInvalid = firebase::remote_config::kFetchFailureReasonInvalid,
        kFetchFailureReasonThrottled = firebase::remote_config::kFetchFailureReasonThrottled,
        kFetchFailureReasonError = firebase::remote_config::kFetchFailureReasonError,

    };
    Q_ENUM(Error)

    QtFirebaseRemoteConfig *instance() {
            if(self == 0) {
                self = new QtFirebaseRemoteConfig(0);
                qDebug() << self << "::instance" << "singleton";
            }
            return self;
        }
    bool checkInstance(const char *function);

    bool ready();
    void setReady(bool ready);

    bool loaded();
    void setLoaded(bool loaded);

public slots:
    void addParameter(const QString &name, long long defaultValue);
    void addParameter(const QString &name, double defaultValue);
    void addParameter(const QString &name, const QString& defaultValue);
    void addParameter(const QString &name, bool defaultValue);

    QVariant getParameterValue(const QString &name) const;
    void requestConfig(long long cacheExpirationInSeconds = firebase::remote_config::kDefaultCacheExpiration);

signals:
    void readyChanged();
    void loadedChanged();
    void error(Error code, QString message);

private slots:
    void addParameterInternal(const QString &name, const QVariant &defaultValue);
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);
private:
    static QtFirebaseRemoteConfig *self;
    Q_DISABLE_COPY(QtFirebaseRemoteConfig)

    QString __QTFIREBASE_ID;

    bool _ready;
    bool _initializing;
    bool _loaded;
    typedef QMap<QString, QVariant> ParametersMap;
    ParametersMap _parameters;

    QString _appId;
    QByteArray __appIdByteArray;
    const char *__appId;

};

#endif //QTFIREBASE_BUILD_REMOTE_CONFIG

#endif // QTFIREBASEREMOTECONFIG_H

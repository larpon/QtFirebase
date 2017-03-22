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
/*How to use in QML example
 * 1. First add parameters in remote config in google firebase console

  RemoteConfig{
        id: remoteConfig
        onReadyChanged: {
            console.log("RemoteConfig ready changed:"+ready);
            if(ready)
            {
                //2. Init remote config with parameters you want to retrieve and default values
                //default value returned if fetch config from server failed
                remoteConfig.addParameter("TestLong", 0);
                remoteConfig.addParameter("TestBool", false);
                remoteConfig.addParameter("TestDouble", 3.14);
                remoteConfig.addParameter("TestString","xxx");
                //3. Initiate fetch (in this example set cache expiration time to 1 second)
                //Be aware of set low cache expiration time since it will cause too much
                //requests to server, and it may cause you will be blocked for some time.
                //This called server throttling, server just refuse your requests for some time and
                //then begin accept connections again
                //Default time cache expiration is 12 hours

                remoteConfig.requestConfig(1);
            }
        }
        onLoadedChanged:{
            console.log("RemoteConfig loaded changed:"+loaded);
            if(loaded)
            {

                //4. Retrieve data if loading success
                console.log("RemoteConfig TestLong:" + remoteConfig.getParameterValue("TestLong"));
                console.log("RemoteConfig TestBool:" + remoteConfig.getParameterValue("TestBool"));
                console.log("RemoteConfig TestDouble:" + remoteConfig.getParameterValue("TestDouble"));
                console.log("RemoteConfig TestString:" + remoteConfig.getParameterValue("TestString"));
            }
        }
        onError:{
            //5. Handle errors
            console.log("RemoteConfig error:" + message);
        }
    }
 */

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

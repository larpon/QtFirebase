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

/*Feature information available at https://firebase.google.com/docs/remote-config/

  How to use in QML example
  0. Add parameters in remote config section at google firebase web console (https://console.firebase.google.com)

  RemoteConfig{
        id: remoteConfig

        //1. Initialize parameters you would like to fetch from server and their default values
        parameters: {
            "TestString" : "test",
            "TestDouble" : 2.56,
            "TestLong" : 1100,
            "TestBool" : true
        }

        //2. Set cache expiration time in milliseconds, see step 3 for details about cache
        cacheExpirationTime: 12*3600*1000 //12 hours in milliseconds (suggested as default in firebase)

        //3. When remote config properly initialized request data from server
        onReadyChanged: {
            console.log("RemoteConfig ready changed:"+ready);
            if(ready)
            {
                remoteConfig.fetch();
                //If the data in the cache was fetched no longer than cacheExpirationTime ago,
                //this method will return the cached data. If not, a fetch from the
                //Remote Config Server will be attempted.
                //If you need to get data urgent use fetchNow(), it is equal to fetch() call with cacheExpirationTime=0
                //Be careful with urgent requests, too often requests will result to server throthling
                //which means it will refuse connections for some time
            }
        }

        //4. If data was retrieved (both from server or cache) the handler will be called
        //you can access data by accessing the "parameters" member variable

        onParametersChanged:{
            console.log("RemoteConfig TestString:" + parameters['TestString']);
            console.log("RemoteConfig TestDouble:" + parameters['TestDouble']);
            console.log("RemoteConfig TestLong:" + parameters['TestLong']);
            console.log("RemoteConfig TestBool:" + parameters['TestBool']);
        }

        //5. Handle errors
        onError:{
            console.log("RemoteConfig error code:" + code + " message:" + message);
        }
    }
 */

class QtFirebaseRemoteConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(long long cacheExpirationTime READ cacheExpirationTime WRITE setCacheExpirationTime NOTIFY cacheExpirationTimeChanged)
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
    void error(Error code, QString message);
    void parametersChanged();
    void cacheExpirationTimeChanged();

private slots:
    void addParameterInternal(const QString &name, const QVariant &defaultValue);
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);

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

    QString _appId;
    QByteArray __appIdByteArray;
    const char *__appId;

};

#endif //QTFIREBASE_BUILD_REMOTE_CONFIG

#endif // QTFIREBASEREMOTECONFIG_H

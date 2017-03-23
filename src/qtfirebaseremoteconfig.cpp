#include "qtfirebaseremoteconfig.h"
#include <memory>
namespace remote_config = ::firebase::remote_config;

QtFirebaseRemoteConfig *QtFirebaseRemoteConfig::self = 0;

QtFirebaseRemoteConfig::QtFirebaseRemoteConfig(QObject *parent) :
    QObject(parent),
    _ready(false),
    _initializing(false),
    _loaded(false),
    __appId(nullptr)
{
    __QTFIREBASE_ID = QString().sprintf("%8p", this);
    if(self == 0)
    {
        self = this;
        qDebug() << self << "::QtFirebaseRemoteConfig" << "singleton";
    }

    if(qFirebase->ready())
    {
        //Call init outside of constructor, otherwise signal readyChanged not emited
        QTimer::singleShot(500, this, SLOT(init()));
    }
    else
    {
        connect(qFirebase,&QtFirebase::readyChanged, this, &QtFirebaseRemoteConfig::init);
        qFirebase->requestInit();
    }
    connect(qFirebase,&QtFirebase::futureEvent, this, &QtFirebaseRemoteConfig::onFutureEvent);
}

bool QtFirebaseRemoteConfig::checkInstance(const char *function)
{
    bool b = (QtFirebaseRemoteConfig::self != 0);
    if(!b) qWarning("QtFirebaseRemoteConfig::%s:", function);
    return b;
}

void QtFirebaseRemoteConfig::addParameterInternal(const QString &name, const QVariant &defaultValue)
{
    _parameters[name] = defaultValue;
}

QVariant QtFirebaseRemoteConfig::getParameterValue(const QString &name) const
{

    if(_parameters.contains(name))
    {
        return _parameters[name];
    }
    return _parameters[name];
}

bool QtFirebaseRemoteConfig::ready()
{
    return _ready;
}

void QtFirebaseRemoteConfig::setReady(bool ready)
{
    qDebug() << this << "::setReady" << ready<<_ready;
    if (_ready != ready) {
        _ready = ready;
        qDebug() << this << "::setReady" << "emit ready";
        emit readyChanged();
    }
}

bool QtFirebaseRemoteConfig::loaded()
{
    return _loaded;
}

void QtFirebaseRemoteConfig::setLoaded(bool loaded)
{
    if(_loaded != loaded) {
        _loaded = loaded;
        qDebug() << this << "::setLoaded" << _loaded;
        emit loadedChanged();
    }
}

void QtFirebaseRemoteConfig::addParameter(const QString &name, long long defaultValue)
{
    addParameterInternal(name, defaultValue);
}

void QtFirebaseRemoteConfig::addParameter(const QString &name, double defaultValue)
{
    addParameterInternal(name, defaultValue);
}

void QtFirebaseRemoteConfig::addParameter(const QString &name, const QString &defaultValue)
{
    addParameterInternal(name, defaultValue);
}

void QtFirebaseRemoteConfig::addParameter(const QString &name, bool defaultValue)
{
    addParameterInternal(name, defaultValue);
}

void QtFirebaseRemoteConfig::init()
{
    if(!qFirebase->ready()) {
        qDebug() << self << "::init" << "base not ready";
        return;
    }

    if(!_ready && !_initializing) {
        _initializing = true;

        remote_config::Initialize(*qFirebase->firebaseApp());
        qDebug() << self << "::init" << "native initialized";
        _initializing = false;
        setReady(true);
    }
}

void QtFirebaseRemoteConfig::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    qDebug()<<"QtFirebaseRemoteConfig"<<"::onFutureEvent"<<eventId;
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    if(eventId == __QTFIREBASE_ID+".config.fetch")
    {
        if(future.status() != firebase::kFutureStatusComplete)
        {
            qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.Error() << future.ErrorMessage();
            _initializing = false;
            return;
        }
        qDebug() << this << "::onFutureEvent initialized";
        _initializing = false;

        bool activate_result = remote_config::ActivateFetched();

        qDebug()<<QString("ActivateFetched %1").arg(activate_result ? "succeeded" : "failed");

        const remote_config::ConfigInfo& info = remote_config::GetInfo();

        qDebug()<<QString("Info last_fetch_time_ms=%1 fetch_status=%2 failure_reason=%3")
                .arg(QString::number(info.fetch_time))
                .arg(info.last_fetch_status)
                .arg(info.last_fetch_failure_reason);

        ParametersMap parametersUpdated;
        for(ParametersMap::const_iterator it = _parameters.begin(); it!=_parameters.end();++it)
        {
            const QVariant& value = it.value();

            Q_ASSERT(value.type() == QVariant::Bool ||
                     value.type() == QVariant::LongLong ||
                     value.type() == QVariant::Double ||
                     value.type() == QVariant::String);

            if(value.type() == QVariant::Bool)
            {
                parametersUpdated[it.key()] = remote_config::GetBoolean(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::LongLong)
            {
                parametersUpdated[it.key()] = remote_config::GetLong(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::Double)
            {
                parametersUpdated[it.key()] = remote_config::GetDouble(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::String)
            {
                //Cause crash
                //std::string result = remote_config::GetString(it.key().toUtf8().constData());
                //parametersUpdated[it.key()] = QString(result.c_str());

                std::vector<unsigned char> out = remote_config::GetData(it.key().toUtf8().constData());
                QByteArray data;
                for (size_t i = 0; i < out.size(); ++i)
                {
                    data.append(out[i]);
                }
                parametersUpdated[it.key()] = QString(data);
            }
        }


        _parameters = parametersUpdated;
        //SDK code for data (char array) container
        /*{
          std::vector<unsigned char> result = remote_config::GetData("TestData");
          for (size_t i = 0; i < result.size(); ++i) {
            const unsigned char value = result[i];
            printf("TestData[%d] = 0x%02x", i, value);
          }
        }*/

        //SDK code to print out the keys
        /*std::vector<std::string> keys = remote_config::GetKeys();
        qDebug()<<"QtFirebaseRemoteConfig GetKeys:";
        for (auto s = keys.begin(); s != keys.end(); ++s)
        {
            qDebug()<<s->c_str();
        }
        keys = remote_config::GetKeysByPrefix("TestD");
        printf("GetKeysByPrefix(\"TestD\"):");
        for (auto s = keys.begin(); s != keys.end(); ++s) {
          printf("  %s", s->c_str());
        }*/

        future.Release();

        if(info.last_fetch_status == remote_config::kLastFetchStatusSuccess)
        {
            setLoaded(true);
        }
        else
        {
            if(info.last_fetch_failure_reason == remote_config::kFetchFailureReasonInvalid)
            {
                emit error(kFetchFailureReasonInvalid, "The fetch has not yet failed.");
            }
            else if(info.last_fetch_failure_reason == remote_config::kFetchFailureReasonThrottled)
            {
                emit error(kFetchFailureReasonThrottled, "Throttled by the server. You are sending too many fetch requests in too short a time.");
            }
            else
            {
                emit error(kFetchFailureReasonError, "Failure reason is unknown");
            }
        }
    }
}

void QtFirebaseRemoteConfig::requestConfig(long long cacheExpirationInSeconds)
{
    _loaded = false;
    if(_parameters.size() == 0)
        return;

    qDebug()<<"QtFirebaseRemoteConfig::requestConfig with expirationtime"<<cacheExpirationInSeconds<<"sec";

    std::unique_ptr<remote_config::ConfigKeyValueVariant[]> defaults(
                new remote_config::ConfigKeyValueVariant[_parameters.size()]);


    uint cnt = 0;
    for(ParametersMap::const_iterator it = _parameters.begin(); it!=_parameters.end();++it)
    {
        const QVariant& value = it.value();

        Q_ASSERT(value.type() == QVariant::Bool ||
                 value.type() == QVariant::LongLong ||
                 value.type() == QVariant::Double ||
                 value.type() == QVariant::String);


        if(value.type() == QVariant::Bool)
        {
            defaults[cnt] = remote_config::ConfigKeyValueVariant{it.key().toUtf8().constData(),
                                                                 value.toBool()};
        }
        else if(value.type() == QVariant::LongLong)
        {
            defaults[cnt] = remote_config::ConfigKeyValueVariant{it.key().toUtf8().constData(),
                                                                 value.toLongLong()};
        }
        else if(value.type() == QVariant::Double)
        {
            defaults[cnt] = remote_config::ConfigKeyValueVariant{it.key().toUtf8().constData(),
                                                                 value.toDouble()};
        }

        else if(value.type() == QVariant::String)
        {
            //Strings cause crash on extracting from fetch
            //So workaround in by using data set type

            /*defaults[cnt] = remote_config::ConfigKeyValueVariant{it.key().toLatin1().constData(),
                                                                 value.toString().toLatin1().constData()};*/

            QByteArray data = value.toString().toUtf8();
            defaults[cnt] = remote_config::ConfigKeyValueVariant{
                                it.key().toUtf8().constData(),
                                firebase::Variant::FromMutableBlob(data.constData(), data.size())};
        }
        cnt++;
    }
    remote_config::SetDefaults(defaults.get(), _parameters.size());

    //Cause crash
    /*remote_config::SetConfigSetting(remote_config::kConfigSettingDeveloperMode, "1");
    if ((*remote_config::GetConfigSetting(remote_config::kConfigSettingDeveloperMode)
                .c_str()) != '1') {
        qDebug()<<"Failed to enable developer mode";
    }*/

    qDebug() << self << "::requestConfig" << "start fetching...";
    auto future = remote_config::Fetch(cacheExpirationInSeconds);
    qFirebase->addFuture(__QTFIREBASE_ID + ".config.fetch",future);
}

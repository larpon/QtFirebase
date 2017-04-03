#include "qtfirebaseremoteconfig.h"
#include <memory>
namespace remote_config = ::firebase::remote_config;

QtFirebaseRemoteConfig *QtFirebaseRemoteConfig::self = 0;

QtFirebaseRemoteConfig::QtFirebaseRemoteConfig(QObject *parent) :
    QObject(parent),
    _ready(false),
    _initializing(false),
    _cacheExpirationTime(firebase::remote_config::kDefaultCacheExpiration*1000),//milliseconds
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
    qDebug() << this << "::setReady" << ready;
    if (_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

QVariantMap QtFirebaseRemoteConfig::parameters() const
{
    return _parameters;
}

void QtFirebaseRemoteConfig::setParameters(const QVariantMap &map)
{
    _parameters = map;
    emit parametersChanged();
}

long long QtFirebaseRemoteConfig::cacheExpirationTime() const
{
    return _cacheExpirationTime;
}

void QtFirebaseRemoteConfig::setCacheExpirationTime(long long timeMs)
{
    _cacheExpirationTime = timeMs;
    emit cacheExpirationTimeChanged();
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
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    qDebug()<<self<<"::onFutureEvent"<<eventId;
    if(eventId != __QTFIREBASE_ID+".config.fetch")
        return;

    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        _initializing = false;
        return;
    }
    qDebug() << this << "::onFutureEvent initialized ok";
    _initializing = false;

    bool fetchActivated = remote_config::ActivateFetched();
    //On first run even if we have activateResult failed we still can get cached values
    qDebug()<<self<<QString("ActivateFetched %1").arg(fetchActivated ? "succeeded" : "failed");

    const remote_config::ConfigInfo& info = remote_config::GetInfo();

    qDebug()<<self<<QString("Info last_fetch_time_ms=%1 fetch_status=%2 failure_reason=%3")
            .arg(QString::number(info.fetch_time))
            .arg(info.last_fetch_status)
            .arg(info.last_fetch_failure_reason);

    if(info.last_fetch_status == remote_config::kLastFetchStatusSuccess)
    {
        QVariantMap updatedParameters;
        for(QVariantMap::const_iterator it = _parameters.begin(); it!=_parameters.end();++it)
        {
            const QVariant& value = it.value();

            Q_ASSERT(value.type() == QVariant::Bool ||
                     value.type() == QVariant::LongLong ||
                     value.type() == QVariant::Int ||
                     value.type() == QVariant::Double ||
                     value.type() == QVariant::String);

            if(value.type() == QVariant::Bool)
            {
                updatedParameters[it.key()] = remote_config::GetBoolean(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::LongLong)
            {
                updatedParameters[it.key()] = remote_config::GetLong(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::Int)
            {
                updatedParameters[it.key()] = remote_config::GetLong(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::Double)
            {
                updatedParameters[it.key()] = remote_config::GetDouble(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::String)
            {
                std::string result = remote_config::GetString(it.key().toUtf8().constData());
                updatedParameters[it.key()] = QString(result.c_str());

                //Code for data type
                /*std::vector<unsigned char> out = remote_config::GetData(it.key().toUtf8().constData());
                QByteArray data;
                for (size_t i = 0; i < out.size(); ++i)
                {
                    data.append(out[i]);
                }
                updatedParameters[it.key()] = QString(data);*/
            }
        }

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

        setParameters(updatedParameters);
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
    future.Release();
}

void QtFirebaseRemoteConfig::fetch()
{
    fetch(_cacheExpirationTime<1000 ? 0 : _cacheExpirationTime/1000);
}

void QtFirebaseRemoteConfig::fetch(long long cacheExpirationInSeconds)
{
    if(_parameters.size() == 0)
    {
        qDebug()<<self<<"::fetch not started, parameters were not initialized";
        return;
    }
    qDebug()<<self<<"::fetch with expirationtime"<<cacheExpirationInSeconds<<"seconds";

    QVariantMap filteredMap;
    for(QVariantMap::const_iterator it = _parameters.begin(); it!=_parameters.end();++it)
    {
        const QVariant& value = it.value();
        if(value.type() == QVariant::Bool ||
                value.type() == QVariant::LongLong ||
                value.type() == QVariant::Int ||
                value.type() == QVariant::Double ||
                value.type() == QVariant::String)
        {
            filteredMap[it.key()] = value;
        }
        else
        {
            qWarning()<<self<<"Data type:"<<value.typeName()<<" not supported";
        }
    }

    std::unique_ptr<remote_config::ConfigKeyValueVariant[]> defaults(
                new remote_config::ConfigKeyValueVariant[filteredMap.size()]);

    uint cnt = 0;
    for(QVariantMap::const_iterator it = filteredMap.begin(); it!=filteredMap.end();++it)
    {
        const QVariant& value = it.value();

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
        else if(value.type() == QVariant::Int)
        {
            defaults[cnt] = remote_config::ConfigKeyValueVariant{it.key().toUtf8().constData(),
                                                                 value.toInt()};
        }
        else if(value.type() == QVariant::Double)
        {
            defaults[cnt] = remote_config::ConfigKeyValueVariant{it.key().toUtf8().constData(),
                                                                 value.toDouble()};
        }

        else if(value.type() == QVariant::String)
        {
            defaults[cnt] = remote_config::ConfigKeyValueVariant{it.key().toLatin1().constData(),
                                                                 value.toString().toUtf8().constData()};

            //Code for data type
            /*QByteArray data = value.toString().toUtf8();
            defaults[cnt] = remote_config::ConfigKeyValueVariant{
                                it.key().toUtf8().constData(),
                                firebase::Variant::FromMutableBlob(data.constData(), data.size())};*/
        }
        cnt++;
    }
    remote_config::SetDefaults(defaults.get(), filteredMap.size());

    /*remote_config::SetConfigSetting(remote_config::kConfigSettingDeveloperMode, "1");
    if ((*remote_config::GetConfigSetting(remote_config::kConfigSettingDeveloperMode)
                .c_str()) != '1') {
        qDebug()<<"Failed to enable developer mode";
    }*/

    qDebug() << self << "::fetch" << "run fetching...";
    auto future = remote_config::Fetch(cacheExpirationInSeconds);
    qFirebase->addFuture(__QTFIREBASE_ID + ".config.fetch",future);
}

void QtFirebaseRemoteConfig::fetchNow()
{
    fetch(0);
}

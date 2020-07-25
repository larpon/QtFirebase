#include "qtfirebaseremoteconfig.h"

namespace remote_config = ::firebase::remote_config;

QtFirebaseRemoteConfig *QtFirebaseRemoteConfig::self = nullptr;

QtFirebaseRemoteConfig::QtFirebaseRemoteConfig(QObject *parent)
    : QObject(parent)
    , __QTFIREBASE_ID(QString().asprintf("%8p", static_cast<void*> (this)))
    , _ready(false)
    , _initializing(false)
    , _parameters()
    , _cacheExpirationTime(firebase::remote_config::kDefaultCacheExpiration*1000) // milliseconds
    , _appId()
    , __appIdByteArray()
    , __defaultsByteArrayList()
    , __appId(nullptr)
{
    if(self == nullptr)
    {
        self = this;
        qDebug() << self << "::QtFirebaseRemoteConfig" << "singleton";
    }

    #if defined(Q_OS_ANDROID)
    if (GooglePlayServices::available()) {
        qDebug() << this << " Google Play Services is available, now init remote_config" ;

        //Call init outside of constructor, otherwise signal readyChanged not emited
        QTimer::singleShot(500, this, SLOT(delayedInit()));
        connect(qFirebase,&QtFirebase::futureEvent, this, &QtFirebaseRemoteConfig::onFutureEvent);
    } else {
        qDebug() << this << " Google Play Services is NOT available, CANNOT use remote_config" ;
    }
    #else
    //Call init outside of constructor, otherwise signal readyChanged not emited
    QTimer::singleShot(500, this, SLOT(delayedInit()));
    connect(qFirebase,&QtFirebase::futureEvent, this, &QtFirebaseRemoteConfig::onFutureEvent);
    #endif
}

QtFirebaseRemoteConfig::~QtFirebaseRemoteConfig() {
    if(_ready)
        remote_config::Terminate();
}


bool QtFirebaseRemoteConfig::checkInstance(const char *function)
{
    bool b = (QtFirebaseRemoteConfig::self != nullptr);
    if(!b) qWarning("QtFirebaseRemoteConfig::%s:", function);
    return b;
}

void QtFirebaseRemoteConfig::addParameterInternal(const QString &name, const QVariant &defaultValue)
{
    _parameters[name] = defaultValue;
}

void QtFirebaseRemoteConfig::delayedInit()
{
    if(qFirebase->ready())
    {
        qDebug() << this << "::delayedInit : QtFirebase is ready, calling init" ;
        init();
    }
    else
    {
        qDebug() << this << "::delayedInit : QtFirebase not ready, connecting to its readyChanged signal" ;
        connect(qFirebase,&QtFirebase::readyChanged, this, &QtFirebaseRemoteConfig::init);
        qFirebase->requestInit();
    }
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
    qDebug() << this << "::setReady before:" << _ready << "now:" << ready;
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

quint64 QtFirebaseRemoteConfig::cacheExpirationTime() const
{
    return _cacheExpirationTime;
}

void QtFirebaseRemoteConfig::setCacheExpirationTime(quint64 timeMs)
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
    qDebug() << this << "::init" << "called";
    if(!qFirebase->ready()) {
        qDebug() << this << "::init" << "base not ready";
        return;
    }

    if(!_ready && !_initializing) {
        _initializing = true;

        auto future = _initializer.Initialize(qFirebase->firebaseApp(), nullptr, [](::firebase::App* app, void*) {
            // NOTE only write debug output here when developing
            // Causes crash on re-initialization (probably the "self" reference. And "this" can't be used in a lambda)
            //qDebug() << self << "::init" << "try to initialize Remote Config";
            return ::firebase::remote_config::Initialize(*app);
        });

        qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.init"), future);
    }
}

void QtFirebaseRemoteConfig::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    qDebug() << this << "::onFutureEvent" << eventId;
    if(eventId == __QTFIREBASE_ID + QStringLiteral(".config.fetch"))
        onFutureEventFetch(future);
    else if( eventId == __QTFIREBASE_ID + QStringLiteral(".config.init") )
        onFutureEventInit(future);
}

void QtFirebaseRemoteConfig::onFutureEventInit(firebase::FutureBase &future)
{
    if (future.status() != firebase::kFutureStatusComplete) {
        qDebug() << this << "::onFutureEvent" << "initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        _initializing = false;
        return;
    }

    qDebug() << this << "::onFutureEvent initialized ok";
    _initializing = false;
    setReady(true);
    future.Release();
}

void QtFirebaseRemoteConfig::onFutureEventFetch(firebase::FutureBase &future)
{
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
    qDebug() << this << QString(QStringLiteral("ActivateFetched %1")).arg(fetchActivated ? QStringLiteral("succeeded") : QStringLiteral("failed"));

    const remote_config::ConfigInfo& info = remote_config::GetInfo();

    qDebug() << this << QString(QStringLiteral("Info last_fetch_time_ms=%1 fetch_status=%2 failure_reason=%3"))
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
                updatedParameters[it.key()] = static_cast<qint64> ( remote_config::GetLong(it.key().toUtf8().constData()) );
            }
            else if(value.type() == QVariant::Int)
            {
                updatedParameters[it.key()] = static_cast<qint64> ( remote_config::GetLong(it.key().toUtf8().constData()) );
            }
            else if(value.type() == QVariant::Double)
            {
                updatedParameters[it.key()] = remote_config::GetDouble(it.key().toUtf8().constData());
            }
            else if(value.type() == QVariant::String)
            {
                std::string result = remote_config::GetString(it.key().toUtf8().constData());
                updatedParameters[it.key()] = QString(QString::fromUtf8(result.c_str()));

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
        qDebug() << "QtFirebaseRemoteConfig GetKeys:";
        for (auto s = keys.begin(); s != keys.end(); ++s)
        {
            qDebug() << s->c_str();
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
            emit error(FetchFailureReasonInvalid, QStringLiteral("The fetch has not yet failed."));
        }
        else if(info.last_fetch_failure_reason == remote_config::kFetchFailureReasonThrottled)
        {
            emit error(FetchFailureReasonThrottled, QStringLiteral("Throttled by the server. You are sending too many fetch requests in too short a time."));
        }
        else
        {
            emit error(FetchFailureReasonError, QStringLiteral("Failure reason is unknown"));
        }
    }
    future.Release();
}

void QtFirebaseRemoteConfig::fetch()
{
    fetch(_cacheExpirationTime<1000 ? 0 : _cacheExpirationTime/1000);
}

void QtFirebaseRemoteConfig::fetch(quint64 cacheExpirationInSeconds)
{
    if(_parameters.size() == 0)
    {
        qDebug() << this << "::fetch not started, parameters were not initialized";
        return;
    }
    qDebug() << this <<"::fetch with expirationtime" << cacheExpirationInSeconds << "seconds";

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
            qWarning() << this << "Data type:" << value.typeName() << " not supported";
        }
    }

    // From <memory> include
    std::unique_ptr<remote_config::ConfigKeyValueVariant[]> defaults(
                new remote_config::ConfigKeyValueVariant[filteredMap.size()]);

    __defaultsByteArrayList.clear();
    size_t index = 0;
    for(QVariantMap::const_iterator it = filteredMap.begin(); it!=filteredMap.end();++it)
    {
        __defaultsByteArrayList.insert(static_cast<int> (index),
                                       QByteArray(it.key().toUtf8().data()));
        const char* key = __defaultsByteArrayList.at(static_cast<int> (index)).constData();
        const QVariant& value = it.value();

        if(value.type() == QVariant::Bool)
        {
            defaults[index] = remote_config::ConfigKeyValueVariant{key, value.toBool()};
        }
        else if(value.type() == QVariant::LongLong)
        {
            defaults[index] = remote_config::ConfigKeyValueVariant{key, static_cast<int64_t> (value.toLongLong() ) };
        }
        else if(value.type() == QVariant::Int)
        {
            defaults[index] = remote_config::ConfigKeyValueVariant{key, value.toInt()};
        }
        else if(value.type() == QVariant::Double)
        {
            defaults[index] = remote_config::ConfigKeyValueVariant{key, value.toDouble()};
        }

        else if(value.type() == QVariant::String)
        {
            defaults[index] = remote_config::ConfigKeyValueVariant{key,
                    value.toString().toUtf8().constData()};

            //Code for data type
            /*QByteArray data = value.toString().toUtf8();
            defaults[cnt] = remote_config::ConfigKeyValueVariant{
                                it.key().toUtf8().constData(),
                                firebase::Variant::FromMutableBlob(data.constData(), data.size())};*/
        }

        index++;
    }

    remote_config::SetDefaults(defaults.get(), static_cast<size_t> (filteredMap.size()));

    /*remote_config::SetConfigSetting(remote_config::kConfigSettingDeveloperMode, "1");
    if ((*remote_config::GetConfigSetting(remote_config::kConfigSettingDeveloperMode)
                .c_str()) != '1') {
        qDebug() << "Failed to enable developer mode";
    }*/

    qDebug() << this << "::fetch" << "run fetching...";
    auto future = remote_config::Fetch(cacheExpirationInSeconds);
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.fetch"), future);
}

void QtFirebaseRemoteConfig::fetchNow()
{
    fetch(0);
}

#include "qtfirebaseremoteconfig.h"

#include <memory>

namespace remote_config = ::firebase::remote_config;

QtFirebaseRemoteConfig *QtFirebaseRemoteConfig::self { nullptr };

QtFirebaseRemoteConfig *QtFirebaseRemoteConfig::instance(QObject *parent)
{
    if (!self)
        self = new QtFirebaseRemoteConfig(parent);
    return self;
}

QtFirebaseRemoteConfig::QtFirebaseRemoteConfig(QObject *parent)
    : QObject(parent)
    , __QTFIREBASE_ID(QString().asprintf("%8p", static_cast<void *>(this)))
{
    // deny multiple instances
    Q_ASSERT(!self);
    if (self)
        return;
    self = this;

#ifdef Q_OS_ANDROID
    if (!GooglePlayServices::available())
        return;
#endif
    connect(qFirebase, &QtFirebase::futureEvent, this, &QtFirebaseRemoteConfig::onFutureEvent);
    QTimer::singleShot(0, this, [ this ] {
        if (qFirebase->ready()) {
            init();
            return;
        }
        connect(qFirebase, &QtFirebase::readyChanged, this, &QtFirebaseRemoteConfig::init);
        qFirebase->requestInit();
    });
}

QtFirebaseRemoteConfig::~QtFirebaseRemoteConfig()
{
#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    if (_ready)
        remote_config::Terminate();
#endif

    // check this instance is legal
    if (self == this)
        self = nullptr;
}

void QtFirebaseRemoteConfig::init()
{
    if (!qFirebase->ready() || _ready || _initializing)
        return;
    _initializing = true;

    const auto future = _initializer.Initialize(qFirebase->firebaseApp(), nullptr, [ ](::firebase::App *app, void *) {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
        ::firebase::remote_config::RemoteConfig::GetInstance(app)->EnsureInitialized();
        // TODO: No init_result_out parameter in ::firebase::remote_config::RemoteConfig::GetInstance() yet
        return ::firebase::kInitResultSuccess;
#else
        return ::firebase::remote_config::Initialize(*app);
#endif
    });

    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.init"), future);
}

void QtFirebaseRemoteConfig::onFutureEventInit(const firebase::FutureBase &future)
{
    const auto status = future.status();

    const bool ready = (status == firebase::kFutureStatusComplete);
    if (!ready)
        qWarning().noquote() << Q_FUNC_INFO << "future status" << status;

    _initializing = false;
    setReady(ready);
}

void QtFirebaseRemoteConfig::setReady(bool ready)
{
    if (_ready == ready)
        return;
    _ready = ready;
    emit readyChanged();
}

void QtFirebaseRemoteConfig::setParameters(const QVariantMap &parameters)
{
    if (_parameters == parameters)
        return;
    _parameters = parameters;
    emit parametersChanged();
}

void QtFirebaseRemoteConfig::setCacheExpirationTime(quint64 ms)
{
    if (_cacheExpirationTime == ms)
        return;
    _cacheExpirationTime = ms;
    emit cacheExpirationTimeChanged();
}

void QtFirebaseRemoteConfig::onFutureEvent(const QString &eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    qDebug() << this << "::onFutureEvent" << eventId;
    if(eventId == __QTFIREBASE_ID + QStringLiteral(".config.fetch"))
        onFutureEventFetch(future);
    else if( eventId == __QTFIREBASE_ID + QStringLiteral(".config.init") )
        onFutureEventInit(future);
    future.Release();
}

void QtFirebaseRemoteConfig::onFutureEventFetch(const firebase::FutureBase &future)
{
    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        _initializing = false;
        return;
    }
    qDebug() << this << "::onFutureEvent initialized ok";
    _initializing = false;

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto remoteConfigInstance = remote_config::RemoteConfig::GetInstance(qFirebase->firebaseApp());
#endif
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto activateFuture = remoteConfigInstance->Activate();

    qFirebase->waitForFutureCompletion(activateFuture);

    bool fetchActivated = false;
    if (activateFuture.result())
        fetchActivated = *activateFuture.result();
#else
    bool fetchActivated = remote_config::ActivateFetched();
#endif
    //On first run even if we have activateResult failed we still can get cached values
    qDebug() << this << QString(QStringLiteral("ActivateFetched %1")).arg(fetchActivated ? QStringLiteral("succeeded") : QStringLiteral("failed"));

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    const remote_config::ConfigInfo& info = remoteConfigInstance->GetInfo();
#else
    const remote_config::ConfigInfo& info = remote_config::GetInfo();
#endif

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
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
                updatedParameters[it.key()] = remoteConfigInstance->GetBoolean(it.key().toUtf8().constData());
#else
                updatedParameters[it.key()] = remote_config::GetBoolean(it.key().toUtf8().constData());
#endif
            }
            else if(value.type() == QVariant::LongLong)
            {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
                updatedParameters[it.key()] = static_cast<qint64> ( remoteConfigInstance->GetLong(it.key().toUtf8().constData()) );
#else
                updatedParameters[it.key()] = static_cast<qint64> ( remote_config::GetLong(it.key().toUtf8().constData()) );
#endif
            }
            else if(value.type() == QVariant::Int)
            {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
                updatedParameters[it.key()] = static_cast<qint64> ( remoteConfigInstance->GetLong(it.key().toUtf8().constData()) );
#else
                updatedParameters[it.key()] = static_cast<qint64> ( remote_config::GetLong(it.key().toUtf8().constData()) );
#endif
            }
            else if(value.type() == QVariant::Double)
            {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
                updatedParameters[it.key()] = remoteConfigInstance->GetDouble(it.key().toUtf8().constData());
#else
                updatedParameters[it.key()] = remote_config::GetDouble(it.key().toUtf8().constData());
#endif
            }
            else if(value.type() == QVariant::String)
            {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
                std::string result = remoteConfigInstance->GetString(it.key().toUtf8().constData());
#else
                std::string result = remote_config::GetString(it.key().toUtf8().constData());
#endif
                updatedParameters[it.key()] = QString(QString::fromUtf8(result.c_str()));
            }
        }

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
}

void QtFirebaseRemoteConfig::fetch(quint64 cacheExpirationInSeconds)
{
    if(_parameters.size() == 0)
    {
        qDebug() << this << "::fetch not started, parameters were not initialized";
        return;
    }
    qDebug() << this <<"::fetch with expirationtime" << cacheExpirationInSeconds << "seconds";

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto remoteConfigInstance = remote_config::RemoteConfig::GetInstance(qFirebase->firebaseApp());
#endif

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

    QVector<QByteArray> stringsData;

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
            // Store the string to ensure a valid data pointer
            stringsData += value.toString().toUtf8();

            defaults[index] = remote_config::ConfigKeyValueVariant{key, stringsData.last().constData()};
        }

        index++;
    }

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    remoteConfigInstance->SetDefaults(defaults.get(), static_cast<size_t> (filteredMap.size()));
#else
    remote_config::SetDefaults(defaults.get(), static_cast<size_t> (filteredMap.size()));
#endif

    qDebug() << this << "::fetch" << "run fetching...";
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto future = remoteConfigInstance->Fetch(cacheExpirationInSeconds);
#else
    auto future = remote_config::Fetch(cacheExpirationInSeconds);
#endif
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.fetch"), future);
}

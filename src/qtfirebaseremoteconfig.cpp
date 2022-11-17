#include "qtfirebaseremoteconfig.h"

#include <QSet>
#include <memory>

namespace remote_config = firebase::remote_config;

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
    if (!GooglePlayServices::available()) {
        QTimer::singleShot(0, this, &QtFirebaseRemoteConfig::googlePlayServicesError);
        return;
    }
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
    if (!qFirebase->ready() || _ready)
        return;

    const auto app = qFirebase->firebaseApp();

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    const auto future = firebase::remote_config::RemoteConfig::GetInstance(app)->EnsureInitialized();
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.init"), future);
#else
    const auto initResult = firebase::remote_config::Initialize(*app);
    if (initResult != firebase::kInitResultSuccess) {
        emit googlePlayServicesError();
        return;
    }
    setReady();
#endif
}

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseRemoteConfig::onFutureEventInit(const firebase::FutureBase &future)
{
    const auto futureStatus = future.status();
    const auto futureError = future.error();
    if (futureStatus != firebase::kFutureStatusComplete || futureError) {
        emit futuresError(futureError, future.error_message());
        return;
    }
    setReady();
}
#endif

void QtFirebaseRemoteConfig::onFutureEvent(const QString &eventId, firebase::FutureBase future)
{
    if (!eventId.startsWith(__QTFIREBASE_ID))
        return;

    if (eventId == __QTFIREBASE_ID + QStringLiteral(".config.fetch"))
        onFutureEventFetch(future);
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    else if (eventId == __QTFIREBASE_ID + QStringLiteral(".config.init"))
        onFutureEventInit(future);
#endif

    future.Release();
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

void QtFirebaseRemoteConfig::fetch(quint64 cacheExpirationInSeconds)
{
    if (!_ready)
        return;

    if (_parameters.isEmpty())
        return;

    QVariantMap filteredMap;
    for (auto it = _parameters.cbegin(); it != _parameters.cend(); ++it) {
        static QSet<QVariant::Type> types {
            QVariant::Bool,
            QVariant::Int,
            QVariant::LongLong,
            QVariant::Double,
            QVariant::String,
        };
        const auto &value = it.value();
        if (types.contains(value.type()))
            filteredMap[it.key()] = value;
    }

    QByteArrayList stringsData;

    std::unique_ptr<remote_config::ConfigKeyValueVariant[]> defaults(new remote_config::ConfigKeyValueVariant[filteredMap.size()]);
    size_t index = 0;

    __defaultsByteArrayList.clear();
    for (auto it = filteredMap.cbegin(); it != filteredMap.cend(); ++it) {
        __defaultsByteArrayList.insert(static_cast<int>(index), QByteArray(it.key().toUtf8()));
        const auto key = __defaultsByteArrayList.at(static_cast<int>(index)).constData();

        const auto &value = it.value();
        const auto type = value.type();

        switch (type) {
        case QVariant::Bool:
            defaults[index] = remote_config::ConfigKeyValueVariant { key, value.toBool() };
            break;
        case QVariant::Int:
            defaults[index] = remote_config::ConfigKeyValueVariant { key, value.toInt() };
            break;
        case QVariant::LongLong:
            defaults[index] = remote_config::ConfigKeyValueVariant { key, static_cast<int64_t>(value.toLongLong()) };
            break;
        case QVariant::Double:
            defaults[index] = remote_config::ConfigKeyValueVariant { key, value.toDouble() };
            break;
        case QVariant::String:
            stringsData += value.toString().toUtf8();
            defaults[index] = remote_config::ConfigKeyValueVariant { key, stringsData.last().constData() };
            break;
        default:
            break;
        }

        index++;
    }

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto instance = remote_config::RemoteConfig::GetInstance(qFirebase->firebaseApp());
    instance->SetDefaults(defaults.get(), static_cast<size_t>(filteredMap.size()));
    const auto future = instance->Fetch(cacheExpirationInSeconds);
#else
    remote_config::SetDefaults(defaults.get(), static_cast<size_t>(filteredMap.size()));
    const auto future = remote_config::Fetch(cacheExpirationInSeconds);
#endif

    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.fetch"), future);
}

void QtFirebaseRemoteConfig::onFutureEventFetch(const firebase::FutureBase &future)
{
    const auto futureStatus = future.status();
    const auto futureError = future.error();
    if (futureStatus != firebase::kFutureStatusComplete || futureError) {
        emit futuresError(futureError, future.error_message());
        return;
    }

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto remoteConfigInstance = remote_config::RemoteConfig::GetInstance(qFirebase->firebaseApp());
    const auto activateFuture = remoteConfigInstance->Activate();
    qFirebase->waitForFutureCompletion(activateFuture);

    const bool fetchActivated = activateFuture.result() ? *activateFuture.result() : false;
#else
    const bool fetchActivated = remote_config::ActivateFetched();
#endif
    Q_UNUSED(fetchActivated)

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    const remote_config::ConfigInfo &info = remoteConfigInstance->GetInfo();
#else
    const remote_config::ConfigInfo &info = remote_config::GetInfo();
#endif

    const auto lastFetchStatus = info.last_fetch_status;
    const auto lastFetchFailureReason = info.last_fetch_failure_reason;
    if (lastFetchStatus != remote_config::kLastFetchStatusSuccess) {
        if (lastFetchFailureReason == remote_config::kFetchFailureReasonInvalid) {
            emit error(FetchFailureReasonInvalid, QStringLiteral("The fetch has not yet failed."));
            return;
        }
        if (lastFetchFailureReason == remote_config::kFetchFailureReasonThrottled) {
            emit error(FetchFailureReasonThrottled, QStringLiteral("Throttled by the server. You are sending too many fetch requests in too short a time."));
            return;
        }
        emit error(FetchFailureReasonError, QStringLiteral("Failure reason is unknown."));
        return;
    }

    QVariantMap updatedParameters;
    for (auto it = _parameters.cbegin(); it != _parameters.cend(); ++it) {
        static QSet<QVariant::Type> types {
            QVariant::Bool,
            QVariant::Int,
            QVariant::LongLong,
            QVariant::Double,
            QVariant::String,
        };
        const QVariant &value = it.value();
        const QString &key = it.key();

        const auto type = value.type();

        Q_ASSERT(types.contains(type));

        if (type == QVariant::Bool) {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
            updatedParameters[key] = remoteConfigInstance->GetBoolean(key.toUtf8().constData());
#else
            updatedParameters[key] = remote_config::GetBoolean(key.toUtf8().constData());
#endif
        } else if (type == QVariant::Int) {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
            updatedParameters[key] = static_cast<int>(remoteConfigInstance->GetLong(key.toUtf8().constData()));
#else
            updatedParameters[key] = static_cast<int>(remote_config::GetLong(key.toUtf8().constData()));
#endif
        } else if (type == QVariant::LongLong) {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
            updatedParameters[key] = static_cast<long long>(remoteConfigInstance->GetLong(key.toUtf8().constData()));
#else
            updatedParameters[key] = static_cast<long long>(remote_config::GetLong(key.toUtf8().constData()));
#endif
        } else if (type == QVariant::Double) {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
            updatedParameters[key] = remoteConfigInstance->GetDouble(key.toUtf8().constData());
#else
            updatedParameters[key] = remote_config::GetDouble(key.toUtf8().constData());
#endif
        } else if (type == QVariant::String) {
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
            const std::string result = remoteConfigInstance->GetString(key.toUtf8().constData());
#else
            const std::string result = remote_config::GetString(key.toUtf8().constData());
#endif
            updatedParameters[key] = QString::fromUtf8(result.c_str());
        }
    }

    setParameters(updatedParameters);
}

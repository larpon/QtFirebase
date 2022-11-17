#include "qtfirebaseremoteconfig.h"

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
    const auto future = remote_config::RemoteConfig::GetInstance(app)->EnsureInitialized();
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.init"), future);
#else
    const auto initResult = remote_config::Initialize(*app);
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

void QtFirebaseRemoteConfig::setFetching(bool fetching)
{
    if (_fetching == fetching)
        return;
    _fetching = fetching;
    emit fetchingChanged();
}

void QtFirebaseRemoteConfig::setParametersAndFetching(const QVariantMap &parameters, bool fetching)
{
    const bool emitFetching = (_fetching != fetching);

    const bool emitParameters = (_parameters != parameters);
    if (emitParameters)
        _parameters = parameters;
    _fetching = fetching;

    if (emitFetching)
        emit fetchingChanged();
    if (emitParameters)
        emit parametersChanged();
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
    if (!_ready || _fetching)
        return;

    if (_parameters.isEmpty())
        return;

    QVector<remote_config::ConfigKeyValueVariant> defaults;
    defaults.reserve(_parameters.size());

    QByteArrayList keysData;
    QByteArrayList stringsData;
    for (auto it = _parameters.cbegin(); it != _parameters.cend(); ++it) {
        keysData << it.key().toUtf8();

        const auto key = keysData.last().constData();
        const auto &value = it.value();

        const auto type = value.type();
        switch (type) {
        case QVariant::Bool:
            defaults << remote_config::ConfigKeyValueVariant { key, value.toBool() };
            break;
        case QVariant::Int:
            defaults << remote_config::ConfigKeyValueVariant { key, value.toInt() };
            break;
        case QVariant::LongLong:
            defaults << remote_config::ConfigKeyValueVariant { key, static_cast<int64_t>(value.toLongLong()) };
            break;
        case QVariant::Double:
            defaults << remote_config::ConfigKeyValueVariant { key, value.toDouble() };
            break;
        case QVariant::String:
            stringsData << value.toString().toUtf8();
            defaults << remote_config::ConfigKeyValueVariant { key, stringsData.last().constData() };
            break;
        default:
            break;
        }
    }

    setFetching();

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto instance = remote_config::RemoteConfig::GetInstance(qFirebase->firebaseApp());
    instance->SetDefaults(defaults.constData(), static_cast<size_t>(defaults.length()));
    const auto future = instance->Fetch(cacheExpirationInSeconds);
#else
    remote_config::SetDefaults(defaults.constData(), static_cast<size_t>(defaults.length()));
    const auto future = remote_config::Fetch(cacheExpirationInSeconds);
#endif

    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.fetch"), future);
}

void QtFirebaseRemoteConfig::onFutureEventFetch(const firebase::FutureBase &future)
{
    const auto futureStatus = future.status();
    const auto futureError = future.error();
    if (futureStatus != firebase::kFutureStatusComplete || futureError) {
        setFetching(false);
        emit futuresError(futureError, future.error_message());
        return;
    }

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    auto instance = remote_config::RemoteConfig::GetInstance(qFirebase->firebaseApp());
    const auto activateFuture = instance->Activate();
    qFirebase->waitForFutureCompletion(activateFuture);

    const bool fetchActivated = activateFuture.result() ? *activateFuture.result() : false;
#else
    const bool fetchActivated = remote_config::ActivateFetched();
#endif
    Q_UNUSED(fetchActivated)

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    const remote_config::ConfigInfo &info = instance->GetInfo();
#else
    const remote_config::ConfigInfo &info = remote_config::GetInfo();
#endif

    const auto lastFetchStatus = info.last_fetch_status;
    const auto lastFetchFailureReason = info.last_fetch_failure_reason;
    if (lastFetchStatus != remote_config::kLastFetchStatusSuccess) {
        setFetching(false);
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
        const auto &key = it.key();
        const auto keyUtf8 = key.toUtf8();
        const auto keyStr = keyUtf8.constData();

        const auto type = it.value().type();
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
        switch (type) {
        case QVariant::Bool:
            updatedParameters[key] = instance->GetBoolean(keyStr);
            break;
        case QVariant::Int:
            updatedParameters[key] = static_cast<int>(instance->GetLong(keyStr));
            break;
        case QVariant::LongLong:
            updatedParameters[key] = static_cast<long long>(instance->GetLong(keyStr));
            break;
        case QVariant::Double:
            updatedParameters[key] = instance->GetDouble(keyStr);
            break;
        case QVariant::String:
            updatedParameters[key] = QString::fromStdString(instance->GetString(keyStr));
            break;
        default:
            break;
        }
#else
        switch (type) {
        case QVariant::Bool:
            updatedParameters[key] = remote_config::GetBoolean(keyStr);
            break;
        case QVariant::Int:
            updatedParameters[key] = static_cast<int>(remote_config::GetLong(keyStr));
            break;
        case QVariant::LongLong:
            updatedParameters[key] = static_cast<long long>(remote_config::GetLong(keyStr));
            break;
        case QVariant::Double:
            updatedParameters[key] = remote_config::GetDouble(keyStr);
            break;
        case QVariant::String:
            updatedParameters[key] = QString::fromStdString(remote_config::GetString(keyStr));
            break;
        default:
            break;
        }
#endif
    }

    setParametersAndFetching(updatedParameters, false);
}

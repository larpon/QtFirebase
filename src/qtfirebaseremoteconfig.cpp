#include "qtfirebaseremoteconfig.h"

namespace remote_config = firebase::remote_config;

QtFirebaseRemoteConfig *QtFirebaseRemoteConfig::self { nullptr };

QtFirebaseRemoteConfig::QtFirebaseRemoteConfig(QObject *parent)
    : QObject(parent)
    , __QTFIREBASE_ID(QString().asprintf("%8p", static_cast<void *>(this)))
{
    // deny multiple instances
    Q_ASSERT(!self);
    if (self)
        return;
    self = this;

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
    _rc = remote_config::RemoteConfig::GetInstance(app);
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.init"), _rc->EnsureInitialized());
#else
    const auto result = remote_config::Initialize(*app);
    if (result != firebase::kInitResultSuccess) {
        emit googlePlayServicesError();
        return;
    }
    setReady();
#endif
}

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseRemoteConfig::onFutureEventInit()
{
    setReady();
}
#endif

void QtFirebaseRemoteConfig::onFutureEvent(const QString &eventId, firebase::FutureBase future)
{
    if (!eventId.startsWith(__QTFIREBASE_ID))
        return;

    const auto status = future.status();
    Q_ASSERT(status == firebase::kFutureStatusComplete);
    if (status != firebase::kFutureStatusComplete)
        return;

    const auto error = future.error();
    const auto message = QString::fromUtf8(future.error_message());
    future.Release();
    if (error) {
        setFetching(false);
        emit futuresError(error, message);
        return;
    }

    if (eventId == __QTFIREBASE_ID + QStringLiteral(".config.fetch"))
        return onFutureEventFetch();
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    else if (eventId == __QTFIREBASE_ID + QStringLiteral(".config.activate"))
        return onFutureEventActivate();
    else if (eventId == __QTFIREBASE_ID + QStringLiteral(".config.defaults"))
        return onFutureEventDefaults();
    else if (eventId == __QTFIREBASE_ID + QStringLiteral(".config.init"))
        return onFutureEventInit();
#endif
    Q_ASSERT(false);
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

void QtFirebaseRemoteConfig::fetch(quint64 cacheExpirationInSeconds)
{
    if (!_ready || _fetching || _parameters.isEmpty())
        return;

    QVector<remote_config::ConfigKeyValueVariant> defaults;
    defaults.reserve(_parameters.size());

    QByteArrayList stringsData;
    QByteArrayList keysData;
    for (auto it = _parameters.cbegin(); it != _parameters.cend(); ++it) {
        keysData << it.key().toUtf8();

        const auto keyStr = keysData.last().constData();

        const auto &value = it.value();
        const auto type = value.type();
        switch (type) {
        case QVariant::Bool:
            defaults << remote_config::ConfigKeyValueVariant { keyStr, value.toBool() };
            break;
        case QVariant::Int:
            defaults << remote_config::ConfigKeyValueVariant { keyStr, value.toInt() };
            break;
        case QVariant::LongLong:
            defaults << remote_config::ConfigKeyValueVariant { keyStr, static_cast<int64_t>(value.toLongLong()) };
            break;
        case QVariant::Double:
            defaults << remote_config::ConfigKeyValueVariant { keyStr, value.toDouble() };
            break;
        case QVariant::String:
            stringsData << value.toString().toUtf8();
            defaults << remote_config::ConfigKeyValueVariant { keyStr, stringsData.last().constData() };
            break;
        default:
            break;
        }
    }

    if (defaults.isEmpty())
        return;
    const auto data = defaults.constData();
    const auto n = static_cast<size_t>(defaults.length());

    setFetching();

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    _cacheExpirationInSeconds = cacheExpirationInSeconds;
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.defaults"), _rc->SetDefaults(data, n));
#else
    remote_config::SetDefaults(data, n);
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.fetch"), remote_config::Fetch(cacheExpirationInSeconds));
#endif
}

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseRemoteConfig::onFutureEventDefaults()
{
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.fetch"), _rc->Fetch(_cacheExpirationInSeconds));
}
#endif

void QtFirebaseRemoteConfig::onFutureEventFetch()
{
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".config.activate"), _rc->Activate());
#else
    remote_config::ActivateFetched();
    updateParameters();
#endif
}

#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseRemoteConfig::onFutureEventActivate()
{
    updateParameters();
}
#endif

void QtFirebaseRemoteConfig::updateParameters()
{
#if QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    const auto info = _rc->GetInfo();
#else
    const auto info = remote_config::GetInfo();
#endif

    const auto lastFetchStatus = info.last_fetch_status;
    if (lastFetchStatus != remote_config::kLastFetchStatusSuccess) {
        setFetching(false);

        const auto lastFetchFailureReason = info.last_fetch_failure_reason;
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
            updatedParameters[key] = _rc->GetBoolean(keyStr);
            break;
        case QVariant::Int:
            updatedParameters[key] = static_cast<int>(_rc->GetLong(keyStr));
            break;
        case QVariant::LongLong:
            updatedParameters[key] = static_cast<long long>(_rc->GetLong(keyStr));
            break;
        case QVariant::Double:
            updatedParameters[key] = _rc->GetDouble(keyStr);
            break;
        case QVariant::String:
            updatedParameters[key] = QString::fromStdString(_rc->GetString(keyStr));
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

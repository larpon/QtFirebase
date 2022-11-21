#include "qtfirebaseanalytics.h"

#include <firebase/analytics.h>

#include <QVector>
#include <QDebug>

const int USER_ID_MAX_LEN { 256 };

namespace analytics = firebase::analytics;

QtFirebaseAnalytics *QtFirebaseAnalytics::self { nullptr };

QtFirebaseAnalytics::QtFirebaseAnalytics(QObject *parent)
    : QObject(parent)
{
    // deny multiple instances
    Q_ASSERT(!self);
    if (self)
        return;
    self = this;

    QTimer::singleShot(0, this, [ this ] {
        if (qFirebase->ready()) {
            init();
            return;
        }
        connect(qFirebase, &QtFirebase::readyChanged, this, &QtFirebaseAnalytics::init);
        qFirebase->requestInit();
    });
}

QtFirebaseAnalytics::~QtFirebaseAnalytics()
{
    if (_ready)
        analytics::Terminate();

    // check this instance is legal
    if (self == this)
        self = nullptr;
}

void QtFirebaseAnalytics::init()
{
    if (!qFirebase->ready() || _ready)
        return;

    const auto app = qFirebase->firebaseApp();

    analytics::Initialize(*app);
    analytics::SetSessionTimeoutDuration(_sessionTimeout);

    setReady();
}

void QtFirebaseAnalytics::setReady(bool ready)
{
    if (_ready == ready)
        return;
    _ready = ready;
    emit readyChanged();
}

void QtFirebaseAnalytics::setEnabled(bool enabled)
{
    if (!_ready) {
        qDebug() << this << "::setEnabled native part not ready";
        return;
    }

    if (_enabled == enabled)
        return;

    analytics::SetAnalyticsCollectionEnabled(enabled);

    _enabled = enabled;
    qDebug() << this << "::setEnabled" << enabled;
    emit enabledChanged();
}

void QtFirebaseAnalytics::setMinimumSessionDuration(uint minimumSessionDuration)
{
    qWarning() << this << "::setMinimumSessionDuration is deprecated and no longer functional";
    if (!_ready) {
        qDebug() << this << "::setMinimumSessionDuration native part not ready";
        return;
    }

    if (_minimumSessionDuration == minimumSessionDuration)
        return;

    _minimumSessionDuration = minimumSessionDuration;
    qDebug() << this << "::setMinimumSessionDuration" << minimumSessionDuration;
    emit minimumSessionDurationChanged();
}

void QtFirebaseAnalytics::setSessionTimeout(uint sessionTimeout)
{
    if (!_ready) {
        qDebug() << this << "::setSessionTimeout native part not ready";
        return;
    }

    if (_sessionTimeout == sessionTimeout)
        return;

    analytics::SetSessionTimeoutDuration(_sessionTimeout);

    _sessionTimeout = sessionTimeout;
    qDebug() << this << "::setSessionTimeout" << sessionTimeout;
    emit sessionTimeoutChanged();
}

void QtFirebaseAnalytics::setUserId(const QString &userId)
{
    if (!_ready) {
        qDebug() << this << "::setUserId native part not ready";
        return;
    }
    if (userId.isEmpty())
        return unsetUserId();

    auto aUserId = userId;
    if (aUserId.length() > USER_ID_MAX_LEN) {
        aUserId = aUserId.left(USER_ID_MAX_LEN);
        qWarning() << this << QStringLiteral("::setUserId ID longer than allowed %1 chars and truncated to").arg(USER_ID_MAX_LEN) << aUserId;
    }
    if (_userId == aUserId)
        return;

    analytics::SetUserId(_userId.toUtf8().constData());

    _userId = aUserId;
    qDebug() << this << "::setUserId" << _userId;
    emit userIdChanged();
}

void QtFirebaseAnalytics::setUserProperties(const QVariantList &userProperties)
{
    if (!_ready) {
        qDebug() << this << "::setUserProperties native part not ready";
        return;
    }

    if (_userProperties == userProperties)
        return;
    _userProperties = userProperties;

    if (_userProperties.size() > 25)
        qWarning() << this << "::setUserProperties size is more than 25";

    uint index = 0;
    for (auto it = _userProperties.cbegin(); it != _userProperties.cend(); ++it, ++index) {
        const bool ok = (*it).canConvert<QVariantMap>();
        if (!ok) {
            qWarning() << this << "::setUserProperties wrong entry at index" << index;
            continue;
        }
        const auto map = (*it).toMap();
        if (map.isEmpty())
            continue;
        const auto &first = map.first();
        if (first.canConvert<QString>()) {
            const auto key = map.firstKey();
            const auto value = first.toString();
            setUserProperty(key, value);
        }
    }
    emit userPropertiesChanged();
}

void QtFirebaseAnalytics::setUserProperty(const QString &name, const QString &value)
{
    if (!_ready) {
        qDebug() << this << "::setUserProperty native part not ready";
        return;
    }
    qDebug() << this << "::setUserProperty" << name << ":" << value;
    analytics::SetUserProperty(name.toUtf8().constData(), value.toUtf8().constData());
}

void QtFirebaseAnalytics::unsetUserId()
{
    if (!_ready) {
        qDebug() << this << "::unsetUserId native part not ready";
        return;
    }

    if (_userId.isEmpty())
        return;
    _userId.clear();
    analytics::SetUserId(nullptr);
    emit userIdChanged();
}

#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseAnalytics::setCurrentScreen(const QString &screenName, const QString &screenClass)
{
    qWarning() << this << "::setCurrentScreen is deprecated";
    if (!_ready) {
        qDebug() << this << "::setCurrentScreen native part not ready";
        return;
    }
    qDebug() << this << "::setCurrentScreen" << screenName << ":" << screenClass;
    analytics::SetCurrentScreen(screenName.toUtf8().constData(), screenClass.toUtf8().constData());
}
#endif

void QtFirebaseAnalytics::logEvent(const QString &name)
{
    if (!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }
    qDebug() << this << "::logEvent" << name << "(no params)";
    analytics::LogEvent(name.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &param, int value)
{
    if (!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }
    qDebug() << this << "::logEvent" << name << "int param" << param << ":" << value;
    analytics::LogEvent(name.toUtf8().constData(), param.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &param, double value)
{
    if (!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }
    qDebug() << this << "::logEvent" << name << "double param" << param << ":" << value;
    analytics::LogEvent(name.toUtf8().constData(), param.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &param, const QString &value)
{
    if (!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }
    qDebug() << this << "::logEvent" << name << "string param" << param << ":" << value;
    analytics::LogEvent(name.toUtf8().constData(), param.toUtf8().constData(), value.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QVariantMap &bundle)
{
    if (!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }
    qDebug().noquote() << this << "::logEvent bundle" << name;

    QVector<analytics::Parameter> parameters;
    parameters.reserve(bundle.size());

    QByteArrayList stringsData;
    QByteArrayList keysData;
    for (auto it = bundle.cbegin(); it != bundle.cend(); ++it) {
        keysData << it.key().toUtf8();

        const auto keyStr = keysData.last().constData();

        const auto &value = it.value();
        const auto type = value.type();
        switch (type) {
        case QVariant::Int: {
            const int intVal = value.toInt();
            parameters << analytics::Parameter(keyStr, intVal);
            qDebug() << this << "::logEvent bundle param" << keyStr << ":" << intVal;
            break;
        }
        case QVariant::Double: {
            const double doubleVal = value.toDouble();
            parameters << analytics::Parameter(keyStr, doubleVal);
            qDebug() << this << "::logEvent bundle param" << keyStr << ":" << doubleVal;
            break;
        }
        case QVariant::String: {
            stringsData << value.toString().toUtf8();
            const auto valueStr = stringsData.last().constData();
            parameters << analytics::Parameter(keyStr, valueStr);
            qDebug() << this << "::logEvent bundle param" << keyStr << ":" << valueStr;
            break;
        }
        default:
            parameters << analytics::Parameter("", "");
            qWarning() << this << "::logEvent bundle param" << keyStr << "with unsupported data type";
            break;
        }
    }

    analytics::LogEvent(name.toUtf8().constData(), parameters.constData(), static_cast<size_t>(parameters.length()));
}

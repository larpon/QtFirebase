#include "qtfirebaseanalytics.h"

#include <firebase/analytics.h>

#include <QVector>
#include <QDebug>

#define QTFIREBASE_ANALYTICS_CHECK_READY(name) \
    if (!_ready) { \
        qDebug().noquote() << this << name << "native part not ready"; \
        return; \
    }

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

    connect(this, &QtFirebaseAnalytics::readyChanged, this, &QtFirebaseAnalytics::processCache);

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

void QtFirebaseAnalytics::processCache()
{
    if (!_ready)
        return;

    for (const auto &f : qAsConst(_cache))
        f();
    _cache.clear();
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
    QTFIREBASE_ANALYTICS_CHECK_READY("::setEnabled")
    if (_enabled == enabled)
        return;
    qDebug() << this << "::setEnabled" << enabled;
    analytics::SetAnalyticsCollectionEnabled(enabled);
    _enabled = enabled;
    emit enabledChanged();
}

void QtFirebaseAnalytics::setMinimumSessionDuration(uint minimumSessionDuration)
{
    qWarning() << this << "::setMinimumSessionDuration is deprecated and no longer functional";
    QTFIREBASE_ANALYTICS_CHECK_READY("::setMinimumSessionDuration")
    if (_minimumSessionDuration == minimumSessionDuration)
        return;
    qDebug() << this << "::setMinimumSessionDuration" << minimumSessionDuration;
    _minimumSessionDuration = minimumSessionDuration;
    emit minimumSessionDurationChanged();
}

void QtFirebaseAnalytics::setSessionTimeout(uint ms)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::setSessionTimeout")
    if (_sessionTimeout == ms)
        return;
    qDebug() << this << "::setSessionTimeout" << ms;
    analytics::SetSessionTimeoutDuration(ms);
    _sessionTimeout = ms;
    emit sessionTimeoutChanged();
}

void QtFirebaseAnalytics::setUserId(const QString &userId)
{
    if (userId.isEmpty())
        return unsetUserId();

    QTFIREBASE_ANALYTICS_CHECK_READY("::setUserId")
    if (_userId == userId)
        return;
    qDebug() << this << "::setUserId" << userId;
    analytics::SetUserId(userId.toUtf8().constData());
    _userId = userId;
    emit userIdChanged();
}

void QtFirebaseAnalytics::unsetUserId()
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::unsetUserId")
    if (_userId.isEmpty())
        return;
    _userId = QString();
    qDebug() << this << "::setUserId nullptr";
    analytics::SetUserId(nullptr);
    emit userIdChanged();
}

void QtFirebaseAnalytics::setUserProperties(const QVariantList &userProperties)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::setUserProperties")
    if (_userProperties == userProperties)
        return;

    for (auto it = userProperties.cbegin(); it != userProperties.cend(); ++it) {
        const auto index = it - userProperties.cbegin();

        const bool ok = it->canConvert<QVariantMap>();
        if (!ok) {
            qWarning() << this << "::setUserProperties wrong entry at index" << index;
            continue;
        }

        const auto map = it->toMap();
        if (map.isEmpty()) {
            qWarning() << this << "::setUserProperties wrong entry at index" << index;
            continue;
        }

        const auto &first = map.first();
        if (!first.canConvert<QString>()) {
            qWarning() << this << "::setUserProperties wrong entry at index" << index;
            continue;
        }

        setUserProperty(map.firstKey(), first.toString());
    }
    _userProperties = userProperties;
    emit userPropertiesChanged();
}

void QtFirebaseAnalytics::setUserProperty(const QString &name, const QString &value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::setUserProperty")
    qDebug() << this << "::setUserProperty" << name << ":" << value;
    analytics::SetUserProperty(name.toUtf8().constData(), value.isEmpty() ? nullptr : value.toUtf8().constData());
}

#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseAnalytics::setCurrentScreen(const QString &screenName, const QString &screenClass)
{
    qWarning() << this << "::setCurrentScreen is deprecated";
    QTFIREBASE_ANALYTICS_CHECK_READY("::setCurrentScreen")
    qDebug() << this << "::setCurrentScreen" << screenName << ":" << screenClass;
    analytics::SetCurrentScreen(screenName.isEmpty() ? nullptr : screenName.toUtf8().constData(), screenClass.isEmpty() ? nullptr : screenClass.toUtf8().constData());
}
#endif

void QtFirebaseAnalytics::logEvent(const QString &event)
{
    if (!_ready)
        _cache << [ this, event ] { logEvent(event); };
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

    qDebug() << this << "::logEvent" << event << "with no params";
    analytics::LogEvent(event.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, int value)
{
    if (!_ready)
        _cache << [ this, event, param, value ] { logEvent(event, param, value); };
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

    qDebug() << this << "::logEvent" << event << "int param" << param << ":" << value;
    analytics::LogEvent(event.toUtf8().constData(), param.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, long long value)
{
    if (!_ready)
        _cache << [ this, event, param, value ] { logEvent(event, param, value); };
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

    qDebug() << this << "::logEvent" << event << "long long param" << param << ":" << value;
    analytics::LogEvent(event.toUtf8().constData(), param.toUtf8().constData(), static_cast<int64_t>(value));
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, double value)
{
    if (!_ready)
        _cache << [ this, event, param, value ] { logEvent(event, param, value); };
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

    qDebug() << this << "::logEvent" << event << "double param" << param << ":" << value;
    analytics::LogEvent(event.toUtf8().constData(), param.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, const QString &value)
{
    if (!_ready)
        _cache << [ this, event, param, value ] { logEvent(event, param, value); };
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

    qDebug() << this << "::logEvent" << event << "string param" << param << ":" << value;
    analytics::LogEvent(event.toUtf8().constData(), param.toUtf8().constData(), value.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QVariantMap &bundle)
{
    if (!_ready)
        _cache << [ this, event, bundle ] { logEvent(event, bundle); };
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

    qDebug().noquote() << this << "::logEvent bundle" << event;

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
            qDebug() << this << "::logEvent bundle int param" << keyStr << ":" << intVal;
            break;
        }
        case QVariant::LongLong: {
            const int longLongVal = value.toLongLong();

            parameters << analytics::Parameter(keyStr, static_cast<int64_t>(longLongVal));
            qDebug() << this << "::logEvent bundle long long param" << keyStr << ":" << longLongVal;
            break;
        }
        case QVariant::Double: {
            const double doubleVal = value.toDouble();

            parameters << analytics::Parameter(keyStr, doubleVal);
            qDebug() << this << "::logEvent bundle double param" << keyStr << ":" << doubleVal;
            break;
        }
        case QVariant::String: {
            const char *valueStr = nullptr;
            const auto valueString = value.toString();
            if (!valueString.isEmpty()) {
                stringsData << valueString.toUtf8();
                valueStr = stringsData.last().constData();
            }

            parameters << analytics::Parameter(keyStr, valueStr);
            qDebug() << this << "::logEvent bundle string param" << keyStr << ":" << valueStr;
            break;
        }
        default:
            qWarning() << this << "::logEvent bundle param" << keyStr << "with unsupported data type";
            break;
        }
    }

    analytics::LogEvent(event.toUtf8().constData(), parameters.constData(), static_cast<size_t>(parameters.length()));
}

#include "qtfirebaseanalytics.h"

#include <firebase/analytics.h>

#include <QVector>
#include <QDebug>

#include <algorithm>
#include <cctype>

#define QTFIREBASE_ANALYTICS_CHECK_READY(name) \
    if (!_ready) { \
        qDebug().noquote() << this << name << "native part not ready"; \
        return; \
    }

const int USER_ID_MAX_LEN { 256 };
const int USER_PROPS_LIST_MAX_LEN { 25 };
const int USER_PROP_NAME_MAX_LEN { 40 };
const int USER_PROP_VALUE_MAX_LEN { 100 };

#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
const int SCREEN_NAME_MAX_LEN { 100 };
const int SCREEN_CLASS_MAX_LEN { 100 };
#endif

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
    QTFIREBASE_ANALYTICS_CHECK_READY("::setEnabled")
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
    QTFIREBASE_ANALYTICS_CHECK_READY("::setMinimumSessionDuration")
    if (_minimumSessionDuration == minimumSessionDuration)
        return;

    _minimumSessionDuration = minimumSessionDuration;
    qDebug() << this << "::setMinimumSessionDuration" << minimumSessionDuration;
    emit minimumSessionDurationChanged();
}

void QtFirebaseAnalytics::setSessionTimeout(uint sessionTimeout)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::setSessionTimeout")
    if (_sessionTimeout == sessionTimeout)
        return;

    analytics::SetSessionTimeoutDuration(_sessionTimeout);

    _sessionTimeout = sessionTimeout;
    qDebug() << this << "::setSessionTimeout" << sessionTimeout;
    emit sessionTimeoutChanged();
}

void QtFirebaseAnalytics::setUserId(const QString &userId)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::setUserId")

    if (userId.isEmpty())
        return unsetUserId();

    auto aUserId = userId;
    if (aUserId.length() > USER_ID_MAX_LEN) {
        aUserId = aUserId.left(USER_ID_MAX_LEN);
        qWarning() << this << QStringLiteral("::setUserId id longer than allowed %1 chars and truncated to").arg(USER_ID_MAX_LEN) << aUserId;
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
    QTFIREBASE_ANALYTICS_CHECK_READY("::setUserProperties")
    if (_userProperties == userProperties)
        return;
    _userProperties = userProperties;

    if (_userProperties.size() > USER_PROPS_LIST_MAX_LEN)
        qWarning().noquote() << this << "::setUserProperties list length is more than" << USER_PROPS_LIST_MAX_LEN;

    uint index = 0;
    for (auto it = _userProperties.cbegin(); it != _userProperties.cend(); ++it, ++index) {
        const bool ok = it->canConvert<QVariantMap>();
        if (!ok) {
            qWarning() << this << "::setUserProperties wrong entry at index" << index;
            continue;
        }
        const auto map = it->toMap();
        if (map.isEmpty())
            continue;
        const auto &first = map.first();
        if (first.canConvert<QString>()) {
            const auto value = first.toString();
            const auto key = map.firstKey();
            setUserProperty(key, value);
        }
    }
    emit userPropertiesChanged();
}

void QtFirebaseAnalytics::setUserProperty(const QString &name, const QString &value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::setUserProperty")
    qDebug() << this << "::setUserProperty" << name << ":" << value;

    if (name.isEmpty()) {
        qWarning().noquote() << this << "::setUserProperty name is empty";
        return;
    }

    auto aName = name;
    if (aName.length() > USER_PROP_NAME_MAX_LEN) {
        aName = aName.left(USER_PROP_NAME_MAX_LEN);
        qWarning() << this << QStringLiteral("::setUserProperty name longer than allowed %1 chars and truncated to").arg(USER_PROP_NAME_MAX_LEN) << aName;
    }

    const auto aNameUtf8 = aName.toUtf8();

    const bool valid = std::all_of(aNameUtf8.cbegin(), aNameUtf8.cend(), [ ](const char ch) { return std::isalnum(ch); });
    if (!valid) {
        qWarning().noquote() << this << "::setUserProperty name is not alphanumeric" << aName;
        return;
    }

    if (!aName.at(0).isLetter()) {
        qWarning().noquote() << this << "::setUserProperty name must start with a letter" << aName;
        return;
    }

    auto aValue = value;
    if (aValue.length() > USER_PROP_VALUE_MAX_LEN) {
        aValue = aValue.left(USER_PROP_VALUE_MAX_LEN);
        qWarning() << this << QStringLiteral("::setUserProperty %1 value longer than allowed %2 chars and truncated to").arg(aName).arg(USER_PROP_VALUE_MAX_LEN) << aValue;
    }

    analytics::SetUserProperty(aNameUtf8.constData(), aValue.isEmpty() ? nullptr : aValue.toUtf8().constData());
}

void QtFirebaseAnalytics::unsetUserId()
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::unsetUserId")

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
    QTFIREBASE_ANALYTICS_CHECK_READY("::setCurrentScreen")
    qDebug() << this << "::setCurrentScreen" << screenName << ":" << screenClass;

    auto aName = screenName;
    if (aName.length() > SCREEN_NAME_MAX_LEN) {
        aName = aName.left(SCREEN_NAME_MAX_LEN);
        qWarning() << this << QStringLiteral("::setCurrentScreen name longer than allowed %1 chars and truncated to").arg(SCREEN_NAME_MAX_LEN) << aName;
    }

    auto aClass = screenClass;
    if (aClass.length() > SCREEN_CLASS_MAX_LEN) {
        aClass = aClass.left(SCREEN_CLASS_MAX_LEN);
        qWarning() << this << QStringLiteral("::setCurrentScreen class longer than allowed %1 chars and truncated to").arg(SCREEN_CLASS_MAX_LEN) << aClass;
    }

    analytics::SetCurrentScreen(aName.isEmpty() ? nullptr : aName.toUtf8().constData(), aClass.isEmpty() ? nullptr : aClass.toUtf8().constData());
}
#endif

void QtFirebaseAnalytics::logEvent(const QString &name)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << name << "(no params)";
    analytics::LogEvent(name.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &param, int value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << name << "int param" << param << ":" << value;
    analytics::LogEvent(name.toUtf8().constData(), param.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &param, double value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << name << "double param" << param << ":" << value;
    analytics::LogEvent(name.toUtf8().constData(), param.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &param, const QString &value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << name << "string param" << param << ":" << value;
    analytics::LogEvent(name.toUtf8().constData(), param.toUtf8().constData(), value.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QVariantMap &bundle)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

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

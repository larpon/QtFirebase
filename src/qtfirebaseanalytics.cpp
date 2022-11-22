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

const int EVENT_NAME_MAX_LEN { 40 };
const int PARAM_NAME_MAX_LEN { 40 };
const int PARAM_VALUE_MAX_LEN { 100 };

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

QString QtFirebaseAnalytics::fixStringLength(const QString &str, int maxLength, const char *func, const char *name) const
{
    if (str.length() <= maxLength)
        return str;
    const auto fixed = str.left(maxLength);
    qWarning() << this << QStringLiteral("%1 %2 longer than allowed %3 chars and truncated to").arg(func).arg(name).arg(maxLength) << fixed;
    return fixed;
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
    const auto aUserId = fixStringLength(userId, USER_ID_MAX_LEN, "::setUserId", "id");
    if (_userId == aUserId)
        return;

    qDebug() << this << "::setUserId" << aUserId;
    analytics::SetUserId(aUserId.toUtf8().constData());
    _userId = aUserId;
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

    if (userProperties.size() > USER_PROPS_LIST_MAX_LEN)
        qWarning().noquote() << this << "::setUserProperties list length is more than" << USER_PROPS_LIST_MAX_LEN;

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
        const auto value = first.toString();
        const auto key = map.firstKey();

        setUserProperty(key, value);
    }
    _userProperties = userProperties;
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

    const auto aName = fixStringLength(name, USER_PROP_NAME_MAX_LEN, "::setUserProperty", "name");
    const auto aNameUtf8 = aName.toUtf8();

    const bool valid = std::all_of(aNameUtf8.cbegin(), aNameUtf8.cend(), [ ](const char ch) { return std::isalnum(ch); });
    if (!valid) {
        qWarning().noquote() << this << "::setUserProperty name is not alphanumeric" << ':' << aName;
        return;
    }
    if (!std::isalpha(aNameUtf8[0])) {
        qWarning().noquote() << this << "::setUserProperty name must start with a letter" << ':' << aName;
        return;
    }

    const auto t = QStringLiteral("%1 value").arg(aName).toUtf8();
    const auto aValue = fixStringLength(name, USER_PROP_VALUE_MAX_LEN, "::setUserProperty", t.constData());

    analytics::SetUserProperty(aNameUtf8.constData(), aValue.isEmpty() ? nullptr : aValue.toUtf8().constData());
}

#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseAnalytics::setCurrentScreen(const QString &screenName, const QString &screenClass)
{
    qWarning() << this << "::setCurrentScreen is deprecated";
    QTFIREBASE_ANALYTICS_CHECK_READY("::setCurrentScreen")
    qDebug() << this << "::setCurrentScreen" << screenName << ":" << screenClass;

    const auto aName = fixStringLength(screenName, SCREEN_NAME_MAX_LEN, "::setCurrentScreen", "name");
    const auto aClass = fixStringLength(screenClass, SCREEN_CLASS_MAX_LEN, "::setCurrentScreen", "class");

    analytics::SetCurrentScreen(aName.isEmpty() ? nullptr : aName.toUtf8().constData(), aClass.isEmpty() ? nullptr : aClass.toUtf8().constData());
}
#endif

bool QtFirebaseAnalytics::checkEventName(QString &fixed, const QString &name) const
{
    if (name.isEmpty()) {
        qWarning().noquote() << this << "::logEvent event name is empty";
        return false;
    }

    if (name.startsWith(QStringLiteral("firebase_"))) {
        qWarning().noquote() << this << "::logEvent event name is reserved";
        return false;
    }

    const auto aName = fixStringLength(name, EVENT_NAME_MAX_LEN, "::logEvent", "name");
    const auto aNameUtf8 = aName.toUtf8();

    const bool valid = std::all_of(aNameUtf8.cbegin(), aNameUtf8.cend(), [ ](const char ch) { return std::isalnum(ch) || ch == '_'; });
    if (!valid) {
        qWarning().noquote() << this << "::logEvent name is not alphanumeric with underscores" << ':' << aName;
        return false;
    }

    if (!std::isalpha(aNameUtf8[0])) {
        qWarning().noquote() << this << "::logEvent name must start with a letter" << ':' << aName;
        return false;
    }

    fixed = aName;
    return true;
}

bool QtFirebaseAnalytics::checkParamName(QString &fixed, const QString &name) const
{
    if (name.isEmpty()) {
        qWarning().noquote() << this << "::logEvent param name is empty";
        return false;
    }

    static QStringList reserved {
        QStringLiteral("firebase_"),
        QStringLiteral("google_"),
        QStringLiteral("ga_"),
    };
    if (std::any_of(reserved.cbegin(), reserved.cend(), [ &name ](const QString &prefix) { return name.startsWith(prefix); })) {
        qWarning().noquote() << this << "::logEvent param name is reserved";
        return false;
    }

    const auto aName = fixStringLength(name, PARAM_NAME_MAX_LEN, "::logEvent", "param name");
    const auto aNameUtf8 = aName.toUtf8();

    const bool valid = std::all_of(aNameUtf8.cbegin(), aNameUtf8.cend(), [ ](const char ch) { return std::isalnum(ch) || ch == '_'; });
    if (!valid) {
        qWarning().noquote() << this << "::logEvent param name is not alphanumeric with underscores" << aName;
        return false;
    }

    if (!std::isalpha(aNameUtf8[0])) {
        qWarning().noquote() << this << "::logEvent param name must start with a letter" << aName;
        return false;
    }

    fixed = aName;
    return true;
}

void QtFirebaseAnalytics::logEvent(const QString &event)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << event << "with no params";

    QString eventFixed;
    if (!checkEventName(eventFixed, event))
        return;

    analytics::LogEvent(eventFixed.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, int value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << event << "int param" << param << ":" << value;

    QString eventFixed;
    QString paramFixed;
    if (!checkEventName(eventFixed, event))
        return;
    if (!checkParamName(paramFixed, param))
        return;

    analytics::LogEvent(eventFixed.toUtf8().constData(), paramFixed.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, long long value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << event << "long long param" << param << ":" << value;

    QString eventFixed;
    QString paramFixed;
    if (!checkEventName(eventFixed, event))
        return;
    if (!checkParamName(paramFixed, param))
        return;

    analytics::LogEvent(eventFixed.toUtf8().constData(), paramFixed.toUtf8().constData(), static_cast<int64_t>(value));
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, double value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << event << "double param" << param << ":" << value;

    QString eventFixed;
    QString paramFixed;
    if (!checkEventName(eventFixed, event))
        return;
    if (!checkParamName(paramFixed, param))
        return;

    analytics::LogEvent(eventFixed.toUtf8().constData(), paramFixed.toUtf8().constData(), value);
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QString &param, const QString &value)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")
    qDebug() << this << "::logEvent" << event << "string param" << param << ":" << value;

    QString eventFixed;
    QString paramFixed;
    if (!checkEventName(eventFixed, event))
        return;
    if (!checkParamName(paramFixed, param))
        return;

    const auto t = QStringLiteral("%1 value").arg(paramFixed).toUtf8();
    const auto aValue = fixStringLength(value, PARAM_VALUE_MAX_LEN, "::logEvent", t.constData());

    analytics::LogEvent(eventFixed.toUtf8().constData(), paramFixed.toUtf8().constData(), aValue.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &event, const QVariantMap &bundle)
{
    QTFIREBASE_ANALYTICS_CHECK_READY("::logEvent")

    qDebug().noquote() << this << "::logEvent bundle" << event;
    QString eventFixed;
    if (!checkEventName(eventFixed, event))
        return;

    QVector<analytics::Parameter> parameters;
    parameters.reserve(bundle.size());

    QByteArrayList stringsData;
    QByteArrayList keysData;
    for (auto it = bundle.cbegin(); it != bundle.cend(); ++it) {
        const auto &key = it.key();
        QString keyFixed;
        if (!checkParamName(keyFixed, key))
            continue;

        keysData << keyFixed.toUtf8();

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
            const auto t = QStringLiteral("%1 value").arg(keyFixed).toUtf8();
            const auto aValue = fixStringLength(value.toString(), PARAM_VALUE_MAX_LEN, "::logEvent", t.constData());

            const char *valueStr = nullptr;
            if (!aValue.isEmpty()) {
                stringsData << aValue.toUtf8();
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

    analytics::LogEvent(eventFixed.toUtf8().constData(), parameters.constData(), static_cast<size_t>(parameters.length()));
}

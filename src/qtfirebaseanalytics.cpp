#include "qtfirebaseanalytics.h"

#include <QGuiApplication>

namespace analytics = ::firebase::analytics;

QtFirebaseAnalytics *QtFirebaseAnalytics::self = nullptr;

QtFirebaseAnalytics::QtFirebaseAnalytics(QObject* parent)
    : QObject(parent)
    , _ready(false)
    , _initializing(false)
    , _enabled(false)
    , _minimumSessionDuration(0) // Depricated
    , _sessionTimeout(1800000)
    , _userId()
    , _userProperties()
{
    if(!self) {
        self = this;
        qDebug() << self << "::QtFirebaseAnalytics" << "singleton";
    }

    if(qFirebase->ready())
        init();
    else {
        connect(qFirebase,&QtFirebase::readyChanged, this, &QtFirebaseAnalytics::init);
        qFirebase->requestInit();
    }
}

QtFirebaseAnalytics::~QtFirebaseAnalytics()
{
    if(_ready) {
        qDebug() << self << "::~QtFirebaseAnalytics" << "shutting down";
        analytics::Terminate();
        _ready = false;
        self = nullptr;
    }
}

bool QtFirebaseAnalytics::checkInstance(const char *function)
{
    bool b = (QtFirebaseAnalytics::self != nullptr);
    if (!b)
        qWarning("QtFirebaseAnalytics::%s: Please instantiate the QtFirebaseAnalytics object first", function);
    return b;
}

void QtFirebaseAnalytics::setUserProperty(const QString &propertyName, const QString &propertyValue)
{
    if(!_ready) {
        qDebug() << this << "::setUserProperty native part not ready";
        return;
    }

    qDebug() << this << "::setUserProperty" << propertyName << ":" << propertyValue;
    analytics::SetUserProperty(propertyName.toLatin1().constData(), propertyValue.toLatin1().constData());
}

#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
void QtFirebaseAnalytics::setCurrentScreen(const QString &screenName, const QString &screenClass)
{
    if(!_ready) {
        qDebug() << this << "::setCurrentScreen native part not ready";
        return;
    }

    qDebug() << this << "::setCurrentScreen" << screenName << ":" << screenClass ;
    analytics::SetCurrentScreen(screenName.toLatin1().constData(), screenClass.toLatin1().constData());
}
#endif

void QtFirebaseAnalytics::logEvent(const QString &name)
{
    if(!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }

    qDebug() << this << "::logEvent" << name << "logging (no parameters)";
    analytics::LogEvent(name.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &parameterName, const QString &parameterValue)
{
    if(!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }

    qDebug() << this << "::logEvent" << name << "logging string parameter" << parameterName << ":" << parameterValue;
    analytics::LogEvent(name.toUtf8().constData(), parameterName.toUtf8().constData(),parameterValue.toUtf8().constData());
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &parameterName, const double parameterValue)
{
    if(!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }

    qDebug() << this << "::logEvent" << name << "logging double parameter" << parameterName << ":" << parameterValue;
    analytics::LogEvent(name.toUtf8().constData(), parameterName.toUtf8().constData(),parameterValue);
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QString &parameterName, const int parameterValue)
{
    if(!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }

    qDebug() << this << "::logEvent" << name << "logging int parameter" << parameterName << ":" << parameterValue;
    analytics::LogEvent(name.toUtf8().constData(), parameterName.toUtf8().constData(),parameterValue);
}

void QtFirebaseAnalytics::logEvent(const QString &name, const QVariantMap &bundle)
{
    if(!_ready) {
        qDebug() << this << "::logEvent native part not ready";
        return;
    }

    analytics::Parameter *parameters = new analytics::Parameter[bundle.size()];

    QByteArrayList keys;
    QByteArrayList strings;

    QMapIterator<QString, QVariant> i(bundle);
    int index = 0;
    while (i.hasNext()) {
        i.next();

        keys.append(i.key().toUtf8());
        QString eventKey = i.key();
        QVariant variant = i.value();

        if(variant.type() == QVariant::Type(QMetaType::Int)) {
            parameters[index] = analytics::Parameter(keys.at(index).constData(), variant.toInt());
            qDebug() << this << "::logEvent" << "bundle parameter" << eventKey << ":" << variant.toInt();
        } else if(variant.type() == QVariant::Type(QMetaType::Double)) {
            parameters[index] = analytics::Parameter(keys.at(index).constData(),variant.toDouble());
            qDebug() << this << "::logEvent" << "bundle parameter" << eventKey << ":" << variant.toDouble();
        } else if(variant.type() == QVariant::Type(QMetaType::QString)) {
            strings.append(variant.toString().toUtf8());
            parameters[index] = analytics::Parameter(keys.at(index).constData(), strings.at(strings.size()-1).constData());
            qDebug() << this << "::logEvent" << "bundle parameter" << eventKey << ":" << strings.at(strings.size()-1);
        } else {
            qWarning() << this << "::logEvent" << "bundle parameter" << eventKey << "has unsupported data type. Sending empty strings";
            parameters[index] = analytics::Parameter("", "");
        }

        index++;
    }

    qDebug() << this << "::logEvent" << "logging" << "bundle" << name;
    analytics::LogEvent(name.toUtf8().constData(), parameters, static_cast<size_t>(bundle.size()));
    delete[] parameters;
    parameters = nullptr;
}

QVariantList QtFirebaseAnalytics::userProperties() const
{
    return _userProperties;
}

void QtFirebaseAnalytics::setUserProperties(const QVariantList &userProperties)
{
    if(!_ready) {
        qDebug() << this << "::setUserProperties native part not ready";
        return;
    }

    if (_userProperties != userProperties) {
        _userProperties = userProperties;

        if(_userProperties.size() > 25) {

        }

        unsigned index = 0;
        for (QVariantList::iterator j = _userProperties.begin(); j != _userProperties.end(); j++)
        {
            if((*j).canConvert<QVariantMap>()) {

                QVariantMap map = (*j).toMap();

                if(!map.isEmpty()) {
                    if(map.first().canConvert<QString>()) {
                        QString key = map.firstKey();
                        QString value = map.first().toString();
                        setUserProperty(key,value);
                    }
                }

            } else {
                qWarning() << this << "::setUserProperties" << "wrong entry in userProperties list at index" << index;
            }
            index++;
        }
        emit userPropertiesChanged();
    }
}

QString QtFirebaseAnalytics::userId() const
{
    return _userId;
}

void QtFirebaseAnalytics::setUserId(const QString &userId)
{
    if(!_ready) {
        qDebug() << this << "::setUserId native part not ready";
        return;
    }

    QString aUserId = userId;
    if(aUserId.isEmpty()) {
        unsetUserId();
    }
    if(aUserId.length() > 36) {
        aUserId = aUserId.left(36);
        qWarning() << this << "::setUserId" << "ID longer than allowed 36 chars" << "TRUNCATED to" << aUserId;
    }
    if(_userId != aUserId) {
        _userId = aUserId;
        analytics::SetUserId(_userId.toLatin1().constData());
        qDebug() << this << "::setUserId sat to" << _userId;
        emit userIdChanged();
    }
}

void QtFirebaseAnalytics::unsetUserId()
{
    if(!_ready) {
        qDebug() << this << "::unsetUserId native part not ready";
        return;
    }

    if(!_userId.isEmpty()) {
        _userId.clear();
        analytics::SetUserId(nullptr);
        emit userIdChanged();
    }
}

unsigned int QtFirebaseAnalytics::sessionTimeout() const
{
    return _sessionTimeout;
}

void QtFirebaseAnalytics::setSessionTimeout(unsigned int sessionTimeout)
{
    if(_sessionTimeout != sessionTimeout) {
        _sessionTimeout = sessionTimeout;
        emit sessionTimeoutChanged();

        if(_ready) {
            analytics::SetSessionTimeoutDuration(_sessionTimeout);
        }
    }
}

bool QtFirebaseAnalytics::ready()
{
    return _ready;
}

void QtFirebaseAnalytics::setReady(bool ready)
{
    if (_ready != ready) {
        _ready = ready;
        qDebug() << self << "::setReady" << ready;
        emit readyChanged();
    }
}

bool QtFirebaseAnalytics::enabled()
{
    return _enabled;
}

void QtFirebaseAnalytics::setEnabled(bool enabled)
{
    if(!_ready) {
        qDebug() << this << "::setEnabled native part not ready";
        return;
    }

    if (_enabled != enabled) {
        analytics::SetAnalyticsCollectionEnabled(enabled);
        _enabled = enabled;
        qDebug() << self << "::setEnabled" << enabled;
        emit enabledChanged();
    }
}

unsigned int QtFirebaseAnalytics::minimumSessionDuration()
{
    return _minimumSessionDuration;
}

void QtFirebaseAnalytics::setMinimumSessionDuration(unsigned int minimumSessionDuration)
{
    qWarning() << this << "::setMinimumSessionDuration is deprecated and no longer functional!";

    if(!_ready) {
        qDebug() << this << "::setMinimumSessionDuration native part not ready";
        return;
    }

    if (_minimumSessionDuration != minimumSessionDuration) {
        _minimumSessionDuration = minimumSessionDuration;
        qDebug() << self << "::setMinimumSessionDuration" << minimumSessionDuration;
        emit minimumSessionDurationChanged();
    }
}

void QtFirebaseAnalytics::init()
{
    if(!qFirebase->ready()) {
        qDebug() << self << "::init" << "base not ready";
        return;
    }

    if(!_ready && !_initializing) {
        _initializing = true;

        analytics::Initialize(*qFirebase->firebaseApp());
        qDebug() << self << "::init" << "native initialized";
        _initializing = false;
        setReady(true);

        // Set current session duration timeout if it was set up already
        analytics::SetSessionTimeoutDuration(_sessionTimeout);
    }
}

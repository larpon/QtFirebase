#ifndef QTFIREBASE_ANALYTICS_H
#define QTFIREBASE_ANALYTICS_H

#ifdef QTFIREBASE_BUILD_ANALYTICS

#include "src/qtfirebase.h"

#if defined(qFirebaseAnalytics)
#undef qFirebaseAnalytics
#endif
#define qFirebaseAnalytics (static_cast<QtFirebaseAnalytics *>(QtFirebaseAnalytics::instance()))

#include "firebase/analytics.h"
#include "firebase/analytics/event_names.h"
#include "firebase/analytics/parameter_names.h"
#include "firebase/analytics/user_property_names.h"

#include <QDebug>
#include <QObject>
#include <QVariantMap>
#include <QVariantList>

class QtFirebaseAnalytics : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    Q_PROPERTY(unsigned int minimumSessionDuration READ minimumSessionDuration WRITE setMinimumSessionDuration NOTIFY minimumSessionDurationChanged)
    Q_PROPERTY(unsigned int sessionTimeout READ sessionTimeout WRITE setSessionTimeout NOTIFY sessionTimeoutChanged)

    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QVariantList userProperties READ userProperties WRITE setUserProperties NOTIFY userPropertiesChanged)

public:
    explicit QtFirebaseAnalytics(QObject* parent = nullptr);
    ~QtFirebaseAnalytics();

    static QtFirebaseAnalytics *instance() {
        if(!self) {
            self = new QtFirebaseAnalytics();
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    bool checkInstance(const char *function);

    bool ready();
    void setReady(bool ready);

    bool enabled();
    void setEnabled(bool enabled);

    unsigned int minimumSessionDuration();
    void setMinimumSessionDuration(unsigned int minimumSessionDuration);

    unsigned int sessionTimeout() const;
    void setSessionTimeout(unsigned int sessionTimeout);

    QString userId() const;
    Q_INVOKABLE void unsetUserId();

    QVariantList userProperties() const;
    void setUserProperties(const QVariantList &userProperties);

signals:
    void readyChanged();
    void enabledChanged();
    void minimumSessionDurationChanged();
    void sessionTimeoutChanged();
    void userIdChanged();
    void userPropertiesChanged();

public slots:
    void setUserId(const QString &userId);
    void setUserProperty(const QString &propertyName, const QString &propertyValue);
#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    void setCurrentScreen(const QString &screenName, const QString &screenClass);
#endif
    void logEvent(const QString &name);
    void logEvent(const QString &name, const QString &parameterName, const QString &parameterValue);
    void logEvent(const QString &name, const QString &parameterName, const double parameterValue);
    void logEvent(const QString &name, const QString &parameterName, const int parameterValue);
    void logEvent(const QString &name, const QVariantMap &bundle);
    //void logEvent(const QString &name, const QString &parameterName, const int64_t parameterValue);

private slots:
    void init();

private:
    static QtFirebaseAnalytics *self;
    Q_DISABLE_COPY(QtFirebaseAnalytics)

    bool _ready;
    bool _initializing;

    bool _enabled;
    unsigned int _minimumSessionDuration;
    unsigned int _sessionTimeout;

    QString _userId;

    QVariantList _userProperties;
};

#endif // QTFIREBASE_BUILD_ANALYTICS
#endif // QTFIREBASE_ANALYTICS_H

#ifndef QTFIREBASE_ANALYTICS_H
#define QTFIREBASE_ANALYTICS_H

#ifdef QTFIREBASE_BUILD_ANALYTICS

#include "qtfirebase.h"

#if defined(qFirebaseAnalytics)
#undef qFirebaseAnalytics
#endif
#define qFirebaseAnalytics (static_cast<QtFirebaseAnalytics *>(QtFirebaseAnalytics::instance()))

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
    explicit QtFirebaseAnalytics(QObject* parent = nullptr) { Q_UNUSED(parent) }
    ~QtFirebaseAnalytics() {}

    static QtFirebaseAnalytics *instance() {
        if(!self) {
            self = new QtFirebaseAnalytics();
        }
        return self;
    }
    bool checkInstance(const char *function) { Q_UNUSED(function) return false; }

    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready) }

    bool enabled() { return false; }
    void setEnabled(bool enabled) { Q_UNUSED(enabled) }

    unsigned int minimumSessionDuration() { return 0; }
    void setMinimumSessionDuration(unsigned int minimumSessionDuration) { Q_UNUSED(minimumSessionDuration) }

    unsigned int sessionTimeout() const { return 0; }
    void setSessionTimeout(unsigned int sessionTimeout) { Q_UNUSED(sessionTimeout) }

    QString userId() const { return QStringLiteral(""); }
    Q_INVOKABLE void unsetUserId() {}

    QVariantList userProperties() const { return QVariantList(); }
    void setUserProperties(const QVariantList &userProperties) { Q_UNUSED(userProperties) }

signals:
    void readyChanged();
    void enabledChanged();
    void minimumSessionDurationChanged();
    void sessionTimeoutChanged();
    void userIdChanged();
    void userPropertiesChanged();

public slots:
    void setUserId(const QString &userId) { Q_UNUSED(userId) }
    void setUserProperty(const QString &propertyName, const QString &propertyValue) { Q_UNUSED(propertyName) Q_UNUSED(propertyValue) }
    void setCurrentScreen(const QString &screenName, const QString &screenClass) { Q_UNUSED(screenName) Q_UNUSED(screenClass) }
    void logEvent(const QString &name) { Q_UNUSED(name) }
    void logEvent(const QString &name, const QString &parameterName, const QString &parameterValue) { Q_UNUSED(name) Q_UNUSED(parameterName) Q_UNUSED(parameterValue) }
    void logEvent(const QString &name, const QString &parameterName, const double parameterValue) { Q_UNUSED(name) Q_UNUSED(parameterName) Q_UNUSED(parameterValue) }
    void logEvent(const QString &name, const QString &parameterName, const int parameterValue) { Q_UNUSED(name) Q_UNUSED(parameterName) Q_UNUSED(parameterValue) }
    void logEvent(const QString &name, const QVariantMap bundle) { Q_UNUSED(name) Q_UNUSED(bundle) }

private:
    static QtFirebaseAnalytics *self;
    Q_DISABLE_COPY(QtFirebaseAnalytics)
};

#endif // QTFIREBASE_BUILD_ANALYTICS
#endif // QTFIREBASE_ANALYTICS_H

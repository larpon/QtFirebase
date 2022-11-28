#ifndef QTFIREBASE_ANALYTICS_H
#define QTFIREBASE_ANALYTICS_H

#ifdef QTFIREBASE_BUILD_ANALYTICS

#include "src/qtfirebase.h"

#include <QObject>
#include <QVariantMap>
#include <QVariantList>

#if defined(qFirebaseAnalytics)
#undef qFirebaseAnalytics
#endif
#define qFirebaseAnalytics (QtFirebaseAnalytics::instance())

class QtFirebaseAnalytics : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QtFirebaseAnalytics)

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(uint minimumSessionDuration READ minimumSessionDuration WRITE setMinimumSessionDuration NOTIFY minimumSessionDurationChanged)
    Q_PROPERTY(uint sessionTimeout READ sessionTimeout WRITE setSessionTimeout NOTIFY sessionTimeoutChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QVariantList userProperties READ userProperties WRITE setUserProperties NOTIFY userPropertiesChanged)

    static QtFirebaseAnalytics *self;
public:
    static QtFirebaseAnalytics *instance(QObject *parent = nullptr) {
        if (!self)
            self = new QtFirebaseAnalytics(parent);
        return self;
    }

    static bool checkInstance(const char *function = nullptr) { Q_UNUSED(function) return self; }

    explicit QtFirebaseAnalytics(QObject *parent = nullptr) : QObject(parent) { }

    bool ready() const { return false; }
    bool enabled() const { return false; }
    uint minimumSessionDuration() const { return 0; }
    uint sessionTimeout() const { return 0; }
    QString userId() const { return QString(); }
    QVariantList userProperties() const { return QVariantList(); }

    void setEnabled(bool = true) { }
    void setMinimumSessionDuration(uint) { }
    void setSessionTimeout(uint) { }
    void setUserId(const QString &) { }
    void setUserProperties(const QVariantList &) { }
public slots:
    void setUserProperty(const QString &, const QString &) { }
    void unsetUserId() { }

    void setCurrentScreen(const QString &, const QString &) { }

    void logEvent(const QString &) { }
    void logEvent(const QString &, const QString &, int) { }
    void logEvent(const QString &, const QString &, long long) { }
    void logEvent(const QString &, const QString &, double) { }
    void logEvent(const QString &, const QString &, const QString &) { }
    void logEvent(const QString &, const QVariantMap &) { }

signals:
    void readyChanged();
    void enabledChanged();
    void minimumSessionDurationChanged();
    void sessionTimeoutChanged();
    void userIdChanged();
    void userPropertiesChanged();
};

#endif // QTFIREBASE_BUILD_ANALYTICS
#endif // QTFIREBASE_ANALYTICS_H

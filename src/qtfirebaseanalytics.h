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

    explicit QtFirebaseAnalytics(QObject *parent = nullptr);
    virtual ~QtFirebaseAnalytics();

    bool ready() const { return _ready; }
    bool enabled() const { return _enabled; }
    uint minimumSessionDuration() const { return _minimumSessionDuration; }
    uint sessionTimeout() const { return _sessionTimeout; }
    QString userId() const { return _userId; }
    QVariantList userProperties() const { return _userProperties; }

    void setReady(bool = true);
    void setEnabled(bool = true);
    void setMinimumSessionDuration(uint);
    void setSessionTimeout(uint);
    void setUserId(const QString &);
    void setUserProperties(const QVariantList &);
public slots:
    void setUserProperty(const QString &name, const QString &value);

    void unsetUserId();

#if QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 0, 0)
    void setCurrentScreen(const QString &screenName, const QString &screenClass);
#endif

    void logEvent(const QString &name);
    void logEvent(const QString &name, const QString &param, int value);
    void logEvent(const QString &name, const QString &param, double value);
    void logEvent(const QString &name, const QString &param, const QString &value);
    void logEvent(const QString &name, const QVariantMap &bundle);

signals:
    void readyChanged();
    void enabledChanged();
    void minimumSessionDurationChanged();
    void sessionTimeoutChanged();
    void userIdChanged();
    void userPropertiesChanged();

private slots:
    void init();
private:
    QString fixStringLength(const QString &str, uint maxLength, const char *func, const char *name) const;
    QString fixStringLength(const QString &str, uint maxLength, const char *func, const QByteArray &name) const {
        return fixStringLength(str, maxLength, func, name.constData());
    }
    bool checkEventName(QString &fixed, const QString &name) const;
    bool checkParamName(QString &fixed, const QString &name) const;
private:

    bool _ready = false;
    bool _enabled = false;
    uint _minimumSessionDuration = 0;
    uint _sessionTimeout = 1800000;
    QString _userId;
    QVariantList _userProperties;
};

#endif // QTFIREBASE_BUILD_ANALYTICS
#endif // QTFIREBASE_ANALYTICS_H

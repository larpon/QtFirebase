#ifndef QTFIREBASE_MESSAGING_H
#define QTFIREBASE_MESSAGING_H

#ifdef QTFIREBASE_BUILD_MESSAGING

#include "src/qtfirebase.h"

#include <QObject>
#include <QVariantMap>

#include <QQmlParserStatus>

#if defined(qFirebaseMessaging)
#undef qFirebaseMessaging
#endif
#define qFirebaseMessaging (QtFirebaseMessaging::instance())

class MessageListener;

class QtFirebaseMessaging : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_DISABLE_COPY(QtFirebaseMessaging)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool hasMissingDependency READ hasMissingDependency NOTIFY hasMissingDependencyChanged)
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)
    Q_PROPERTY(QVariantMap data READ data NOTIFY dataChanged)

    static QtFirebaseMessaging *self;
public:
    static QtFirebaseMessaging *instance(QObject *parent = nullptr) {
        if (!self)
            self = new QtFirebaseMessaging(parent);
        return self;
    }

    static bool checkInstance(const char *function = nullptr) { Q_UNUSED(function) return self; }

    explicit QtFirebaseMessaging(QObject *parent = nullptr) : QObject(parent) { }

    void classBegin() override { }
    void componentComplete() override { }

    bool ready() const { return false; }
    bool hasMissingDependency() const { return false; }
    QString token() const { return QString(); }
    QVariantMap data() const { return QVariantMap(); }

    // TODO: make setters private
    void setReady(bool = true) { }
    void setHasMissingDependency(bool = true) { }
    void setToken(const QString &) { }
    void setData(const QVariantMap &) { }

public slots:
    void subscribe(const QString &topic) { Q_UNUSED(topic) }
    void unsubscribe(const QString &topic) { Q_UNUSED(topic) }

signals:
    void readyChanged();
    void hasMissingDependencyChanged();
    void tokenChanged();
    void dataChanged();

    void messageReceived();
    void subscribed(QString topic);
    void unsubscribed(QString topic);

    void error(int code, QString message);
};

#endif // QTFIREBASE_BUILD_MESSAGING
#endif // QTFIREBASE_MESSAGING_H

#ifndef QTFIREBASE_MESSAGING_H
#define QTFIREBASE_MESSAGING_H

#ifdef QTFIREBASE_BUILD_MESSAGING

#include "qtfirebase.h"

#if defined(qFirebaseMessaging)
#undef qFirebaseMessaging
#endif
#define qFirebaseMessaging (static_cast<QtFirebaseMessaging *>(QtFirebaseMessaging::instance()))

#include <QObject>
#include <QVariantMap>
#include <QVariantList>

class QtFirebaseMessaging : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap data READ data NOTIFY dataChanged)
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)

public:
    explicit QtFirebaseMessaging(QObject* parent = nullptr) { Q_UNUSED(parent) }
    ~QtFirebaseMessaging() {}

    static QtFirebaseMessaging *instance() {
        if(!self) {
            self = new QtFirebaseMessaging();
        }
        return self;
    }

    bool checkInstance(const char *function) { Q_UNUSED(function) return false; }

    bool ready() { return false; }
    void setReady(bool ready) { Q_UNUSED(ready) }

    QVariantMap data() { return QVariantMap(); }
    void setData(QVariantMap data) { Q_UNUSED(data) }

    QString token() { return QString(); }
    void setToken(QString token) { Q_UNUSED(token) }

    Q_INVOKABLE void subscribe(const QString &topic) { Q_UNUSED(topic) }
    Q_INVOKABLE void unsubscribe(const QString &topic) { Q_UNUSED(topic) }

signals:
    void readyChanged();
    void dataChanged();
    void tokenChanged();
    void messageReceived();

    void error(int code, QString message);

    // Topic signals
    void subscribed(const QString &topic);
    void unsubscribed(const QString &topic);


private:
    static QtFirebaseMessaging *self;
    Q_DISABLE_COPY(QtFirebaseMessaging)
};

#endif // QTFIREBASE_BUILD_MESSAGING
#endif // QTFIREBASE_MESSAGING_H

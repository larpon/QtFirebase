#ifndef QTFIREBASE_MESSAGING_H
#define QTFIREBASE_MESSAGING_H

#ifdef QTFIREBASE_BUILD_MESSAGING

#include "src/qtfirebase.h"

#if defined(qFirebaseMessaging)
#undef qFirebaseMessaging
#endif
#define qFirebaseMessaging (static_cast<QtFirebaseMessaging *>(QtFirebaseMessaging::instance()))

#include "firebase/app.h"
#include "firebase/messaging.h"
#include "firebase/util.h"

#include <QDebug>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QQmlParserStatus>
#include <QMetaMethod>

class MessageListener;
class QtFirebaseMessaging: public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap data READ data NOTIFY dataChanged)
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)

public:
    explicit QtFirebaseMessaging(QObject* parent = 0);
    ~QtFirebaseMessaging();

    void classBegin() override;
    void componentComplete() override;

    static QtFirebaseMessaging *instance() {
        if(self == 0) {
            self = new QtFirebaseMessaging(0);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    bool checkInstance(const char *function);

    bool ready();
    void setReady(bool ready);

    QVariantMap data();
    void setData(QVariantMap data);

    QString token();
    void setToken(QString token);

private slots:
    void init();
    void onFutureEvent(QString eventId, firebase::FutureBase future);
    void getMessage();
    void getToken();

signals:
    void readyChanged();
    void dataChanged();
    void tokenChanged();
    void messageReceived();

private:
    static QtFirebaseMessaging *self;
    Q_DISABLE_COPY(QtFirebaseMessaging)

    bool _ready;
    bool _initializing;

    QString __QTFIREBASE_ID;
    MessageListener* g_listener = nullptr;
    QVariantMap _data;
    QString _token;
};

class MessageListener : public QObject, public firebase::messaging::Listener
{
    Q_OBJECT

public:
    MessageListener(QObject* parent = nullptr);

    virtual void OnMessage(const ::firebase::messaging::Message& message);
    virtual void OnTokenReceived(const char* token);

    QVariantMap data();
    void setData(QVariantMap data);

    QString token();
    void setToken(QString token);

protected:
    void connectNotify(const QMetaMethod &signal) override;

signals:
    void emitMessageReceived();
    void onMessageReceived();
    void onTokenReceived();
    void onConnected();

private:
    QVariantMap _data;
    QString _token;
    bool _connected = false;
    bool _notify = false;
};

#endif // QTFIREBASE_BUILD_MESSAGING
#endif // QTFIREBASE_MESSAGING_H

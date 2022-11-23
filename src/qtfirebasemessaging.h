#ifndef QTFIREBASE_MESSAGING_H
#define QTFIREBASE_MESSAGING_H

#ifdef QTFIREBASE_BUILD_MESSAGING

#include "src/qtfirebase.h"

#include <firebase/messaging.h>

#include <QObject>
#include <QVariantMap>
#include <QMetaMethod>
#include <QMutex>
#include <QMutexLocker>

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

    enum Error {
        ErrorNone = firebase::messaging::kErrorNone,
        ErrorFailedToRegisterForRemoteNotifications = firebase::messaging::kErrorFailedToRegisterForRemoteNotifications,
        ErrorInvalidTopicName = firebase::messaging::kErrorInvalidTopicName,
        ErrorNoRegistrationToken = firebase::messaging::kErrorNoRegistrationToken,
        ErrorUnknown = firebase::messaging::kErrorUnknown,
    };
    Q_ENUM(Error)

    explicit QtFirebaseMessaging(QObject *parent = nullptr);
    virtual ~QtFirebaseMessaging();

    void classBegin() override { }
    void componentComplete() override;

    bool ready() const { return _ready; }
    bool hasMissingDependency() const { return _hasMissingDependency; }
    QString token() const { QMutexLocker locker { &(const_cast<QtFirebaseMessaging *>(this)->_tokenMutex) }; return _token; }
    QVariantMap data() const { return _data; }

    void setReady(bool = true);
    void setHasMissingDependency(bool = true);
    void setToken(const QString &);
    void setData(const QVariantMap &);

public slots:
    void subscribe(const QString &topic);
    void unsubscribe(const QString &topic);

signals:
    void readyChanged();
    void hasMissingDependencyChanged();
    void tokenChanged();
    void dataChanged();

    void messageReceived();
    void subscribed(QString topic);
    void unsubscribed(QString topic);

    void error(int code, QString message);

private slots:
    void init();
    void onFutureEvent(const QString &eventId, const firebase::FutureBase &future);
    void getMessage();
    void getToken();

private:
    const QString __QTFIREBASE_ID;

    bool _initializing = false;

    bool _ready = false;
    bool _hasMissingDependency = false;
    QString _token;
    QVariantMap _data;

    MessageListener *_listener = nullptr;
    QMutex _tokenMutex;
};

class MessageListener : public QObject, public firebase::messaging::Listener
{
    Q_OBJECT
    Q_DISABLE_COPY(MessageListener)
public:
    explicit MessageListener(QObject* parent = nullptr);
    QString token() const { return _token; }
    QVariantMap data() const { return _data; }

    void setToken(const QString &);
    void setData(const QVariantMap &);

    virtual void OnTokenReceived(const char *) override;
    virtual void OnMessage(const firebase::messaging::Message &) override;
signals:
    void emitMessageReceived();

    void onConnected();
    void onTokenReceived();
    void onMessageReceived();

protected:
    void connectNotify(const QMetaMethod &signal) override;
private:
    QString _token;
    QVariantMap _data;

    bool _messageReceivedConnected = false;
    bool _tokenReceivedConnected = false;
    bool _notifyMessageReceived = false;
    bool _notifyTokenReceived = false;

    QMutex _tokenMutex;
};

#endif // QTFIREBASE_BUILD_MESSAGING
#endif // QTFIREBASE_MESSAGING_H

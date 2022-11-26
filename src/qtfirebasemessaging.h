#ifndef QTFIREBASE_MESSAGING_H
#define QTFIREBASE_MESSAGING_H

#ifdef QTFIREBASE_BUILD_MESSAGING

#include "src/qtfirebase.h"

#include <firebase/messaging.h>

#include <QObject>
#include <QVariantMap>

#include <QQmlParserStatus>

// https://github.com/firebase/firebase-cpp-sdk/pull/667
#if (QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(7, 0, 0)) && (QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 6, 0))
#ifdef Q_OS_ANDROID
#define QTFIREBASE_ANDROID_FIX
#endif
#endif

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
    QString token() const { return _token; }
    QVariantMap data() const { return _data; }

    // TODO: make setters private
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

private:
    const QString __QTFIREBASE_ID;

    bool _ready = false;
    bool _hasMissingDependency = false;
    QString _token;
    QVariantMap _data;

    MessageListener *_listener = nullptr;
};

#endif // QTFIREBASE_BUILD_MESSAGING
#endif // QTFIREBASE_MESSAGING_H

#include "qtfirebasemessaging.h"

#include <firebase/app.h>
#include <firebase/messaging.h>
#include <firebase/internal/common.h>

#define QTFIREBASE_MESSAGING_CHECK_READY(name) \
    if (!_ready) { \
        qDebug().noquote() << this << name << "native part not ready"; \
        return; \
    }

namespace messaging = firebase::messaging;

QtFirebaseMessaging *QtFirebaseMessaging::self { nullptr };

QtFirebaseMessaging::QtFirebaseMessaging(QObject *parent)
    : QObject(parent)
    , __QTFIREBASE_ID(QString().asprintf("%8p", static_cast<void *>(this)))
    , _listener(new MessageListener(this))
{
    // deny multiple instances
    Q_ASSERT(!self);
    if (self)
        return;
    self = this;

    QTimer::singleShot(0, this, [ this ] {
        if (qFirebase->ready()) {
            init();
            return;
        }
        connect(qFirebase, &QtFirebase::readyChanged, this, &QtFirebaseMessaging::init);
        qFirebase->requestInit();
    });
}

QtFirebaseMessaging::~QtFirebaseMessaging()
{
    if (_ready)
        messaging::Terminate();

    // check this instance is legal
    if (self == this)
        self = nullptr;
}

void QtFirebaseMessaging::componentComplete()
{
    // Connect on componentComplete so the signals are emited in the correct order
    connect(_listener, &MessageListener::onMessageReceived, this, &QtFirebaseMessaging::getMessage);
    connect(_listener, &MessageListener::onTokenReceived, this, &QtFirebaseMessaging::getToken);
}

void QtFirebaseMessaging::init()
{
    if (!qFirebase->ready() || _ready)
        return;

    const auto result = messaging::Initialize(*qFirebase->firebaseApp(), _listener);
    if (result == firebase::kInitResultFailedMissingDependency) {
        qWarning().noquote() << this << "failed to initialize due to a missing dependency";
        return setHasMissingDependency();
    }
    if (result != firebase::kInitResultSuccess) {
        qWarning().noquote() << this << "failed to initialize due to an unknown error" << result;
        return;
    }

#ifdef QTFIREBASE_ANDROID_FIX
    const auto future = messaging::GetToken();
    future.OnCompletion([ this ](const firebase::FutureBase &future) {
        const auto code = future.error();
        const auto message = QString::fromUtf8(future.error_message());
        if (code) {
            emit error(code, message);
            return;
        }

        setReady();

        const auto result = static_cast<const std::string *>(future.result_void());
        if (result)
            self->setToken(QString::fromStdString(*result));
    });
#else
    setReady();
#endif
}

void QtFirebaseMessaging::subscribe(const QString &topic)
{
    QTFIREBASE_MESSAGING_CHECK_READY("::subscribe")

    const auto result = firebase::messaging::Subscribe(topic.toUtf8());
    result.OnCompletion([ this, topic ](const firebase::FutureBase &future) {
        const auto code = future.error();
        const auto message = QString::fromUtf8(future.error_message());
        if (code) {
            emit error(code, message);
            return;
        }
        emit subscribed(topic);
    });
}

void QtFirebaseMessaging::unsubscribe(const QString &topic)
{
    QTFIREBASE_MESSAGING_CHECK_READY("::unsubscribe")

    const auto result = firebase::messaging::Unsubscribe(topic.toUtf8());
    result.OnCompletion([ this, topic ](const firebase::FutureBase &future) {
        const auto code = future.error();
        const auto message = QString::fromUtf8(future.error_message());
        if (code) {
            emit error(code, message);
            return;
        }
        emit unsubscribed(topic);
    });
}

void QtFirebaseMessaging::getMessage()
{
    setData(_listener->data());
}

void QtFirebaseMessaging::getToken()
{
    setToken(_listener->token());
}

void QtFirebaseMessaging::setReady(bool ready)
{
    if (_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

void QtFirebaseMessaging::setHasMissingDependency(bool hasMissingDependency)
{
    if (_hasMissingDependency != hasMissingDependency) {
        _hasMissingDependency = hasMissingDependency;
        emit hasMissingDependencyChanged();
    }
}

void QtFirebaseMessaging::setData(const QVariantMap &data)
{
    if (_data != data) {
        _data = data;
        emit dataChanged();
        emit messageReceived();
    }
}

void QtFirebaseMessaging::setToken(const QString &token)
{
    QMutexLocker lock { &_tokenMutex };
    if (_token != token) {
        _token = token;
        lock.unlock();

        emit tokenChanged();
    }
}

MessageListener::MessageListener(QObject* parent)
    : QObject(parent)
{
}

void MessageListener::OnMessage(const messaging::Message &message)
{
    // When messages are received by the server, they are placed into an
    // internal queue, waiting to be consumed. When ProcessMessages is called,
    // this OnMessage function is called once for each queued message.

    QVariantMap data;

    if (message.notification) {
        if (!message.notification->title.empty()) {
            const QString key = QStringLiteral("nTitle");
            const QString value = QString::fromStdString(message.notification->title.c_str());
            data.insert(key, value);
        }
        if (!message.notification->body.empty()) {
            const QString key = QStringLiteral("nBody");
            const QString value = QString::fromStdString(message.notification->body.c_str());
            data.insert(key, value);
        }
        if (!message.notification->icon.empty()) {
            const QString key = QStringLiteral("nIcon");
            const QString value = QString::fromStdString(message.notification->icon.c_str());
            data.insert(key, value);
        }
        if (!message.notification->tag.empty()) {
            const QString key = QStringLiteral("nTag");
            const QString value = QString::fromStdString(message.notification->tag.c_str());
            data.insert(key, value);
        }
        if (!message.notification->color.empty()) {
            const QString key = QStringLiteral("nColor");
            const QString value = QString::fromStdString(message.notification->color.c_str());
            data.insert(key, value);
        }
        if (!message.notification->sound.empty()) {
            const QString key = QStringLiteral("nSound");
            const QString value = QString::fromStdString(message.notification->sound.c_str());
            data.insert(key, value);
        }
        if (!message.notification->click_action.empty()) {
            const QString key = QStringLiteral("nClickAction");
            const QString value = QString::fromStdString(message.notification->click_action.c_str());
            data.insert(key, value);
        }
    }

    if (message.notification_opened) {
        const QString key = QStringLiteral("launchnotification");
        data.insert(key, true);
    }

    for (const auto& field : message.data)
    {
        const QString key = QString::fromStdString(field.first);
        const QString value = QString::fromStdString(field.second);

        data.insert(key, value);
    }

    setData(data);
}

void MessageListener::OnTokenReceived(const char *token)
{
    setToken(QString::fromUtf8(token));
}

void MessageListener::connectNotify(const QMetaMethod &signal)
{
    if (signal == QMetaMethod::fromSignal(&MessageListener::onMessageReceived)) {
        _messageReceivedConnected = true;

        if(_notifyMessageReceived) {
            emit onMessageReceived();
            _notifyMessageReceived = false;
        }
    }

    if (signal == QMetaMethod::fromSignal(&MessageListener::onTokenReceived)) {
        _tokenReceivedConnected = true;

        if(_notifyTokenReceived) {
            emit onTokenReceived();
            _notifyTokenReceived = false;
        }
    }
}

void MessageListener::setData(const QVariantMap &data)
{
    if (_data != data) {
        _notifyMessageReceived = true;
        _data = data;

        if(_messageReceivedConnected) {
            emit onMessageReceived();
            _notifyMessageReceived = false;
        }
    }
}

void MessageListener::setToken(const QString &token)
{
    QMutexLocker lock { &_tokenMutex };
    if (_token != token) {
        _token = token;
        lock.unlock();

        _notifyTokenReceived = true;

        if(_tokenReceivedConnected) {
            emit onTokenReceived();
            _notifyTokenReceived = false;
        }
    }
}

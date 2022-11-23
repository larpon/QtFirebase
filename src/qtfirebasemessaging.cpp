#include "qtfirebasemessaging.h"

#if (QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(7, 0, 0)) && (QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 6, 0))
#if defined(Q_OS_ANDROID)
#include <QPointer>
#endif
#endif

#include <firebase/app.h>
#include <firebase/messaging.h>
#include <firebase/internal/common.h>

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

    connect(qFirebase, &QtFirebase::futureEvent, this, &QtFirebaseMessaging::onFutureEvent);

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
    if(!qFirebase->ready()) {
        qDebug() << self << "::init" << "base not ready";
        return;
    }

    if(!_ready && !_initializing) {
        _initializing = true;

        auto initResult = messaging::Initialize(*qFirebase->firebaseApp(), _listener);
        if(firebase::kInitResultFailedMissingDependency == initResult)
            setHasMissingDependency(true);

        _initializing = false;

        if(firebase::kInitResultSuccess != initResult)
            return;

        setReady(true);
#if (QTFIREBASE_FIREBASE_VERSION >= QTFIREBASE_FIREBASE_VERSION_CHECK(7, 0, 0)) && (QTFIREBASE_FIREBASE_VERSION < QTFIREBASE_FIREBASE_VERSION_CHECK(8, 6, 0))
#if defined(Q_OS_ANDROID) // https://github.com/firebase/firebase-cpp-sdk/pull/667
        QPointer<QtFirebaseMessaging> self { this };

        // firebase::messaging::Listener::OnTokenReceived() may not be called on second app launch
        auto future = messaging::GetToken();
        future.OnCompletion([self, future](const firebase::FutureBase &){
            if(!self)
                return;
            if(future.result())
                self->setToken(QString::fromStdString(*future.result()));
        });
#endif
#endif
    }
}

void QtFirebaseMessaging::onFutureEvent(const QString &eventId, const firebase::FutureBase &future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    qDebug() << self << "::onFutureEvent" << eventId;

    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        _initializing = false;
        return;
    }
    qDebug() << this << "::onFutureEvent initialized ok";
    _initializing = false;
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

void QtFirebaseMessaging::subscribe(const QString &topic)
{
    if(!_ready) {
        qDebug() << this << "::subscribe native part not ready";
        return;
    }

    // TODO queue these futures so repeated calls don't get lost
    auto result = firebase::messaging::Subscribe(topic.toUtf8());

    result.OnCompletion([this, topic](const firebase::FutureBase &completed_future){
        if(completed_future.error() == firebase::messaging::kErrorNone) {
            emit subscribed(topic);
        } else {
            emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
        }
    });
}

void QtFirebaseMessaging::unsubscribe(const QString &topic)
{
    if(!_ready) {
        qDebug() << this << "::unsubscribe native part not ready";
        return;
    }

    // TODO queue these futures so repeated calls don't get lost
    auto result = firebase::messaging::Unsubscribe(topic.toUtf8());

    result.OnCompletion([this, topic](const firebase::FutureBase &completed_future){
        if(completed_future.error() == firebase::messaging::kErrorNone) {
            emit unsubscribed(topic);
        } else {
            emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
        }
    });
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

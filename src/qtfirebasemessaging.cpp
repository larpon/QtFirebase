#include "qtfirebasemessaging.h"

#include <QTimer>

#define QTFIREBASE_MESSAGING_CHECK_READY(name) \
    if (!_ready) { \
        qDebug().noquote() << this << name << "native part not ready"; \
        return; \
    }

namespace messaging = firebase::messaging;

class MessageListener : public messaging::Listener
{
    QtFirebaseMessaging *q;
public:
    MessageListener(QtFirebaseMessaging *q) : q(q) { }
    void OnTokenReceived(const char *) override;
    void OnMessage(const messaging::Message &) override;
};

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

    delete _listener;

    // check this instance is legal
    if (self == this)
        self = nullptr;
}

void QtFirebaseMessaging::componentComplete()
{
    if (!_token.isEmpty())
        emit tokenChanged();
    if (!_data.isEmpty()) {
        emit dataChanged();

        emit messageReceived();
    }
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
        const auto result = static_cast<const std::string *>(future.result_void());
        const auto token = result ? QString::fromStdString(*result) : QString();
        QTimer::singleShot(0, this, [ this, token ] {
            setReady();
            if (!token.isEmpty())
                setToken(token);
        });
    });
#else
    setReady();
#endif
}

void QtFirebaseMessaging::subscribe(const QString &topic)
{
    QTFIREBASE_MESSAGING_CHECK_READY("::subscribe")

    qDebug() << this << "::subscribe" << topic;

    const auto result = messaging::Subscribe(topic.toUtf8());
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

    qDebug() << this << "::unsubscribe" << topic;

    const auto result = messaging::Unsubscribe(topic.toUtf8());
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

void QtFirebaseMessaging::setReady(bool ready)
{
    if (_ready == ready)
        return;
    _ready = ready;
    emit readyChanged();
}

void QtFirebaseMessaging::setHasMissingDependency(bool hasMissingDependency)
{
    if (_hasMissingDependency == hasMissingDependency)
        return;
    _hasMissingDependency = hasMissingDependency;
    emit hasMissingDependencyChanged();
}

void QtFirebaseMessaging::setToken(const QString &token)
{
    if (_token == token)
        return;
    _token = token;
    emit tokenChanged();
}

void QtFirebaseMessaging::setData(const QVariantMap &data)
{
    if (_data == data)
        return;
    _data = data;
    emit dataChanged();

    emit messageReceived();
}

void MessageListener::OnTokenReceived(const char *token)
{
    const auto t = QString::fromUtf8(token);
    QTimer::singleShot(0, q, [ this, t ] { q->setToken(t); });
}

void MessageListener::OnMessage(const messaging::Message &message)
{
    QVariantMap data;

    const auto notification = message.notification;
    if (notification) {
        const QMap<QString, QString> notificationData {
            { QStringLiteral("nTitle"), QString::fromStdString(notification->title) },
            { QStringLiteral("nBody"), QString::fromStdString(notification->body) },
            { QStringLiteral("nIcon"), QString::fromStdString(notification->icon) },
            { QStringLiteral("nTag"), QString::fromStdString(notification->tag) },
            { QStringLiteral("nColor"), QString::fromStdString(notification->color) },
            { QStringLiteral("nSound"), QString::fromStdString(notification->sound) },
            { QStringLiteral("nClickAction"), QString::fromStdString(notification->click_action) },
        };
        const auto keys = notificationData.keys();
        for (const auto &key : qAsConst(keys)) {
            const auto &value = notificationData[key];
            if (!value.isEmpty())
                data[key] = value;
        }
    }

    if (message.notification_opened)
        data[QStringLiteral("launchnotification")] = true;

    for (const auto &field : message.data) {
        const auto key = QString::fromStdString(field.first);
        const auto value = QString::fromStdString(field.second);

        data[key] = value;
    }

    QTimer::singleShot(0, q, [ this, data ] { q->setData(data); });
}

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

    if (!_data.isEmpty())
        emit dataChanged();
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

    QTimer::singleShot(0, q, [ this, data ] { q->setData(data); });
}

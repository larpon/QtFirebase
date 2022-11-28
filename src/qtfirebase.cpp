#include "qtfirebase.h"

#define QTFIREBASE_REQUEST_INIT QTimer::singleShot(1000, this, [ this ] { requestInitInternal(true); })

QtFirebase *QtFirebase::self { nullptr };

QtFirebase::QtFirebase(QObject *parent)
    : QObject(parent)
{
    // deny multiple instances
    Q_ASSERT(!self);
    if (self)
        return;
    self = this;

    // NOTE where having trouble getting a valid UIView pointer using just signals.
    // So this is a hack that let's us check the pointer every 1 seconds during startup.
    // We should probably set a limit to how many checks should be done before giving up?
    // connect(qGuiApp, &QGuiApplication::focusWindowChanged, this, &QtFirebase::requestInit); // <-- Crashes on iOS

    QTFIREBASE_REQUEST_INIT;
}

QtFirebase::~QtFirebase()
{
    // delete _firebaseApp?

    // check this instance is legal
    if (self == this)
        self = nullptr;
}

void QtFirebase::requestInitInternal(bool repeat)
{
    if (_firebaseApp)
        return;

    const auto retry = [ = ] { if (repeat) QTFIREBASE_REQUEST_INIT; };

    const auto p = PlatformUtils::getNativeWindow();
    if (!p) {
        qDebug() << this << "no native UI pointer";
        return retry();
    }

#ifdef Q_OS_ANDROID
    QAndroidJniEnvironment env;

    _firebaseApp = firebase::App::Create(firebase::AppOptions(), env, p);
#else
    _firebaseApp = firebase::App::Create();
#endif
    if (!_firebaseApp) {
        qWarning() << this << "failed to create Firebase App";
        return retry();
    }
    qDebug() << this << "created Firebase App" << _firebaseApp;
    emit readyChanged();
}

void QtFirebase::addFuture(const QString &eventId, const firebase::FutureBase &future)
{
    if (_futures.contains(eventId))
        qWarning() << this << "::addFuture already contains" << eventId;
    _futures[eventId] = future;
    qDebug() << this << "::addFuture added" << eventId;

    qDebug() << this << "::addFuture watch" << eventId;

    future.OnCompletion([ this, eventId ](const firebase::FutureBase &) {
        // callback may be in different thread
        QTimer::singleShot(0, qFirebase, [ this, eventId ] {
            Q_ASSERT(_futures.contains(eventId));
            if (!_futures.contains(eventId))
                return;

            const auto future = _futures.take(eventId);
            qDebug() << this << "::addFuture ready" << eventId;
            emit futureEvent(eventId, future);
        });
    });
}

void QtFirebase::waitForFutureCompletion(firebase::FutureBase future)
{
    qDebug() << self << "::waitForFutureCompletion waiting for future";

    int count = 0;
    while (future.status() == firebase::kFutureStatusPending) {
        QGuiApplication::processEvents();
        count++;
        if (!(count % 100))
            qDebug().noquote() << self << "future is still pending";
        if (!(count % 200)) {
            qDebug() << self << "something is probably wrong, breaking wait cycle";

            break;
        }
        QThread::msleep(10);
    }

    const auto status = future.status();
    if (status == firebase::kFutureStatusComplete)
        qDebug() << self << "::waitForFutureCompletion ended with COMPLETE";
    else if (status == firebase::kFutureStatusInvalid)
        qDebug() << self << "::waitForFutureCompletion ended with INVALID";
    else if (status == firebase::kFutureStatusPending)
        qDebug() << self << "::waitForFutureCompletion ended with PENDING";
}

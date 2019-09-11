#include "qtfirebase.h"

#include <QMutableMapIterator>
#include <QThread>

QtFirebase *QtFirebase::self = nullptr;

QtFirebase::QtFirebase(QObject* parent) : QObject(parent)
{
    _ready = false;

    qDebug() << self << ":QtFirebase(QObject* parent)" ;

    Q_ASSERT_X(!self, "QtFirebase", "there should be only one firebase object");
    QtFirebase::self = this;

    // NOTE where having trouble getting a valid UIView pointer using just signals.
    // So this is a hack that let's us check the pointer every 1 seconds during startup.
    // We should probably set a limit to how many checks should be done before giving up?
    //connect(qGuiApp,&QGuiApplication::focusWindowChanged, this, &QtFirebase::init); // <-- Crashes on iOS
    _initTimer = new QTimer(self);
    _initTimer->setSingleShot(false);
    connect(_initTimer, &QTimer::timeout, self, &QtFirebase::requestInit);
    _initTimer->start(1000);

    _futureWatchTimer = new QTimer(self);
    _futureWatchTimer->setSingleShot(false);
    connect(_futureWatchTimer, &QTimer::timeout, self, &QtFirebase::processEvents);
}

QtFirebase::~QtFirebase()
{
    self = nullptr;
    //delete _firebaseApp;
}

bool QtFirebase::checkInstance(const char *function)
{
    bool b = (QtFirebase::self != nullptr);
    if (!b)
        qWarning("QtFirebase::%s: Please instantiate the QtFirebase object first", function);
    return b;
}

bool QtFirebase::ready() const
{
    return _ready;
}

void QtFirebase::waitForFutureCompletion(firebase::FutureBase future)
{
    static int count = 0;
    qDebug() << self << "::waitForFutureCompletion" << "waiting for future" << &future << "completion. Initial status" << future.status();
    while(future.status() == firebase::kFutureStatusPending) {
        QGuiApplication::processEvents();
        count++;

        if(count % 100 == 0)
            qDebug() << count << "Future" << &future << "is still pending. Has current status" << future.status();

        if(count % 200 == 0) {
            qDebug() << count << "Future" << &future << "is still pending. Something is probably wrong. Breaking wait cycle. Current status" << future.status();
            count = 0;
            break;
        }
        QThread::msleep(10);
    }
    count = 0;

    if(future.status() == firebase::kFutureStatusComplete) {
       qDebug() << self << "::waitForFutureCompletion" << "ended with COMPLETE";
    }

    if(future.status() == firebase::kFutureStatusInvalid) {
       qDebug() << self << "::waitForFutureCompletion" << "ended with INVALID";
    }

    if(future.status() == firebase::kFutureStatusPending) {
       qDebug() << self << "::waitForFutureCompletion" << "ended with PENDING";
    }
}

firebase::App* QtFirebase::firebaseApp() const
{
    return _firebaseApp;
}

void QtFirebase::addFuture(const QString &eventId, const firebase::FutureBase &future)
{
    qDebug() << self << "::addFuture" << "adding" << eventId;

    if (_futureMap.contains(eventId))
    {
        qWarning() << "_futureMap already contains '" << eventId << "'.";
    }

    _futureMap.insert(eventId,future);

    //if(!_futureWatchTimer->isActive()) {
    qDebug() << self << "::addFuture" << "starting future watch";
    _futureWatchTimer->start(1000);
    //}

}

void QtFirebase::requestInit()
{
    if(!PlatformUtils::getNativeWindow()) {
        qDebug() << self << "::requestInit" << "no native UI pointer";
        return;
    }

    if(!_ready) {

        #if defined(Q_OS_ANDROID)

        jobject activity = PlatformUtils::getNativeWindow();

        QAndroidJniEnvironment env;

        // Create the Firebase app.
        _firebaseApp = firebase::App::Create(firebase::AppOptions(), env, activity);

        #else // Q_OS_ANDROID

        // Create the Firebase app.
        _firebaseApp = firebase::App::Create();

        #endif

        qDebug("QtFirebase::requestInit created the Firebase App (%x)",static_cast<int>(reinterpret_cast<intptr_t>(_firebaseApp)));

        _initTimer->stop();
        delete _initTimer;

        _ready = true;
        qDebug() << self << "::requestInit" << "initialized";
        emit readyChanged();
    }
}

void QtFirebase::processEvents()
{
    qDebug() << self << "::processEvents" << "processing events";
    QMutableMapIterator<QString, firebase::FutureBase> i(_futureMap);
    while (i.hasNext()) {
        i.next();
        if(i.value().status() != firebase::kFutureStatusPending) {
            qDebug() << self << "::processEvents" << "future event" << i.key();
            //if(_futureMap.remove(i.key()) >= 1) //QMap holds only one key. Use QMultiMap for multiple keys.
            const auto key = i.key();
            const auto value = i.value();
            i.remove();
            qDebug() << self << "::processEvents" << "removed future event" << key;
            emit futureEvent(key, value);
            // To easen events up:
            //break;
        }

    }

    if(_futureMap.isEmpty()) {
        qDebug() << self << "::processEvents" << "stopping future watch";
        _futureWatchTimer->stop();
    }

}

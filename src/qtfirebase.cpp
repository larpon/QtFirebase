#include "qtfirebase.h"

QtFirebase *QtFirebase::self = 0;

QtFirebase::QtFirebase(QObject* parent) : QObject(parent)
{
    _ready = false;

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
    self = 0;
    //delete _firebaseApp;
}

bool QtFirebase::checkInstance(const char *function)
{
    bool b = (QtFirebase::self != 0);
    if (!b)
        qWarning("QtFirebase::%s: Please instantiate the QtFirebase object first", function);
    return b;
}

bool QtFirebase::ready()
{
    return _ready;
}

int count = 0;
void QtFirebase::waitForFutureCompletion(firebase::FutureBase future)
{
    qDebug() << self << "::waitForFutureCompletion" << "waiting for future" << &future << "completion. Initial status" << future.Status();
    while(future.Status() == firebase::kFutureStatusPending) {
        QGuiApplication::processEvents();
        count++;

        if(count % 100000 == 0)
            qDebug() << count << "Future" << &future << "is still pending. Has current status" << future.Status();

        if(count % 200000 == 0) {
            qDebug() << count << "Future" << &future << "is still pending. Something is probably wrong. Breaking wait cycle. Current status" << future.Status();
            count = 0;
            break;
        }
    }
    count = 0;

    if(future.Status() == firebase::kFutureStatusComplete) {
       qDebug() << self << "::waitForFutureCompletion" << "ended with COMPLETE";
    }

    if(future.Status() == firebase::kFutureStatusInvalid) {
       qDebug() << self << "::waitForFutureCompletion" << "ended with INVALID";
    }

    if(future.Status() == firebase::kFutureStatusPending) {
       qDebug() << self << "::waitForFutureCompletion" << "ended with PENDING";
    }
}

firebase::App* QtFirebase::firebaseApp()
{
    return _firebaseApp;
}

void QtFirebase::addFuture(QString eventId, firebase::FutureBase future)
{
    qDebug() << self << "::addFuture" << "adding" << eventId;
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

        #if defined(__ANDROID__)

        jobject activity = PlatformUtils::getNativeWindow();

        QAndroidJniEnvironment env;

        // Create the Firebase app.
        _firebaseApp = firebase::App::Create(firebase::AppOptions(), env, activity);

        #else // __ANDROID__

        // Create the Firebase app.
        _firebaseApp = firebase::App::Create(firebase::AppOptions());

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
    QMapIterator<QString, firebase::FutureBase> i(_futureMap);
    while (i.hasNext()) {
        i.next();
        if(i.value().Status() != firebase::kFutureStatusPending) {
            qDebug() << self << "::processEvents" << "future event" << i.key();
            if(_futureMap.remove(i.key()) >= 1)
                qDebug() << self << "::processEvents" << "removed future event" << i.key();
            emit futureEvent(i.key(),i.value());

            // To easen events up:
            //break;
        }

    }

    if(_futureMap.isEmpty()) {
        qDebug() << self << "::processEvents" << "stopping future watch";
        _futureWatchTimer->stop();
    }

}

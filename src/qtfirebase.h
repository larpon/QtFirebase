#ifndef QTFIREBASE_H
#define QTFIREBASE_H

#if defined(qFirebase)
#undef qFirebase
#endif
#define qFirebase (static_cast<QtFirebase *>(QtFirebase::instance()))

#include "platformutils.h"

#include "firebase/app.h"
#include "firebase/future.h"
#include "firebase/util.h"

#include <QMap>
#include <QObject>
#include <QTimer>
#include <QGuiApplication>

class QtFirebase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
    explicit QtFirebase(QObject* parent = 0);
    ~QtFirebase();

    static QtFirebase *instance() {
        if(self == 0)
            self = new QtFirebase(0);
        return self;
    }

    bool ready() const;

    static void waitForFutureCompletion(firebase::FutureBase future);
    bool checkInstance(const char *function);

    firebase::App* firebaseApp() const;

    // TODO make protected and have friend classes?
    void addFuture(const QString &eventId, const firebase::FutureBase &future);

signals:
    void readyChanged();

    void futureEvent(const QString &eventId, firebase::FutureBase future);

public slots:
    void requestInit();
    void processEvents();

private:
    static QtFirebase *self;
    Q_DISABLE_COPY(QtFirebase)

    bool _ready = false;
    firebase::App* _firebaseApp;

    QTimer *_initTimer;

    QTimer *_futureWatchTimer;
    QMap<QString, firebase::FutureBase> _futureMap;
};

#endif // QTFIREBASE_H

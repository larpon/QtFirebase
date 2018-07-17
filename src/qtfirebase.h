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
    explicit QtFirebase(QObject* parent = nullptr);
    ~QtFirebase();

    static QtFirebase *instance() {
        if(!self)
            self = new QtFirebase();
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
    firebase::App* _firebaseApp = nullptr;

    QTimer *_initTimer = nullptr;

    QTimer *_futureWatchTimer = nullptr;
    QMap<QString, firebase::FutureBase> _futureMap;
};

#endif // QTFIREBASE_H

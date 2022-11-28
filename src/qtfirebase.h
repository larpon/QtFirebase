#ifndef QTFIREBASE_H
#define QTFIREBASE_H

#include "platformutils.h"

#include <firebase/version.h>
#include <firebase/app.h>
#include <firebase/future.h>
#include <firebase/util.h>

#include <QObject>
#include <QGuiApplication>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QHash>

// Like the QT_VERSION
#define QTFIREBASE_FIREBASE_VERSION QTFIREBASE_FIREBASE_VERSION_CHECK(\
FIREBASE_VERSION_MAJOR,\
FIREBASE_VERSION_MINOR,\
FIREBASE_VERSION_REVISION\
)

// Like the QT_VERSION_CHECK
#define QTFIREBASE_FIREBASE_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#if defined(qFirebase)
#undef qFirebase
#endif
#define qFirebase (QtFirebase::instance())

class QtFirebase : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QtFirebase)

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

    static QtFirebase *self;
public:
    static QtFirebase *instance(QObject *parent = nullptr) {
        if (!self)
            self = new QtFirebase(parent);
        return self;
    }

    static bool checkInstance(const char *function = nullptr) { Q_UNUSED(function) return self; }

    explicit QtFirebase(QObject *parent = nullptr);
    virtual ~QtFirebase();
    bool ready() const { return _firebaseApp; }
    firebase::App *firebaseApp() const { return _firebaseApp; }

    // TODO make protected and have friend classes?
    void addFuture(const QString &eventId, const firebase::FutureBase &);
    static void waitForFutureCompletion(firebase::FutureBase);
public slots:
    void requestInit() { requestInitInternal(false); }  // Calls from QtFirebase's modules
signals:
    void readyChanged();
    void futureEvent(const QString &eventId, firebase::FutureBase future);

private:
    void requestInitInternal(bool repeat = true);

private:
    firebase::App *_firebaseApp = nullptr;
    QHash<QString, firebase::FutureBase> _futures;
};

#endif // QTFIREBASE_H

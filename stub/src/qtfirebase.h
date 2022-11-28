#ifndef QTFIREBASE_H
#define QTFIREBASE_H

#include "platformutils.h"

#include <QObject>
#include <QGuiApplication>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QHash>

#if defined(qFirebase)
#undef qFirebase
#endif
#define qFirebase (QtFirebase::instance())

namespace firebase {
class App;
}

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

    explicit QtFirebase(QObject *parent = nullptr) : QObject(parent) { }
    bool ready() const { return false; }
    firebase::App *firebaseApp() const { return nullptr; }

    // TODO make protected and have friend classes?
public slots:
    void requestInit() { }
signals:
    void readyChanged();
};

#endif // QTFIREBASE_H

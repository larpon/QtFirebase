#ifndef QTFIREBASE_H
#define QTFIREBASE_H

#if defined(qFirebase)
#undef qFirebase
#endif
#define qFirebase (static_cast<QtFirebase *>(QtFirebase::instance()))

#include <QObject>

class QtFirebase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
    explicit QtFirebase(QObject* parent = nullptr) { Q_UNUSED(parent) }
    ~QtFirebase() {}

    static QtFirebase *instance() {
        if(!self)
            self = new QtFirebase();
        return self;
    }

    bool ready() { return false; }

    bool checkInstance(const char *function) { Q_UNUSED(function) return false; }

signals:
    void readyChanged();

public slots:
    void requestInit() {}
    void processEvents() {}

private:
    static QtFirebase *self;
    Q_DISABLE_COPY(QtFirebase)

};

#endif // QTFIREBASE_H

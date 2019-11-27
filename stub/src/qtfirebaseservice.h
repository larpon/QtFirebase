#ifndef QTFIREBASE_SERVICE_H
#define QTFIREBASE_SERVICE_H

#include "qtfirebase.h"

#include <QObject>
#include <QVariant>

template <typename ResultType> const ResultType* result(const void* p) {
  return p==nullptr ? nullptr : static_cast<const ResultType*>(p);
}
/*
void printQtVariant(const QVariant& v, const QString& tab = QString());
void printFbVariant(const firebase::Variant& v, const QString& tab = QString());
*/
class QtFirebaseService: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
public:
    explicit QtFirebaseService(QObject* parent = nullptr) { Q_UNUSED(parent) }
    ~QtFirebaseService() {}
    bool ready() const { return true; }
    static QVariant fromFirebaseVariant(const QVariant& v) { return v; }
    static QVariant fromQtVariant(const QVariant& v) { return v; }
signals:
    void readyChanged();

protected:

    void startInit() {  }
    virtual void setReady(bool value) = 0;
    virtual bool initializing() const = 0;
    virtual void setInitializing(bool value) = 0;
    virtual void init() = 0;
    virtual void onFutureEvent(QString eventId, int future) = 0;

    QString __QTFIREBASE_ID;
    bool _ready;
    bool _initializing;
};

#endif // QTFIREBASE_SERVICE_H

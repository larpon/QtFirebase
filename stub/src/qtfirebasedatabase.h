#ifndef QTFIREBASE_DATABASE_H
#define QTFIREBASE_DATABASE_H
#include <QObject>
#include <QVariant>

#ifdef QTFIREBASE_BUILD_DATABASE
#include "qtfirebase.h"
#include "qtfirebaseservice.h"
#if defined(qFirebaseDatabase)
#undef qFirebaseDatabase
#endif
#define qFirebaseDatabase (static_cast<QtFirebaseDatabase*>(QtFirebaseDatabase::instance()))

class QtFirebaseDatabaseRequest;
class QtFirebaseDatabase : public QtFirebaseService
{
    Q_OBJECT
    typedef QSharedPointer<QtFirebaseDatabase> Ptr;
public:
    static QtFirebaseDatabase* instance() {
        if(self == nullptr) {
            self = new QtFirebaseDatabase(nullptr);
        }
        return self;
    }

    enum Error
    {
        ErrorNone,
        ErrorDisconnected,
        ErrorExpiredToken,
        ErrorInvalidToken,
        ErrorMaxRetries,
        ErrorNetworkError,
        ErrorOperationFailed,
        ErrorOverriddenBySet,
        ErrorPermissionDenied,
        ErrorUnavailable,
        ErrorUnknownError,
        ErrorWriteCanceled,
        ErrorInvalidVariantType,
        ErrorConflictingOperationInProgress,
        ErrorTransactionAbortedByUser
    };
    Q_ENUM(Error)

    void setReady(bool value) { Q_UNUSED(value) }
    bool initializing() const { return false; }
    void setInitializing(bool value) { Q_UNUSED(value) }
    void init() { }
    void onFutureEvent(QString eventId, int future) { Q_UNUSED(eventId) Q_UNUSED(future) }

private:
    explicit QtFirebaseDatabase(QObject *parent = nullptr){ Q_UNUSED(parent) }
    static QtFirebaseDatabase* self;
    Q_DISABLE_COPY(QtFirebaseDatabase)
    friend class QtFirebaseDatabaseRequest;
};

class QtFirebaseDataSnapshot: public QObject
{
    Q_OBJECT
public:
    QtFirebaseDataSnapshot(){}
public slots:
    bool exists() const{return false;}
    QString key() const{return QString();}
    QVariant value() const{return QVariant();}
    QByteArray jsonString() const{return QByteArray();}
    bool hasChildren() const{return false;}
    bool valid() const{return false;}
};

class QtFirebaseDatabaseRequest;
class QtFirebaseDatabaseQuery: public QObject
{
    Q_OBJECT
public:
    QtFirebaseDatabaseQuery(){}
public slots:
    QtFirebaseDatabaseQuery* orderByKey(){return nullptr;}
    QtFirebaseDatabaseQuery* orderByValue(){return nullptr;}
    QtFirebaseDatabaseQuery* orderByChild(const QString& path){ Q_UNUSED(path) return nullptr;}
    QtFirebaseDatabaseQuery* orderByPriority(){return nullptr;}
    QtFirebaseDatabaseQuery* startAt(QVariant order_value){ Q_UNUSED(order_value) return nullptr;}
    QtFirebaseDatabaseQuery* startAt(QVariant order_value, const QString& child_key){ Q_UNUSED(order_value) Q_UNUSED(child_key) return nullptr; }
    QtFirebaseDatabaseQuery* endAt(QVariant order_value){ Q_UNUSED(order_value) return nullptr;}
    QtFirebaseDatabaseQuery* endAt(QVariant order_value, const QString& child_key){ Q_UNUSED(order_value) Q_UNUSED(child_key) return nullptr; }
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value){ Q_UNUSED(order_value) return nullptr;}
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value, const QString& child_key){ Q_UNUSED(order_value) Q_UNUSED(child_key) return nullptr; }
    QtFirebaseDatabaseQuery* limitToFirst(size_t limit){ Q_UNUSED(limit) return nullptr; }
    QtFirebaseDatabaseQuery* limitToLast(size_t limit){ Q_UNUSED(limit) return nullptr; }
    void exec(){}
};

class QtFirebaseDatabaseRequest: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QtFirebaseDataSnapshot* snapshot READ snapshot NOTIFY snapshotChanged)
public slots:
    //Data request
    QtFirebaseDatabaseRequest* child(const QString& path = QString()){ Q_UNUSED(path) return nullptr;}
    QtFirebaseDatabaseRequest* pushChild(){return nullptr;}
    void setValue(const QVariant& value){ Q_UNUSED(value) }
    void requestValue(){}
    void updateTree(const QVariant& tree){ Q_UNUSED(tree) }
    void remove(){}

    //Filters
    QtFirebaseDatabaseQuery* orderByKey(){return nullptr;}
    QtFirebaseDatabaseQuery* orderByValue(){return nullptr;}
    QtFirebaseDatabaseQuery* orderByChild(const QString& path){ Q_UNUSED(path) return nullptr;}
    QtFirebaseDatabaseQuery* orderByPriority(){return nullptr;}
    QtFirebaseDatabaseQuery* startAt(QVariant order_value){ Q_UNUSED(order_value) return nullptr;}
    QtFirebaseDatabaseQuery* startAt(QVariant order_value, const QString& child_key){ Q_UNUSED(order_value) Q_UNUSED(child_key) return nullptr;}
    QtFirebaseDatabaseQuery* endAt(QVariant order_value){ Q_UNUSED(order_value) return nullptr;}
    QtFirebaseDatabaseQuery* endAt(QVariant order_value, const QString& child_key){ Q_UNUSED(order_value) Q_UNUSED(child_key) return nullptr;}
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value){ Q_UNUSED(order_value) return nullptr;}
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value, const QString& child_key){ Q_UNUSED(order_value) Q_UNUSED(child_key) return nullptr;}
    QtFirebaseDatabaseQuery* limitToFirst(size_t limit){ Q_UNUSED(limit) return nullptr;}
    QtFirebaseDatabaseQuery* limitToLast(size_t limit){ Q_UNUSED(limit) return nullptr;}

    //State
    bool running() const{return false;}
    int errorId() const{return 0;}
    bool hasError() const{return false;}
    QString errorMsg() const{return QString();}

    //Data access
    QString childKey() const{return QString();}
    QtFirebaseDataSnapshot* snapshot(){return nullptr;}
signals:
    void completed(bool success);
    void runningChanged();
    void snapshotChanged();

};

#endif //QTFIREBASE_BUILD_DATABASE

#endif // QTFIREBASE_DATABASE_H

#ifndef QTFIREBASEDATABASE_H
#define QTFIREBASEDATABASE_H
#include <QObject>
#include <QVariant>

#ifdef QTFIREBASE_BUILD_DATABASE
#include "src/qtfirebase.h"
#if defined(qFirebaseDb)
#undef qFirebaseDb
#endif
#define qFirebaseDb (static_cast<QtFirebaseDb*>(QtFirebaseDb::instance()))

class QtFirebaseDbRequest;
class QtFirebaseDb: public QObject
{
    Q_OBJECT
    typedef QSharedPointer<QtFirebaseDb> Ptr;
public:
    static QtFirebaseDb* instance() {
        if(self == 0) {
            self = new QtFirebaseDb(0);
        }
        return self;
    }

    enum Error
    {
        kErrorNone,
        kErrorDisconnected,
        kErrorExpiredToken,
        kErrorInvalidToken,
        kErrorMaxRetries,
        kErrorNetworkError,
        kErrorOperationFailed,
        kErrorOverriddenBySet,
        kErrorPermissionDenied,
        kErrorUnavailable,
        kErrorUnknownError,
        kErrorWriteCanceled,
        kErrorInvalidVariantType,
        kErrorConflictingOperationInProgress,
        kErrorTransactionAbortedByUser
    };
    Q_ENUM(Error)
private:
    explicit QtFirebaseDb(QObject *parent = 0){Q_UNUSED(parent);}
    static QtFirebaseDb* self;
    Q_DISABLE_COPY(QtFirebaseDb)
    friend class QtFirebaseDbRequest;
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
    bool isValid() const{return false;}
};

class QtFirebaseDbRequest;
class QtFirebaseDbQuery: public QObject
{
    Q_OBJECT
public:
    QtFirebaseDbQuery(){}
public slots:
    QtFirebaseDbQuery* orderByKey(){return nullptr;}
    QtFirebaseDbQuery* orderByValue(){return nullptr;}
    QtFirebaseDbQuery* orderByChild(const QString& path){Q_UNUSED(path); return nullptr;}
    QtFirebaseDbQuery* orderByPriority(){return nullptr;}
    QtFirebaseDbQuery* startAt(QVariant order_value){Q_UNUSED(order_value); return nullptr;}
    QtFirebaseDbQuery* startAt(QVariant order_value, const QString& child_key){Q_UNUSED(order_value); Q_UNUSED(child_key); return nullptr;}
    QtFirebaseDbQuery* endAt(QVariant order_value){Q_UNUSED(order_value); return nullptr;}
    QtFirebaseDbQuery* endAt(QVariant order_value, const QString& child_key){Q_UNUSED(order_value); Q_UNUSED(child_key); return nullptr;}
    QtFirebaseDbQuery* equalTo(QVariant order_value){Q_UNUSED(order_value); return nullptr;}
    QtFirebaseDbQuery* equalTo(QVariant order_value, const QString& child_key){Q_UNUSED(order_value); Q_UNUSED(child_key); return nullptr;}
    QtFirebaseDbQuery* limitToFirst(size_t limit){Q_UNUSED(limit); return nullptr;}
    QtFirebaseDbQuery* limitToLast(size_t limit){Q_UNUSED(limit); return nullptr;}
    void getValue(){}
signals:
    void queryRun();
};

class QtFirebaseDbRequest: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(QtFirebaseDataSnapshot* snapshot READ getSnapshot NOTIFY snapshotChanged)
public slots:
    //Data request
    QtFirebaseDbRequest* child(const QString& path = QString()){Q_UNUSED(path); return nullptr;}
    QtFirebaseDbRequest* pushChild(){return nullptr;}
    void setValue(const QVariant& value){Q_UNUSED(value);}
    void getValue(){}
    void updateTree(const QVariant& tree){Q_UNUSED(tree);}
    void remove(){}

    //Filters
    QtFirebaseDbQuery* orderByKey(){return nullptr;}
    QtFirebaseDbQuery* orderByValue(){return nullptr;}
    QtFirebaseDbQuery* orderByChild(const QString& path){Q_UNUSED(path);return nullptr;}
    QtFirebaseDbQuery* orderByPriority(){return nullptr;}
    QtFirebaseDbQuery* startAt(QVariant order_value){Q_UNUSED(order_value); return nullptr;}
    QtFirebaseDbQuery* startAt(QVariant order_value, const QString& child_key){Q_UNUSED(order_value); Q_UNUSED(child_key); return nullptr;}
    QtFirebaseDbQuery* endAt(QVariant order_value){Q_UNUSED(order_value); return nullptr;}
    QtFirebaseDbQuery* endAt(QVariant order_value, const QString& child_key){Q_UNUSED(order_value); Q_UNUSED(child_key); return nullptr;}
    QtFirebaseDbQuery* equalTo(QVariant order_value){Q_UNUSED(order_value); return nullptr;}
    QtFirebaseDbQuery* equalTo(QVariant order_value, const QString& child_key){Q_UNUSED(order_value); Q_UNUSED(child_key); return nullptr;}
    QtFirebaseDbQuery* limitToFirst(size_t limit){Q_UNUSED(limit); return nullptr;}
    QtFirebaseDbQuery* limitToLast(size_t limit){Q_UNUSED(limit); return nullptr;}

    //State
    bool isRunning() const{return false;}
    int errorId() const{return 0;}
    bool hasError() const{return false;}
    QString errorMsg() const{return QString();}

    //Data access
    QString childKey() const{return QString();}
    QtFirebaseDataSnapshot* getSnapshot(){return nullptr;}
signals:
    void completed(bool success);
    void runningChanged();
    void snapshotChanged();

};

#endif //QTFIREBASE_BUILD_DATABASE

#endif // QTFIREBASEAUTH_H

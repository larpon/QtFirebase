#ifndef QTFIREBASE_DATABASE_H
#define QTFIREBASE_DATABASE_H

#include "qtfirebaseservice.h"
#include "firebase/database.h"
#include <QMutex>

#ifdef QTFIREBASE_BUILD_DATABASE
#include "src/qtfirebase.h"
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
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    enum Error
    {
        ErrorNone = firebase::database::kErrorNone,
        ErrorDisconnected = firebase::database::kErrorDisconnected,
        ErrorExpiredToken = firebase::database::kErrorExpiredToken,
        ErrorInvalidToken = firebase::database::kErrorInvalidToken,
        ErrorMaxRetries = firebase::database::kErrorMaxRetries,
        ErrorNetworkError = firebase::database::kErrorNetworkError,
        ErrorOperationFailed = firebase::database::kErrorOperationFailed,
        ErrorOverriddenBySet = firebase::database::kErrorOverriddenBySet,
        ErrorPermissionDenied = firebase::database::kErrorPermissionDenied,
        ErrorUnavailable = firebase::database::kErrorUnavailable,
        ErrorUnknownError = firebase::database::kErrorUnknownError,
        ErrorWriteCanceled = firebase::database::kErrorWriteCanceled,
        ErrorInvalidVariantType = firebase::database::kErrorInvalidVariantType,
        ErrorConflictingOperationInProgress = firebase::database::kErrorConflictingOperationInProgress,
        ErrorTransactionAbortedByUser = firebase::database::kErrorTransactionAbortedByUser,
    };
    Q_ENUM(Error)

    //TODO implement database related functions
    //(not important to write/read data for now but useful for extended control)
    //https://firebase.google.com/docs/reference/cpp/class/firebase/database/database
private:
    explicit QtFirebaseDatabase(QObject *parent = nullptr);
    void init() override;
    void onFutureEvent(QString eventId, firebase::FutureBase future) override;

    QString futureKey(QtFirebaseDatabaseRequest* request) const;
    QtFirebaseDatabaseRequest* request(const QString& futureKey) const;
    void addFuture(QString requestId, QtFirebaseDatabaseRequest* request, firebase::FutureBase future);
    void unregisterRequest(QtFirebaseDatabaseRequest* request);
    QString prefix() const;

private:
    static QtFirebaseDatabase* self;
    Q_DISABLE_COPY(QtFirebaseDatabase)

    firebase::database::Database* m_db;
    QMap<QString, QtFirebaseDatabaseRequest*> m_requests;
    QMutex m_futureMutex;

    friend class QtFirebaseDatabaseRequest;
};

class QtFirebaseDataSnapshot: public QObject
{
    Q_OBJECT
public:
    QtFirebaseDataSnapshot(const firebase::database::DataSnapshot& snapshot);
public slots:
    bool exists() const;
    QString key() const;
    QVariant value() const;
    QByteArray jsonString() const;
    bool hasChildren() const;
    bool valid() const;

    //not implemented firebase functions
    //
    //currently can be accessed through qt json support (except priority), like
    //QJsonDocument doc = QJsonDocument::fromVariant(request->getSnapshot().value());
    //

    /*
    QtFirebaseDataSnapshot* child(const QString& path);
    size_t childrenCount() const;
    QtFirebaseDataSnapshot getChild(int i);
    QVariant priority() const;
    bool hasChild(const QString& path) const;
    */
private:
    const firebase::database::DataSnapshot& m_snapshot;
};

class QtFirebaseDatabaseRequest;
class QtFirebaseDatabaseQuery: public QObject
{
    Q_OBJECT
public:
    QtFirebaseDatabaseQuery();
public slots:
    QtFirebaseDatabaseQuery* orderByKey();
    QtFirebaseDatabaseQuery* orderByValue();
    QtFirebaseDatabaseQuery* orderByChild(const QString& path);
    QtFirebaseDatabaseQuery* orderByPriority();
    QtFirebaseDatabaseQuery* startAt(QVariant order_value);
    QtFirebaseDatabaseQuery* startAt(QVariant order_value, const QString& child_key);
    QtFirebaseDatabaseQuery* endAt(QVariant order_value);
    QtFirebaseDatabaseQuery* endAt(QVariant order_value, const QString& child_key);
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value);
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value, const QString& child_key);
    QtFirebaseDatabaseQuery* limitToFirst(size_t limit);
    QtFirebaseDatabaseQuery* limitToLast(size_t limit);
    void exec();
signals:
    void run();
private:
    void clear();
    bool valid() const;
    QtFirebaseDatabaseQuery* setQuery(const firebase::database::Query& query);
    firebase::database::Query& query();

    bool m_valid;
    firebase::database::Query m_query;
    friend class QtFirebaseDatabaseRequest;
};

class QtFirebaseDatabaseRequest: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QtFirebaseDataSnapshot* snapshot READ snapshot NOTIFY snapshotChanged)
public:
    QtFirebaseDatabaseRequest();
    ~QtFirebaseDatabaseRequest();
public slots:
    //Data request
    QtFirebaseDatabaseRequest* child(const QString& path = QString());
    QtFirebaseDatabaseRequest* pushChild();
    void setValue(const QVariant& value);
    void exec();
    void updateTree(const QVariant& tree);
    void remove();

    //Filters
    QtFirebaseDatabaseQuery* orderByKey();
    QtFirebaseDatabaseQuery* orderByValue();
    QtFirebaseDatabaseQuery* orderByChild(const QString& path);
    QtFirebaseDatabaseQuery* orderByPriority();
    QtFirebaseDatabaseQuery* startAt(QVariant order_value);
    QtFirebaseDatabaseQuery* startAt(QVariant order_value, const QString& child_key);
    QtFirebaseDatabaseQuery* endAt(QVariant order_value);
    QtFirebaseDatabaseQuery* endAt(QVariant order_value, const QString& child_key);
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value);
    QtFirebaseDatabaseQuery* equalTo(QVariant order_value, const QString& child_key);
    QtFirebaseDatabaseQuery* limitToFirst(size_t limit);
    QtFirebaseDatabaseQuery* limitToLast(size_t limit);

    //State
    bool running() const;
    int errorId() const;
    bool hasError() const;
    QString errorMsg() const;

    //Data access
    QString childKey() const;
    QtFirebaseDataSnapshot* snapshot();
public:
    void onFutureEvent(QString eventId, firebase::FutureBase future);
signals:
    void completed(bool success);
    void runningChanged();
    void snapshotChanged();
private slots:
    void onRun();
private:
    void setComplete(bool value);
    void setError(int errId, const QString& msg = QString());
    void clearError();
    QtFirebaseDatabaseQuery m_query;
    bool m_inComplexRequest;
    QtFirebaseDataSnapshot* m_snapshot;
    firebase::database::DatabaseReference m_dbRef;
    QString m_action;
    bool m_complete;
    QString m_pushChildKey;
    int m_errId;
    QString m_errMsg;
};

#endif //QTFIREBASE_BUILD_DATABASE

#endif // QTFIREBASE_DATABASE_H

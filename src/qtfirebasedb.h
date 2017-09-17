#ifndef QTFIREBASEDATABASE_H
#define QTFIREBASEDATABASE_H

#include "qtfirebaseservice.h"
#include "firebase/database.h"
#include <QMutex>

#ifdef QTFIREBASE_BUILD_DATABASE
#include "src/qtfirebase.h"
#if defined(qFirebaseDb)
#undef qFirebaseDb
#endif
#define qFirebaseDb (static_cast<QtFirebaseDb*>(QtFirebaseDb::instance()))

class QtFirebaseDbRequest;
class QtFirebaseDb : public QtFirebaseService
{
    Q_OBJECT
    typedef QSharedPointer<QtFirebaseDb> Ptr;
public:
    static QtFirebaseDb* instance() {
        if(self == 0) {
            self = new QtFirebaseDb(0);
            qDebug() << self << "::QtFirebaseDb" << "singleton";
        }
        return self;
    }

    enum Error
    {
        kErrorNone = firebase::database::kErrorNone,
        kErrorDisconnected = firebase::database::kErrorDisconnected,
        kErrorExpiredToken = firebase::database::kErrorExpiredToken,
        kErrorInvalidToken = firebase::database::kErrorInvalidToken,
        kErrorMaxRetries = firebase::database::kErrorMaxRetries,
        kErrorNetworkError = firebase::database::kErrorNetworkError,
        kErrorOperationFailed = firebase::database::kErrorOperationFailed,
        kErrorOverriddenBySet = firebase::database::kErrorOverriddenBySet,
        kErrorPermissionDenied = firebase::database::kErrorPermissionDenied,
        kErrorUnavailable = firebase::database::kErrorUnavailable,
        kErrorUnknownError = firebase::database::kErrorUnknownError,
        kErrorWriteCanceled = firebase::database::kErrorWriteCanceled,
        kErrorInvalidVariantType = firebase::database::kErrorInvalidVariantType,
        kErrorConflictingOperationInProgress = firebase::database::kErrorConflictingOperationInProgress,
        kErrorTransactionAbortedByUser = firebase::database::kErrorTransactionAbortedByUser,
    };
    Q_ENUM(Error)

    //TODO implement database related functions
    //(not important to write/read data for now but useful for extended control)
private:
    explicit QtFirebaseDb(QObject *parent = 0);
    void init() override;
    void onFutureEvent(QString eventId, firebase::FutureBase future) override;

    QString getFutureKey(QtFirebaseDbRequest* request) const;
    QtFirebaseDbRequest* getRequest(const QString& futureKey) const;
    void addFuture(QString requestId, QtFirebaseDbRequest* request, firebase::FutureBase future);
    void unregisterRequest(QtFirebaseDbRequest* request);
    QString prefix() const;
private:
    static QtFirebaseDb* self;
    Q_DISABLE_COPY(QtFirebaseDb)

    firebase::database::Database* m_db;
    QMap<QString, QtFirebaseDbRequest*> m_requests;
    QMutex m_futureMutex;

    friend class QtFirebaseDbRequest;
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
    bool isValid() const;

    //not implemented firebase functions
    //
    //currently can be accessed through qt json support (except priority), like
    //QJsonDocument doc = QJsonDocument::fromVariant(request->getSnapshot().value());
    //

    /*QtFirebaseDataSnapshot* child(const QString& path);
    /*size_t childrenCount() const;
    QtFirebaseDataSnapshot getChild(int i);
    QVariant priority() const;
    bool hasChild(const QString& path) const;
    */
private:
    const firebase::database::DataSnapshot& m_snapshot;
};

class QtFirebaseDbRequest;
class QtFirebaseDbQuery: public QObject
{
    Q_OBJECT
public:
    QtFirebaseDbQuery();
public slots:
    QtFirebaseDbQuery* orderByKey();
    QtFirebaseDbQuery* orderByValue();
    QtFirebaseDbQuery* orderByChild(const QString& path);
    QtFirebaseDbQuery* orderByPriority();
    QtFirebaseDbQuery* startAt(QVariant order_value);
    QtFirebaseDbQuery* startAt(QVariant order_value, const QString& child_key);
    QtFirebaseDbQuery* endAt(QVariant order_value);
    QtFirebaseDbQuery* endAt(QVariant order_value, const QString& child_key);
    QtFirebaseDbQuery* equalTo(QVariant order_value);
    QtFirebaseDbQuery* equalTo(QVariant order_value, const QString& child_key);
    QtFirebaseDbQuery* limitToFirst(size_t limit);
    QtFirebaseDbQuery* limitToLast(size_t limit);
    void getValue();
signals:
    void queryRun();
private:
    void clear();
    bool isValid();
    QtFirebaseDbQuery* setQuery(const firebase::database::Query& query);
    firebase::database::Query& query();

    bool m_valid;
    firebase::database::Query m_query;
    friend class QtFirebaseDbRequest;
};

class QtFirebaseDbRequest: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(QtFirebaseDataSnapshot* snapshot READ getSnapshot NOTIFY snapshotChanged)
public:
    QtFirebaseDbRequest();
    ~QtFirebaseDbRequest();
public slots:
    //Data request
    QtFirebaseDbRequest* child(const QString& path = QString());
    QtFirebaseDbRequest* pushChild();
    void setValue(const QVariant& value);
    void getValue();
    void updateTree(const QVariant& tree);
    void remove();

    //Filters
    QtFirebaseDbQuery* orderByKey();
    QtFirebaseDbQuery* orderByValue();
    QtFirebaseDbQuery* orderByChild(const QString& path);
    QtFirebaseDbQuery* orderByPriority();
    QtFirebaseDbQuery* startAt(QVariant order_value);
    QtFirebaseDbQuery* startAt(QVariant order_value, const QString& child_key);
    QtFirebaseDbQuery* endAt(QVariant order_value);
    QtFirebaseDbQuery* endAt(QVariant order_value, const QString& child_key);
    QtFirebaseDbQuery* equalTo(QVariant order_value);
    QtFirebaseDbQuery* equalTo(QVariant order_value, const QString& child_key);
    QtFirebaseDbQuery* limitToFirst(size_t limit);
    QtFirebaseDbQuery* limitToLast(size_t limit);

    //State
    bool isRunning() const;
    int errorId() const;
    bool hasError() const;
    QString errorMsg() const;

    //Data access
    QString childKey() const;
    QtFirebaseDataSnapshot* getSnapshot();
public:
    void onFutureEvent(QString eventId, firebase::FutureBase future);
signals:
    void completed(bool success);
    void runningChanged();
    void snapshotChanged();
private slots:
    void onQueryRun();
private:
    void setComplete(bool value);
    void setError(int errId, const QString& msg = QString());
    void clearError();
    QtFirebaseDbQuery m_query;
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

#endif // QTFIREBASEAUTH_H

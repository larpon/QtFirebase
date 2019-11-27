#include "qtfirebasedatabase.h"
#include <QJsonDocument>
#include <QJsonObject>
namespace db = ::firebase::database;

QtFirebaseDatabase* QtFirebaseDatabase::self = nullptr;

QtFirebaseDatabase::QtFirebaseDatabase(QObject *parent)
    : QtFirebaseService(parent)
    , m_db(nullptr)
    , m_requests()
    , m_futureMutex()
{
    startInit();
}

void QtFirebaseDatabase::init()
{
    if(!qFirebase->ready()) {
        qDebug() << self << "::init" << "base not ready";
        return;
    }

    if(!ready() && !initializing()) {
        setInitializing(true);
        m_db = db::Database::GetInstance(qFirebase->firebaseApp());
        qDebug() << self << "::init" << "native initialized";
        setInitializing(false);
        setReady(true);
    }
}

void QtFirebaseDatabase::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    qDebug() << self << "::onFutureEvent" << eventId;

    QMutexLocker locker(&m_futureMutex);
    QMap<QString, QtFirebaseDatabaseRequest*>::iterator it = m_requests.find(eventId);
    if(it!=m_requests.end() && it.value()!=nullptr)
    {
        QtFirebaseDatabaseRequest* request = it.value();
        m_requests.erase(it);
        QString id = eventId.right(eventId.size()-prefix().size());
        request->onFutureEvent(id, future);
    }
    else
    {
        qDebug() << this << "::onFutureEvent internal error, no request object found";
    }
}

QtFirebaseDatabaseRequest *QtFirebaseDatabase::request(const QString &futureKey) const
{
    auto it = m_requests.find(futureKey);
    return it!=m_requests.end() ? *it : nullptr;
}

void QtFirebaseDatabase::addFuture(QString requestId, QtFirebaseDatabaseRequest *request, firebase::FutureBase future)
{
    QString futureKey = prefix() + requestId;
    qFirebase->addFuture(futureKey, future);
    m_requests[futureKey] = request;
}

void QtFirebaseDatabase::unregisterRequest(QtFirebaseDatabaseRequest *request)
{
    QMutexLocker locker(&m_futureMutex);
    for(QMap<QString, QtFirebaseDatabaseRequest*>::iterator it = m_requests.begin();it!=m_requests.end();++it)
    {
        if(it.value() == request)
        {
            m_requests.erase(it);
            break;
        }
    }
}

QString QtFirebaseDatabase::prefix() const
{
    return __QTFIREBASE_ID + QStringLiteral(".db.");
}

//====================QtFirebaseDatabaseQuery=====================/

QtFirebaseDatabaseQuery::QtFirebaseDatabaseQuery():
    m_valid(false)
{

}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::orderByKey()
{
    m_query = m_query.OrderByKey();
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::orderByValue()
{
    m_query = m_query.OrderByValue();
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::orderByChild(const QString &path)
{
    m_query = m_query.OrderByChild(path.toUtf8().constData());
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::orderByPriority()
{
    m_query = m_query.OrderByPriority();
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::startAt(QVariant order_value)
{
    m_query = m_query.StartAt(QtFirebaseService::fromQtVariant(order_value));
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::startAt(QVariant order_value, const QString &child_key)
{
    m_query = m_query.StartAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData());
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::endAt(QVariant order_value)
{
    m_query = m_query.EndAt(QtFirebaseService::fromQtVariant(order_value));
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::endAt(QVariant order_value, const QString &child_key)
{
    m_query = m_query.EndAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData());
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::equalTo(QVariant order_value)
{
    m_query = m_query.EqualTo(QtFirebaseService::fromQtVariant(order_value));
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::equalTo(QVariant order_value, const QString &child_key)
{
    m_query = m_query.EqualTo(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData());
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::limitToFirst(size_t limit)
{
    m_query = m_query.LimitToFirst(limit);
    return this;
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseQuery::limitToLast(size_t limit)
{
    m_query = m_query.LimitToLast(limit);
    return this;
}

void QtFirebaseDatabaseQuery::exec()
{
    emit run();
}

void QtFirebaseDatabaseQuery::clear()
{
    m_valid = false;
}

bool QtFirebaseDatabaseQuery::valid() const
{
    return m_valid;
}

QtFirebaseDatabaseQuery* QtFirebaseDatabaseQuery::setQuery(const firebase::database::Query &query)
{
    m_valid = true;
    m_query = query;
    return this;
}

firebase::database::Query &QtFirebaseDatabaseQuery::query()
{
    return m_query;
}

//================QtFirebaseDatabaseRequest===================

namespace DatabaseActions {
    const QString Set = QStringLiteral("set");
    const QString Get = QStringLiteral("get");
    const QString Update = QStringLiteral("update");
    const QString Remove = QStringLiteral("remove");
}

QtFirebaseDatabaseRequest::QtFirebaseDatabaseRequest():
    m_inComplexRequest(false)
    ,m_snapshot(nullptr)
    ,m_complete(true)
{
    clearError();
    connect(&m_query, SIGNAL(run()),this,SLOT(onRun()));
}

QtFirebaseDatabaseRequest::~QtFirebaseDatabaseRequest()
{
    qFirebaseDatabase->unregisterRequest(this);
    if(m_snapshot!=nullptr)
        delete m_snapshot;
}

QtFirebaseDatabaseRequest* QtFirebaseDatabaseRequest::child(const QString &path)
{
    if(!running())
    {
        if(!m_inComplexRequest)
        {
            clearError();
            m_inComplexRequest = true;
        }
        m_pushChildKey.clear();
        if(path.isEmpty())
        {
            m_dbRef = qFirebaseDatabase->m_db->GetReference().GetRoot();
        }
        else
        {
            m_dbRef = qFirebaseDatabase->m_db->GetReference().GetRoot().Child(path.toUtf8().constData());
        }
    }
    return this;
}

QtFirebaseDatabaseRequest *QtFirebaseDatabaseRequest::pushChild()
{
    if(m_inComplexRequest && !running())
    {
        m_pushChildKey = m_dbRef.PushChild().key();
    }
    return this;
}

void QtFirebaseDatabaseRequest::remove()
{
    if(m_inComplexRequest && !running())
    {
        m_inComplexRequest = false;
        setComplete(false);
        firebase::Future<void> future = m_dbRef.RemoveValue();
        qFirebaseDatabase->addFuture(DatabaseActions::Remove, this, future);
    }
}

void QtFirebaseDatabaseRequest::setValue(const QVariant &value)
{
    if(m_inComplexRequest && !running())
    {
        m_inComplexRequest = false;
        setComplete(false);
        if(!m_pushChildKey.isEmpty())
        {
            m_dbRef = m_dbRef.Child(m_pushChildKey.toUtf8().constData());
        }
        firebase::Future<void> future = m_dbRef.SetValue(QtFirebaseService::fromQtVariant(value));
        qFirebaseDatabase->addFuture(DatabaseActions::Set, this, future);
    }
}

void QtFirebaseDatabaseRequest::exec()
{
    if(m_inComplexRequest && !running())
    {
        m_inComplexRequest = false;
        setComplete(false);
        if(m_query.valid())
        {
            firebase::Future<firebase::database::DataSnapshot> future = m_query.query().GetValue();
            qFirebaseDatabase->addFuture(DatabaseActions::Get, this, future);
            m_query.clear();
        }
        else
        {
            firebase::Future<firebase::database::DataSnapshot> future = m_dbRef.GetValue();
            qFirebaseDatabase->addFuture(DatabaseActions::Get, this, future);
        }
    }
}

void QtFirebaseDatabaseRequest::updateTree(const QVariant &tree)
{
    clearError();
    setComplete(false);
    QJsonDocument doc = QJsonDocument::fromJson(tree.toString().toUtf8());
    QVariant v(doc.object().toVariantMap());
    firebase::Variant vfb = QtFirebaseService::fromQtVariant(v);
    firebase::Future<void> future = qFirebaseDatabase->m_db->GetReference().UpdateChildren(vfb);
    qFirebaseDatabase->addFuture(DatabaseActions::Update, this, future);
}

int QtFirebaseDatabaseRequest::errorId() const
{
    return m_errId;
}

bool QtFirebaseDatabaseRequest::hasError() const
{
    return m_errId != QtFirebaseDatabase::ErrorNone;
}

QString QtFirebaseDatabaseRequest::errorMsg() const
{
    return m_errMsg;
}

QString QtFirebaseDatabaseRequest::childKey() const
{
    return m_pushChildKey;
}

QtFirebaseDataSnapshot *QtFirebaseDatabaseRequest::snapshot()
{
    return m_snapshot;
}

void QtFirebaseDatabaseRequest::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent " << "ERROR: Action failed with status: " << future.status();
        setError(QtFirebaseDatabase::ErrorUnknownError);
    }
    else if (future.error() != firebase::database::kErrorNone)
    {
        qDebug() << this << "::onFutureEvent Error occured in result:" << future.error() << future.error_message();
        setError(future.error(), future.error_message());
    }
    else if(eventId == DatabaseActions::Get)
    {
        const firebase::database::DataSnapshot* snapshot = ::result<firebase::database::DataSnapshot>(future.result_void());
        if(m_snapshot)
        {
            delete m_snapshot;
        }
        m_snapshot = new QtFirebaseDataSnapshot(*snapshot);
        clearError();
    }
    m_query.clear();
    setComplete(true);
}

void QtFirebaseDatabaseRequest::setComplete(bool value)
{
    if(m_complete!=value)
    {
        m_complete = value;
        emit runningChanged();
        if(m_complete)
        {
            emit completed(m_errId == QtFirebaseDatabase::ErrorNone);
        }
    }
}

void QtFirebaseDatabaseRequest::setError(int errId, const QString &msg)
{
    m_errId = errId;
    m_errMsg = msg;
}

void QtFirebaseDatabaseRequest::clearError()
{
    setError(QtFirebaseDatabase::ErrorNone);
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::orderByKey()
{
    return m_query.valid() ?
        m_query.orderByKey() :
        m_query.setQuery(m_dbRef.OrderByKey());
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::orderByValue()
{
    return m_query.valid() ?
        m_query.orderByValue() :
        m_query.setQuery(m_dbRef.OrderByValue());
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::orderByChild(const QString &path)
{
    return m_query.valid() ?
        m_query.orderByChild(path):
        m_query.setQuery(m_dbRef.OrderByChild(path.toUtf8().constData()));
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::orderByPriority()
{
    return m_query.valid() ?
        m_query.orderByPriority() :
        m_query.setQuery(m_dbRef.OrderByPriority());
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::startAt(QVariant order_value)
{
    return m_query.valid() ?
        m_query.startAt(order_value) :
        m_query.setQuery(m_dbRef.StartAt(QtFirebaseService::fromQtVariant(order_value)));

}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::startAt(QVariant order_value, const QString &child_key)
{
    return m_query.valid() ?
        m_query.startAt(order_value, child_key) :
        m_query.setQuery(m_dbRef.StartAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData()));
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::endAt(QVariant order_value)
{
    return m_query.valid() ?
        m_query.endAt(order_value) :
        m_query.setQuery(m_dbRef.EndAt(QtFirebaseService::fromQtVariant(order_value)));
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::endAt(QVariant order_value, const QString &child_key)
{
    return m_query.valid() ?
        m_query.endAt(order_value, child_key) :
        m_query.setQuery(m_dbRef.EndAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData()));
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::equalTo(QVariant order_value)
{
    return m_query.valid() ?
        m_query.equalTo(order_value) :
        m_query.setQuery(m_dbRef.EqualTo(QtFirebaseService::fromQtVariant(order_value)));
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::equalTo(QVariant order_value, const QString &child_key)
{
    return m_query.valid() ?
        m_query.equalTo(order_value, child_key) :
        m_query.setQuery(m_dbRef.EqualTo(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData()));
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::limitToFirst(size_t limit)
{
    return m_query.valid() ?
        m_query.limitToFirst(limit) :
        m_query.setQuery(m_dbRef.LimitToFirst(limit));
}

QtFirebaseDatabaseQuery *QtFirebaseDatabaseRequest::limitToLast(size_t limit)
{
    return m_query.valid() ?
        m_query.limitToLast(limit) :
                m_query.setQuery(m_dbRef.LimitToLast(limit));
}

bool QtFirebaseDatabaseRequest::running() const
{
    return !m_complete;
}

void QtFirebaseDatabaseRequest::onRun()
{
    exec();
}

//================QtFirebaseDataSnapshot===================

QtFirebaseDataSnapshot::QtFirebaseDataSnapshot(const firebase::database::DataSnapshot &snapshot):
    m_snapshot(snapshot)
{

}

bool QtFirebaseDataSnapshot::exists() const
{
    return m_snapshot.exists();
}

QString QtFirebaseDataSnapshot::key() const
{
    return m_snapshot.key();
}

QVariant QtFirebaseDataSnapshot::value() const
{

    return QtFirebaseService::fromFirebaseVariant(m_snapshot.value());
}

QByteArray QtFirebaseDataSnapshot::jsonString() const
{
    //Limitation: order queries will not work because of
    //missing order when getting data through value object and not from snapshot hierarchy
    //TODO: Improve JSON translation using snapshot hierarchy
    QJsonDocument doc = QJsonDocument::fromVariant(value());
    return doc.toJson();
}

bool QtFirebaseDataSnapshot::hasChildren() const
{
    return m_snapshot.has_children();
}

bool QtFirebaseDataSnapshot::valid() const
{
    return m_snapshot.is_valid();
}

#include "qtfirebasedb.h"
#include <QJsonDocument>
#include <QJsonObject>
namespace db = ::firebase::database;

QtFirebaseDb* QtFirebaseDb::self = 0;

QtFirebaseDb::QtFirebaseDb(QObject *parent) : QtFirebaseService(parent),
    m_db(nullptr)
{
    startInit();
}

void QtFirebaseDb::init()
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

void QtFirebaseDb::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    qDebug()<<self<<"::onFutureEvent"<<eventId;

    QMutexLocker locker(&m_futureMutex);
    QMap<QString, QtFirebaseDbRequest*>::iterator it = m_requests.find(eventId);
    if(it!=m_requests.end() && it.value()!=nullptr)
    {
        QtFirebaseDbRequest* request = it.value();
        m_requests.erase(it);
        QString id = eventId.right(eventId.size()-prefix().size());
        request->onFutureEvent(id, future);
    }
    else
    {
        qDebug() << this << "::onFutureEvent internal error, no request object found";
    }
}

QtFirebaseDbRequest *QtFirebaseDb::getRequest(const QString &futureKey) const
{
    auto it = m_requests.find(futureKey);
    return it!=m_requests.end() ? *it : nullptr;
}

void QtFirebaseDb::addFuture(QString requestId, QtFirebaseDbRequest *request, firebase::FutureBase future)
{
    QString futureKey = prefix() + requestId;
    qFirebase->addFuture(futureKey, future);
    m_requests[futureKey] = request;
}

void QtFirebaseDb::unregisterRequest(QtFirebaseDbRequest *request)
{
    QMutexLocker locker(&m_futureMutex);
    for(QMap<QString, QtFirebaseDbRequest*>::iterator it = m_requests.begin();it!=m_requests.end();++it)
    {
        if(it.value() == request)
        {
            m_requests.erase(it);
            break;
        }
    }
}

QString QtFirebaseDb::prefix() const
{
    return __QTFIREBASE_ID + ".db.";
}

//====================QtFirebaseDbQuery=====================/

QtFirebaseDbQuery::QtFirebaseDbQuery():
    m_valid(false)
{

}

QtFirebaseDbQuery *QtFirebaseDbQuery::orderByKey()
{
    m_query = m_query.OrderByKey();
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::orderByValue()
{
    m_query = m_query.OrderByValue();
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::orderByChild(const QString &path)
{
    m_query = m_query.OrderByChild(path.toUtf8().constData());
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::orderByPriority()
{
    m_query = m_query.OrderByPriority();
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::startAt(QVariant order_value)
{
    m_query = m_query.StartAt(QtFirebaseService::fromQtVariant(order_value));
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::startAt(QVariant order_value, const QString &child_key)
{
    m_query = m_query.StartAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData());
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::endAt(QVariant order_value)
{
    m_query = m_query.EndAt(QtFirebaseService::fromQtVariant(order_value));
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::endAt(QVariant order_value, const QString &child_key)
{
    m_query = m_query.EndAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData());
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::equalTo(QVariant order_value)
{
    m_query = m_query.EqualTo(QtFirebaseService::fromQtVariant(order_value));
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::equalTo(QVariant order_value, const QString &child_key)
{
    m_query = m_query.EqualTo(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData());
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::limitToFirst(size_t limit)
{
    m_query = m_query.LimitToFirst(limit);
    return this;
}

QtFirebaseDbQuery *QtFirebaseDbQuery::limitToLast(size_t limit)
{
    m_query = m_query.LimitToLast(limit);
    return this;
}

void QtFirebaseDbQuery::getValue()
{
    emit queryRun();
}

void QtFirebaseDbQuery::clear()
{
    m_valid = false;
}

bool QtFirebaseDbQuery::isValid()
{
    return m_valid;
}

QtFirebaseDbQuery* QtFirebaseDbQuery::setQuery(const firebase::database::Query &query)
{
    m_valid = true;
    m_query = query;
    return this;
}

firebase::database::Query &QtFirebaseDbQuery::query()
{
    return m_query;
}

//================QtFirebaseDbRequest===================

namespace DbActions{
    const QString Set = "set";
    const QString Get = "get";
    const QString Update = "update";
    const QString Remove = "remove";
}

QtFirebaseDbRequest::QtFirebaseDbRequest():
    m_inComplexRequest(false)
    ,m_snapshot(nullptr)
    ,m_complete(true)
{
    clearError();
    connect(&m_query, SIGNAL(queryRun()),this,SLOT(onQueryRun()));
}

QtFirebaseDbRequest::~QtFirebaseDbRequest()
{
    qFirebaseDb->unregisterRequest(this);
    if(m_snapshot!=nullptr)
        delete m_snapshot;
}

QtFirebaseDbRequest* QtFirebaseDbRequest::child(const QString &path)
{
    if(!isRunning())
    {
        if(!m_inComplexRequest)
        {
            clearError();
            m_inComplexRequest = true;
        }
        m_pushChildKey.clear();
        if(path.isEmpty())
        {
            m_dbRef = qFirebaseDb->m_db->GetReference().GetRoot();
        }
        else
        {
            m_dbRef = qFirebaseDb->m_db->GetReference().GetRoot().Child(path.toUtf8().constData());
        }
    }
    return this;
}

QtFirebaseDbRequest *QtFirebaseDbRequest::pushChild()
{
    if(m_inComplexRequest && !isRunning())
    {
        m_pushChildKey = m_dbRef.PushChild().key();
    }
    return this;
}

void QtFirebaseDbRequest::remove()
{
    if(m_inComplexRequest && !isRunning())
    {
        m_inComplexRequest = false;
        setComplete(false);
        firebase::Future<void> future = m_dbRef.RemoveValue();
        qFirebaseDb->addFuture(DbActions::Remove, this, future);
    }
}

void QtFirebaseDbRequest::setValue(const QVariant &value)
{
    if(m_inComplexRequest && !isRunning())
    {
        m_inComplexRequest = false;
        setComplete(false);
        if(!m_pushChildKey.isEmpty())
        {
            m_dbRef = m_dbRef.Child(m_pushChildKey.toUtf8().constData());
        }
        firebase::Future<void> future = m_dbRef.SetValue(QtFirebaseService::fromQtVariant(value));
        qFirebaseDb->addFuture(DbActions::Set, this, future);
    }
}

void QtFirebaseDbRequest::getValue()
{
    if(m_inComplexRequest && !isRunning())
    {
        m_inComplexRequest = false;
        setComplete(false);
        if(m_query.isValid())
        {
            firebase::Future<firebase::database::DataSnapshot> future = m_query.query().GetValue();
            qFirebaseDb->addFuture(DbActions::Get, this, future);
            m_query.clear();
        }
        else
        {
            firebase::Future<firebase::database::DataSnapshot> future = m_dbRef.GetValue();
            qFirebaseDb->addFuture(DbActions::Get, this, future);
        }
    }
}

void QtFirebaseDbRequest::updateTree(const QVariant &tree)
{
    clearError();
    setComplete(false);
    QJsonDocument doc = QJsonDocument::fromJson(tree.toString().toUtf8());
    QVariant v(doc.object().toVariantMap());
    firebase::Variant vfb = QtFirebaseService::fromQtVariant(v);
    firebase::Future<void> future = qFirebaseDb->m_db->GetReference().UpdateChildren(vfb);
    qFirebaseDb->addFuture(DbActions::Update, this, future);
}

int QtFirebaseDbRequest::errorId() const
{
    return m_errId;
}

bool QtFirebaseDbRequest::hasError() const
{
    return m_errId != QtFirebaseDb::kErrorNone;
}

QString QtFirebaseDbRequest::errorMsg() const
{
    return m_errMsg;
}

QString QtFirebaseDbRequest::childKey() const
{
    return m_pushChildKey;
}

QtFirebaseDataSnapshot *QtFirebaseDbRequest::getSnapshot()
{
    return m_snapshot;
}

void QtFirebaseDbRequest::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent " << "ERROR: Action failed with status: " << future.status();
        setError(QtFirebaseDb::kErrorUnknownError);
    }
    else if (future.error() != firebase::database::kErrorNone)
    {
        qDebug()<<this<<"::onFutureEvent Error occured in result:"<<future.error() << future.error_message();
        setError(future.error(), future.error_message());
    }
    else if(eventId == DbActions::Get)
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

void QtFirebaseDbRequest::setComplete(bool value)
{
    if(m_complete!=value)
    {
        m_complete = value;
        emit runningChanged();
        if(m_complete)
        {
            emit completed(m_errId == QtFirebaseDb::kErrorNone);
        }
    }
}

void QtFirebaseDbRequest::setError(int errId, const QString &msg)
{
    m_errId = errId;
    m_errMsg = msg;
}

void QtFirebaseDbRequest::clearError()
{
    setError(QtFirebaseDb::kErrorNone);
}

QtFirebaseDbQuery *QtFirebaseDbRequest::orderByKey()
{
    return m_query.isValid() ?
        m_query.orderByKey() :
        m_query.setQuery(m_dbRef.OrderByKey());
}

QtFirebaseDbQuery *QtFirebaseDbRequest::orderByValue()
{
    return m_query.isValid() ?
        m_query.orderByValue() :
        m_query.setQuery(m_dbRef.OrderByValue());
}

QtFirebaseDbQuery *QtFirebaseDbRequest::orderByChild(const QString &path)
{
    return m_query.isValid() ?
        m_query.orderByChild(path):
        m_query.setQuery(m_dbRef.OrderByChild(path.toUtf8().constData()));
}

QtFirebaseDbQuery *QtFirebaseDbRequest::orderByPriority()
{
    return m_query.isValid() ?
        m_query.orderByPriority() :
        m_query.setQuery(m_dbRef.OrderByPriority());
}

QtFirebaseDbQuery *QtFirebaseDbRequest::startAt(QVariant order_value)
{
    return m_query.isValid() ?
        m_query.startAt(order_value) :
        m_query.setQuery(m_dbRef.StartAt(QtFirebaseService::fromQtVariant(order_value)));

}

QtFirebaseDbQuery *QtFirebaseDbRequest::startAt(QVariant order_value, const QString &child_key)
{
    return m_query.isValid() ?
        m_query.startAt(order_value, child_key) :
        m_query.setQuery(m_dbRef.StartAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData()));
}

QtFirebaseDbQuery *QtFirebaseDbRequest::endAt(QVariant order_value)
{
    return m_query.isValid() ?
        m_query.endAt(order_value) :
        m_query.setQuery(m_dbRef.EndAt(QtFirebaseService::fromQtVariant(order_value)));
}

QtFirebaseDbQuery *QtFirebaseDbRequest::endAt(QVariant order_value, const QString &child_key)
{
    return m_query.isValid() ?
        m_query.endAt(order_value, child_key) :
        m_query.setQuery(m_dbRef.EndAt(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData()));
}

QtFirebaseDbQuery *QtFirebaseDbRequest::equalTo(QVariant order_value)
{
    return m_query.isValid() ?
        m_query.equalTo(order_value) :
        m_query.setQuery(m_dbRef.EqualTo(QtFirebaseService::fromQtVariant(order_value)));
}

QtFirebaseDbQuery *QtFirebaseDbRequest::equalTo(QVariant order_value, const QString &child_key)
{
    return m_query.isValid() ?
        m_query.equalTo(order_value, child_key) :
        m_query.setQuery(m_dbRef.EqualTo(QtFirebaseService::fromQtVariant(order_value), child_key.toUtf8().constData()));
}

QtFirebaseDbQuery *QtFirebaseDbRequest::limitToFirst(size_t limit)
{
    return m_query.isValid() ?
        m_query.limitToFirst(limit) :
        m_query.setQuery(m_dbRef.LimitToFirst(limit));
}

QtFirebaseDbQuery *QtFirebaseDbRequest::limitToLast(size_t limit)
{
    return m_query.isValid() ?
        m_query.limitToLast(limit) :
                m_query.setQuery(m_dbRef.LimitToLast(limit));
}

bool QtFirebaseDbRequest::isRunning() const
{
    return !m_complete;
}

void QtFirebaseDbRequest::onQueryRun()
{
    getValue();
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
    QJsonDocument doc = QJsonDocument::fromVariant(value());
    return doc.toJson();
}

bool QtFirebaseDataSnapshot::hasChildren() const
{
    return m_snapshot.has_children();
}

bool QtFirebaseDataSnapshot::isValid() const
{
    return m_snapshot.is_valid();
}

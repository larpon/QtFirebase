#include "qtfirebasedb.h"
#include <QJsonDocument>
#include <QJsonObject>
namespace db = ::firebase::database;

QtFirebaseDb *QtFirebaseDb::self = 0;

QtFirebaseDb::QtFirebaseDb(QObject *parent) : QtFirebaseService(parent),
    m_db(nullptr)
{
    if(self == 0)
    {
        self = this;
        qDebug() << self << "::QtFirebaseDb" << "singleton";
    }
    startInit();
}

QtFirebaseDb &QtFirebaseDb::child(const QString &path)
{
    return *this;
}

QString QtFirebaseDb::getValue()
{
    return "";
}

void QtFirebaseDb::requestData(const QString &path)
{
    firebase::database::DatabaseReference dbRef = m_db->GetReference();
    /*firebase::Future<firebase::database::DataSnapshot> future = dbRef.GetRoot().Child(path.toUtf8().constData()).GetValue();

    /*QString key = dbRef.GetRoot().Child(path.toUtf8().constData()).PushChild().key();

    QString newPath = "test/";
    newPath+=key;
    firebase::Future<void> future = dbRef.GetRoot().
            Child(newPath.toUtf8().constData()).SetValue("xxx");
    qDebug()<<"PushChild Key:"<<key;*/

    std::map<std::string, firebase::Variant>* map = new std::map<std::string, firebase::Variant>();
    (*map)["test/child1"] = firebase::Variant();
    //(*map)["test/child2"] = "xxx2";
    //(*map)["test/child3/child31"] = "xxx5";
    firebase::Future<void> future = dbRef.UpdateChildren(*map);

    qFirebase->addFuture(__QTFIREBASE_ID + ".db.setvalue", future);
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

    QMutexLocker(&m_futureMutex);
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

    /*if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        return;
    }

    if (future.error() != firebase::database::kErrorNone)
    {
        qDebug()<<this<<"::onFutureEvent Error occured in result:"<<future.error() << future.error_message();
        emit error(future.error(), future.error_message());
        return;
    }

    //if(eventId == __QTFIREBASE_ID+".db.getvalue")
    if(eventId.contains("getvalue"))
    {
        const firebase::database::DataSnapshot* snapshot = ::result<firebase::database::DataSnapshot>(future.result_void());
        QVariant v = fromFirebaseVariant(snapshot->value());
        qDebug()<<"Result:"<<snapshot->key()<<v;
        if(snapshot!=nullptr)
        {
            //emit result(snapshot->key(), fromFirebaseVariant(snapshot->value()));
        }
    }
    else if(eventId == __QTFIREBASE_ID+".db.setvalue")
    {

    }*/

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
    QMutexLocker(&m_futureMutex);
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

QtFirebaseDbRequest::QtFirebaseDbRequest():
    m_complete(true)
{
}

QtFirebaseDbRequest::~QtFirebaseDbRequest()
{
    qFirebaseDb->unregisterRequest(this);
}

QtFirebaseDbRequest* QtFirebaseDbRequest::child(const QString &path)
{
    if(isComplete())
    {
        qDebug()<<"Create reference";
        m_dbRef = qFirebaseDb->m_db->GetReference().GetRoot().Child(path.toUtf8().constData());
        qDebug()<<"ok";
    }
    return this;
}

void QtFirebaseDbRequest::getValue()
{
    //if(m_dbRef.is_valid() && isComplete())
    {
        firebase::Future<firebase::database::DataSnapshot> future = m_dbRef.GetValue();
        qFirebaseDb->addFuture("getvalue", this, future);
    }
}

void QtFirebaseDbRequest::updateTree(const QVariant &tree)
{
    QJsonDocument doc = QJsonDocument::fromJson(tree.toString().toUtf8());

    QVariant v(doc.object().toVariantMap());
    firebase::Future<void> future = m_dbRef.UpdateChildren(QtFirebaseService::fromQtVariant(v));
    qFirebaseDb->addFuture("update", this, future);
}

bool QtFirebaseDbRequest::isComplete() const
{
    return true;
}

QtFirebaseDataSnapshot *QtFirebaseDbRequest::getSnapshot()
{
    return m_snapshot;
}

void QtFirebaseDbRequest::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    m_complete = true;
    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        return;
    }

    if (future.error() != firebase::database::kErrorNone)
    {
        qDebug()<<this<<"::onFutureEvent Error occured in result:"<<future.error() << future.error_message();
        emit error(future.error(), future.error_message());
        return;
    }

    qDebug()<<"QtFirebaseDbRequest::onFutureEvent:"<<eventId;

    if(eventId == "getvalue")
    {
        const firebase::database::DataSnapshot* snapshot = ::result<firebase::database::DataSnapshot>(future.result_void());
        m_snapshot = new QtFirebaseDataSnapshot(snapshot);
        emit completed();
    }
    else
    {
        emit completed();
    }
}


QtFirebaseDataSnapshot::QtFirebaseDataSnapshot(const firebase::database::DataSnapshot *snapshot):
    m_snapshot(snapshot)
{

}

bool QtFirebaseDataSnapshot::exists() const
{
    return m_snapshot!=nullptr ? m_snapshot->exists() : false;
}

QString QtFirebaseDataSnapshot::key() const
{
    return m_snapshot!=nullptr ? m_snapshot->key() : QString();
}

QVariant QtFirebaseDataSnapshot::value() const
{
    return m_snapshot!=nullptr ? QtFirebaseService::fromFirebaseVariant(m_snapshot->value()) : false;
}


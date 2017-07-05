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
#define qFirebaseDb (static_cast<QtFirebaseDb *>(QtFirebaseDb::instance()))

class QtFirebaseDbRequest;
class QtFirebaseDb : public QtFirebaseService
{
    Q_OBJECT
public:
    explicit QtFirebaseDb(QObject *parent = 0);
    static QtFirebaseDb *instance() {
        if(self == 0) {
            self = new QtFirebaseDb(0);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

public slots:
    QtFirebaseDb &child(const QString& path);
    //QtFirebaseDb& pushChild();
    QString getValue();
    void requestData(const QString &path);
    //void requestGet(const QString& path);
    //void requestSet(const QString& path, const QVariant& value);
    //void requestRemove(const QString& path);
    //void requestTransaction(const QString& path, const QVariantList& children);
signals:
    void result(QString path, QVariant value);
    void error(int error, QString message);
private:
    void init() override;
    void onFutureEvent(QString eventId, firebase::FutureBase future) override;

    QString getFutureKey(QtFirebaseDbRequest* request) const;
    QtFirebaseDbRequest* getRequest(const QString& futureKey) const;
    void addFuture(QString requestId, QtFirebaseDbRequest* request, firebase::FutureBase future);
    void unregisterRequest(QtFirebaseDbRequest* request);
    QString prefix() const;

    static QtFirebaseDb *self;
    Q_DISABLE_COPY(QtFirebaseDb)

    firebase::database::Database* m_db;
    QMap<QString, QtFirebaseDbRequest*> m_requests;

    friend class QtFirebaseDbRequest;
    QMutex m_futureMutex;
};

class QtFirebaseDataSnapshot: public QObject
{
    Q_OBJECT
public:
    QtFirebaseDataSnapshot(const firebase::database::DataSnapshot* snapshot = nullptr);
public slots:

    bool exists() const;
    QString key() const;
    QVariant value() const;//TODO map and list, move to json object

    /*QtFirebaseDataSnapshot child(const QString& path) const;
    size_t childrenCount() const;
    bool hasChildren() const;
    QtFirebaseDataSnapshot getChild(int i);
    QVariant priority() const;
    bool hasChild(const QString& path) const;
    bool isValid() const;*/
private:
    const firebase::database::DataSnapshot* m_snapshot;
};

class QtFirebaseDbRequest: public QObject
{
    Q_OBJECT
    //Q_PROPERTY(bool complete READ isComplete NOTIFY completeChanged)
    Q_PROPERTY(QtFirebaseDataSnapshot* snapshot READ getSnapshot NOTIFY snapshotChanged)
    //Q_PROPERTY(QtFirebaseDbRequest* request READ getRequest NOTIFY requestChanged)
public:
    QtFirebaseDbRequest();
    ~QtFirebaseDbRequest();
public slots:
    //Data access
    //QtFirebaseDbRequest& update(const QJsonObject& tree);
    QtFirebaseDbRequest* child(const QString& path);
    //QtFirebaseDbRequest& remove();
    //void setValue(const QVariant& value);
    void getValue();
    void updateTree(const QVariant& tree);

    //Filters

    //Query state
    bool isComplete() const;
    //int errorId() const;
    //QString errorMsg() const;

    QtFirebaseDataSnapshot* getSnapshot();
    //QtFirebaseDbRequest* getRequest();
public:
    void onFutureEvent(QString eventId, firebase::FutureBase future);
signals:
    //void completed(QtFirebaseDataSnapshot snapshot);
    void completed();
    void error(int id, QString message);
    void snapshotChanged();
    void requestChanged();
private:
    QtFirebaseDataSnapshot* m_snapshot;
    firebase::database::DatabaseReference m_dbRef;
    //const firebase::database::DataSnapshot* m_snapshot;
    QString m_action;
    bool m_complete;
};


#endif //QTFIREBASE_BUILD_DATABASE

#endif // QTFIREBASEAUTH_H

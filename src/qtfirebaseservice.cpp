#include "qtfirebaseservice.h"
#include <QJsonObject>
#include <QJsonDocument>
QtFirebaseService::QtFirebaseService(QObject* parent):
    QObject(parent),
    _ready(false),
    _initializing(false)
{
    __QTFIREBASE_ID = QString().sprintf("%8p", this);


}

bool QtFirebaseService::ready() const
{
    return _ready;
}

QVariant QtFirebaseService::fromFirebaseVariant(const firebase::Variant &v)
{
    QString typeName;
    switch(v.type())
    {
        case firebase::Variant::kTypeNull:
            return QVariant();
        case firebase::Variant::kTypeInt64:
            return QVariant(v.int64_value());
        case firebase::Variant::kTypeDouble:
            return QVariant(v.double_value());
        case firebase::Variant::kTypeBool:
            return QVariant(v.bool_value());
        case firebase::Variant::kTypeStaticString:
            return QVariant(v.string_value());
        case firebase::Variant::kTypeMutableString:
            return QVariant(v.mutable_string().c_str());
        case firebase::Variant::kTypeVector:
        {
            typeName = "vector";
            break;
        }
        case firebase::Variant::kTypeMap:
        {
            typeName = "map";
            break;
        }
        case firebase::Variant::kTypeStaticBlob:
        {
            typeName = "static_blob";
            break;
        }
        case firebase::Variant::kTypeMutableBlob:
        {
            typeName = "mutable_blob";
            break;
        }
        default:{
            typeName = "unknown";
        }
    }
    qDebug()<<"QtFirebaseService::fromFirebaseVariant type "<<typeName <<" not supported";
    return QVariant();
}

firebase::Variant QtFirebaseService::fromQtVariant(const QVariant &v)
{

    switch(v.type())
    {
        case QVariant::Bool:{
            return firebase::Variant(v.toBool());
        }
        case QVariant::Int:{
            return firebase::Variant(v.toInt());
        }
        /*case QVariant::UInt:{
            return firebase::Variant(v.toUInt());
        }
        case QVariant::LongLong:{
            return firebase::Variant(v.toLongLong());
        }
        case QVariant::ULongLong:{
            return firebase::Variant(v.toULongLong());
        }*/
        case QVariant::Double:{
            return firebase::Variant(v.toDouble());
        }
        case QVariant::String:{
            return firebase::Variant(v.toString().toUtf8().constData());
        }
        case QVariant::ByteArray:{
            return firebase::Variant(v.toByteArray().constData());
        }
        case QVariant::Map:{
            QVariantMap qtMap = v.toMap();
            std::map<std::string, firebase::Variant> firebaseMap;
            for(QVariantMap::const_iterator it = qtMap.begin();it!=qtMap.end();++it)
            {
                firebaseMap[it.key().toUtf8().constData()] = fromQtVariant(it.value());
            }
            return firebase::Variant(firebaseMap);
        }
        case QVariant::List:{
            break;
        }
        default:{
            break;
        }
    }

    qDebug()<<"QtFirebaseService::fromQtVariant type "<<v.typeName() <<" not supported";
    qDebug()<<v.toJsonObject().toVariantMap();
    qDebug()<<v;
    qDebug()<<v.toString()<<v.toStringList();

    return firebase::Variant();
}

void QtFirebaseService::startInit()
{
    if(qFirebase->ready())
    {
        //Call init outside of constructor, otherwise signal readyChanged not emited
        QTimer::singleShot(500, this, &QtFirebaseService::init);
    }
    else
    {
        connect(qFirebase, &QtFirebase::readyChanged, this, &QtFirebaseService::init);
        qFirebase->requestInit();
    }
    connect(qFirebase, &QtFirebase::futureEvent, this, &QtFirebaseService::onFutureEvent);
}

void QtFirebaseService::setReady(bool value)
{
    qDebug() << this << "::setReady" << value;

    if (_ready != value) {
        _ready = value;
        emit readyChanged();
    }
}

bool QtFirebaseService::initializing() const
{
    return _initializing;
}

void QtFirebaseService::setInitializing(bool value)
{
    _initializing = value;
}

#include "qtfirebaseservice.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <iostream>
using namespace std;

//For debug purposes
bool simpleQtType(const QVariant& v)
{
    QSet<int> simpleTypes;
    simpleTypes<<QVariant::Bool<<QVariant::Int<<QVariant::UInt<<QVariant::LongLong<<QVariant::ULongLong
             <<QVariant::Double<<QVariant::String<<QVariant::ByteArray;
    return simpleTypes.contains(v.type());
}

//For debug purposes
bool simpleFbType(const firebase::Variant& v)
{
    QSet<int> simpleTypes;
    simpleTypes<<firebase::Variant::kTypeBool
               <<firebase::Variant::kTypeDouble
               <<firebase::Variant::kTypeInt64
               <<firebase::Variant::kTypeMutableString
               <<firebase::Variant::kTypeStaticString;

    return simpleTypes.contains(v.type());
}

//For debug purposes
void printQtVariant(const QVariant& v, const QString& tab)
{
    const QString tabKey = QStringLiteral("    ");

    switch(v.type())
    {
        case QVariant::Bool:{
            qDebug() << tab.toUtf8().constData() << v.toBool();
            break;
        }
        case QVariant::Int:{
            qDebug() << tab.toUtf8().constData() << v.toInt();
            break;
        }
        case QVariant::UInt:{
            qDebug() << tab.toUtf8().constData() << v.toUInt();
            break;
        }
        case QVariant::LongLong:{
            qDebug() << tab.toUtf8().constData() << v.toLongLong();
            break;
        }
        case QVariant::ULongLong:{
            qDebug() << tab.toUtf8().constData() << v.toULongLong();
            break;
        }
        case QVariant::Double:{
            qDebug() << tab.toUtf8().constData() << v.toDouble();
            break;
        }
        case QVariant::String:{
            qDebug() << tab.toUtf8().constData() << v.toString().toUtf8().constData();
            break;
        }
        case QVariant::ByteArray:{
            qDebug() << v.toByteArray().constData();
            break;
        }
        case QVariant::Map:{
            QVariantMap map = v.toMap();
            qDebug() << tab.toUtf8().constData() << "{";
            for(QVariantMap::const_iterator it = map.begin();it!=map.end();++it)
            {
                qDebug() << tab.toUtf8().constData() << it.key().toUtf8().constData() << ":";
                printQtVariant(it.value(), tab+tabKey);
            }
            qDebug() << tab.toUtf8().constData() << "}";
            break;
        }
        case QVariant::List:{
            QVariantList lst = v.toList();
            qDebug() << tab.toUtf8().constData() << "[";
            for(QVariantList::const_iterator it = lst.begin();it!=lst.end();++it)
            {
                printQtVariant(*it, tab+tabKey);
            }
            qDebug() << tab.toUtf8().constData() << "]";
            break;

        }
        default:{
            qDebug() << "printQtVariant(): Type:" << v.typeName() << "not supported";
            break;
        }
    }
}

//For debug purposes
void printFbVariant(const firebase::Variant& v, const QString& tab)
{
    const QString tabKey = QStringLiteral("    ");

    switch(v.type())
    {
        case firebase::Variant::kTypeBool:{
            qDebug() << tab.toUtf8().constData() << v.bool_value();
            break;
        }
        case firebase::Variant::kTypeInt64:{
            qDebug() << tab.toUtf8().constData() << v.int64_value();
            break;
        }

        case firebase::Variant::kTypeDouble:{
            qDebug() << tab.toUtf8().constData() << v.double_value();
            break;
        }
        case firebase::Variant::kTypeStaticString:{
            qDebug() << tab.toUtf8().constData() << v.string_value();
            break;
        }
        case firebase::Variant::kTypeMutableString:{
            qDebug() << tab.toUtf8().constData() << v.mutable_string().c_str();
            break;
        }
        case firebase::Variant::kTypeMap:{
            const std::map<firebase::Variant, firebase::Variant>& map = v.map();
            qDebug() << tab.toUtf8().constData() << "{";
            for(std::map<firebase::Variant, firebase::Variant>::const_iterator it = map.begin();it!=map.end();++it)
            {
                firebase::Variant key = it->first;
                if(key.type() == firebase::Variant::kTypeMutableString ||
                        key.type() == firebase::Variant::kTypeStaticString)
                {
                    qDebug() << tab.toUtf8().constData() << it->first.string_value() << ":";
                    printFbVariant(it->second, tab+tabKey);
                }
                else
                {
                    qDebug() << "Input key:" << key.TypeName(v.type());
                    qDebug() << "QtFirebase does not support non string keys";
                    return;
                }
            }
            qDebug() << tab.toUtf8().constData() << "}";
            break;
        }
        case firebase::Variant::kTypeVector:{
            std::vector<firebase::Variant> lst = v.vector();
            qDebug() << tab.toUtf8().constData() << "[";
            for(std::vector<firebase::Variant>::const_iterator it = lst.begin();it!=lst.end();++it)
            {
                printFbVariant(*it, tab+tabKey);
            }
            qDebug() << tab.toUtf8().constData() << "]";
            break;

        }
        default:{
            qDebug() << "printFbVariant(): Type:" << v.TypeName(v.type()) << "not supported";
            break;
        }
    }
}

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
            return QVariant(QString::fromUtf8(v.string_value()));
        case firebase::Variant::kTypeMutableString:
            return QVariant(QString::fromUtf8(v.mutable_string().c_str()));
        case firebase::Variant::kTypeMap:{
            std::map<firebase::Variant, firebase::Variant> srcMap = v.map();
            QVariantMap targetMap;
            for(std::map<firebase::Variant, firebase::Variant>::const_iterator it = srcMap.begin();it!=srcMap.end();++it)
            {
                firebase::Variant key = it->first;
                if(key.type()==firebase::Variant::kTypeStaticString ||
                        key.type()==firebase::Variant::kTypeMutableString)
                {
                    targetMap[QString::fromUtf8(key.string_value())] = fromFirebaseVariant(it->second);
                }
                else
                {
                    qDebug() << "QtFirebaseService::fromFirebaseVariant:" << "QtFirebase does not support non string keys";
                    qDebug() << "QtFirebaseService::fromFirebaseVariant:" << "Got key of type:" << key.TypeName(v.type());
                    return QVariant();
                }
            }
            return QVariant(targetMap);
        }
        case firebase::Variant::kTypeVector:{
            std::vector<firebase::Variant> srcLst = v.vector();
            QVariantList targetLst;
            for(std::vector<firebase::Variant>::const_iterator it = srcLst.begin();it!=srcLst.end();++it)
            {
                targetLst<<fromFirebaseVariant(*it);
            }
            return QVariant(targetLst);
        }
        default:{
            qDebug() << "QtFirebaseService::fromFirebaseVariant type:" << v.TypeName(v.type()) << " not supported";
        }
    }
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
            return firebase::Variant(static_cast<int64_t>(v.toInt()));
        }
        case QVariant::UInt:{
            return firebase::Variant(static_cast<int64_t>(v.toUInt()));
        }
        case QVariant::LongLong:{
            return firebase::Variant(static_cast<int64_t>(v.toLongLong()));
        }
        case QVariant::ULongLong:{
            return firebase::Variant(static_cast<int64_t>(v.toULongLong()));
        }
        case QVariant::Double:{
            return firebase::Variant(v.toDouble());
        }
        case QVariant::String:{
            std::string str(v.toString().toUtf8().constData());
            return firebase::Variant(str);
        }
        case QVariant::ByteArray:{
            std::string str(v.toByteArray().constData());
            return firebase::Variant(str);
        }
        case QVariant::Map:{
            QVariantMap srcMap = v.toMap();
            std::map<std::string, firebase::Variant> targetMap;
            for(QVariantMap::const_iterator it = srcMap.begin();it!=srcMap.end();++it)
            {
                targetMap[it.key().toUtf8().constData()] = fromQtVariant(it.value());
            }
            return firebase::Variant(targetMap);
        }
        case QVariant::List:{
            QVariantList srcLst = v.toList();
            std::vector<firebase::Variant> targetLst;
            for(QVariantList::const_iterator it = srcLst.begin();it!=srcLst.end();++it)
            {
                targetLst.push_back(fromQtVariant(*it));
            }
            return firebase::Variant(targetLst);
        }
        default:{
            qDebug() << "QtFirebaseService::fromQtVariant type:" << v.typeName() << "not supported";
        }
    }

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

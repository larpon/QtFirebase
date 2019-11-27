#include "qtfirebaseadmobtestdevices.h"

QtFirebaseAdMobTestDevices *QtFirebaseAdMobTestDevices::self = nullptr;

QtFirebaseAdMobTestDevices::QtFirebaseAdMobTestDevices(QObject* parent)
    : QObject(parent)
    , _testDevices()
    , __testDevicesByteArrayList()
    , __testDevices(nullptr)

{
    if(self == nullptr) {
        self = this;
        qDebug() << self << "::QtFirebaseAdMobTestDevices" << "singleton";
    }
}

QtFirebaseAdMobTestDevices::~QtFirebaseAdMobTestDevices()
{
    qDebug() << this << "::~QtFirebaseAdMobTestDevices" << "shutting down";
    self = nullptr;
}

bool QtFirebaseAdMobTestDevices::checkInstance(const char *function) const
{
    bool b = (QtFirebaseAdMobTestDevices::self != nullptr);
    if(!b)
        qWarning("QtFirebaseAdMobTestDevices::%s: Please instantiate the QtFirebaseAdMobTestDevices object first", function);
    return b;
}

QVariantList QtFirebaseAdMobTestDevices::testDevices() const
{
    return _testDevices;
}

void QtFirebaseAdMobTestDevices::setTestDevices(const QVariantList &testDevices)
{
    if(_testDevices != testDevices) {
        _testDevices = testDevices;

        __testDevicesByteArrayList.clear();

        if(__testDevices == nullptr)
            __testDevices = new const char*[_testDevices.size()];
        else {
            qDebug() << this << "::setTestDevices" << "potential crash - not tested";
            delete __testDevices;
            __testDevices = new const char*[_testDevices.size()];
            qDebug() << this << "::setTestDevices" << "survived potential crash!";
        }

        int index = 0;
        for (QVariantList::iterator j = _testDevices.begin(); j != _testDevices.end(); j++)
        {
            //QByteArray ba = QByteArray((*j).toString().toLatin1().data());
            qDebug() << this << "::setTestDevices" << "adding" << (*j).toString();
            __testDevicesByteArrayList.append(QByteArray((*j).toString().toLatin1().data()));
            qDebug() << this << "::setTestDevices" << "adding to char**" << (*j).toString();
            __testDevices[index] = __testDevicesByteArrayList.at(index).constData();
            index++;
        }

        qDebug() << this << "::setTestDevices" << "done";

        emit testDevicesChanged();
    }
}

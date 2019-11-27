#include "qtfirebaseadmobrequest.h"

QtFirebaseAdMobRequest::QtFirebaseAdMobRequest(QObject *parent)
    : QObject(parent)
    , _gender(GenderUnknown)
    , _childDirectedTreatment(ChildDirectedTreatmentTagged)
    , __keywords(nullptr)
    , __extras(nullptr)
    , __testDevices(nullptr)
{

}

int QtFirebaseAdMobRequest::gender() const
{
    return _gender;
}

void QtFirebaseAdMobRequest::setGender(int gender)
{
    if(_gender != gender) {
        _gender = gender;
        emit genderChanged();
    }
}

int QtFirebaseAdMobRequest::childDirectedTreatment() const
{
    return _childDirectedTreatment;
}

void QtFirebaseAdMobRequest::setChildDirectedTreatment(int childDirectedTreatment)
{
    if(_childDirectedTreatment != childDirectedTreatment) {
        _childDirectedTreatment = childDirectedTreatment;
        emit childDirectedTreatmentChanged();
    }
}

QDateTime QtFirebaseAdMobRequest::birthday() const
{
    return _birthday;
}

void QtFirebaseAdMobRequest::setBirthday(const QDateTime &birthday)
{
    if(_birthday != birthday) {
        _birthday = birthday;
        emit birthdayChanged();
    }
}

QVariantList QtFirebaseAdMobRequest::keywords() const
{
    return _keywords;
}

void QtFirebaseAdMobRequest::setKeywords(const QVariantList &keywords)
{
    if(_keywords != keywords) {
        _keywords = keywords;

        __keywordsByteArrayList.clear();

        if(__keywords == nullptr)
            __keywords = new const char*[_keywords.size()];
        else {
            qDebug() << this << "::setKeywords" << "potential crash - not tested";
            delete __keywords;
            __keywords = new const char*[_keywords.size()];
            qDebug() << this << "::setKeywords" << "survived potential crash!";
        }

        int index = 0;
        for (QVariantList::iterator j = _keywords.begin(); j != _keywords.end(); j++)
        {
            //QByteArray ba = QByteArray((*j).toString().toLatin1().data());
            qDebug() << this << "::setKeywords" << "adding" << (*j).toString();
            __keywordsByteArrayList.append(QByteArray((*j).toString().toLatin1().data()));
            qDebug() << this << "::setKeywords" << "adding to char**" << (*j).toString();
            __keywords[index] = __keywordsByteArrayList.at(index).constData();
            index++;
        }

        qDebug() << this << "::setKeywords" << "done";

        emit keywordsChanged();
    }
}

QVariantList QtFirebaseAdMobRequest::extras() const
{
    return _extras;
}

void QtFirebaseAdMobRequest::setExtras(const QVariantList &extras)
{
    if(_extras != extras) {
        _extras = extras;

        __extrasList.clear();

        if(__extras == nullptr)
            __extras = new firebase::admob::KeyValuePair[_extras.size()];
        else {
            qDebug() << this << "::setExtras" << "potential crash - not tested";
            delete __extras;
            __extras = new firebase::admob::KeyValuePair[_extras.size()];
            qDebug() << this << "::setExtras" << "survived potential crash!";
        }

        int index = 0;
        for (QVariantList::iterator j = _extras.begin(); j != _extras.end(); j++) {
            if((*j).canConvert<QVariantMap>()) {

                QVariantMap map = (*j).toMap();

                if(!map.isEmpty()) {
                    if(map.first().canConvert<QString>()) {
                        QString key = map.firstKey();
                        QString value = map.first().toString();
                        qDebug() << this << "::setExtras" << "appending" << key << ":" << value << "to list";
                        __extrasList.append(QPair<QByteArray,QByteArray>(key.toLatin1(),value.toLatin1()));
                        __extras[index] = { __extrasList.at(index).first.constData(), __extrasList.at(index).second.constData() };
                    }
                }

            } else {
                qWarning() << this << "::setExtras" << "wrong entry in extras list at index" << index;
            }
            index++;
        }
        emit extrasChanged();
    }
}

QVariantList QtFirebaseAdMobRequest::testDevices() const
{
    return _testDevices;
}

void QtFirebaseAdMobRequest::setTestDevices(QVariantList &testDevices)
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

firebase::admob::AdRequest QtFirebaseAdMobRequest::asAdMobRequest()
{
    qDebug() << this << "::asAdMobRequest";

    // Set some hopefully sane defaults

    // If the app is aware of the user's gender, it can be added to the targeting
    // information. Otherwise, "unknown" should be used.
    _admobRequest.gender = firebase::admob::kGenderUnknown;

    // This value allows publishers to specify whether they would like the request
    // to be treated as child-directed for purposes of the Children’s Online
    // Privacy Protection Act (COPPA).
    // See http://business.ftc.gov/privacy-and-security/childrens-privacy.
    _admobRequest.tagged_for_child_directed_treatment = firebase::admob::kChildDirectedTreatmentStateTagged;

    // The user's birthday, if known. Note that months are indexed from one.
    _admobRequest.birthday_day = 0;
    _admobRequest.birthday_month = 0; // Months begin at 1!
    _admobRequest.birthday_year = 0;

    // Additional keywords to be used in targeting.
    _admobRequest.keyword_count = 0;

    // "Extra" key value pairs can be added to the request as well. Typically
    // these are used when testing new features.
    _admobRequest.extras_count = 0;

    _admobRequest.test_device_id_count = 0;

    // Change defaults
    if(_gender != QtFirebaseAdMobRequest::GenderUnknown) {
        if(_gender == QtFirebaseAdMobRequest::GenderFemale)
            _admobRequest.gender = firebase::admob::kGenderFemale;
        else
            _admobRequest.gender = firebase::admob::kGenderMale;
    }

    if(_childDirectedTreatment != QtFirebaseAdMobRequest::ChildDirectedTreatmentTagged) {
        if(_childDirectedTreatment == QtFirebaseAdMobRequest::ChildDirectedTreatmentNotTagged)
            _admobRequest.tagged_for_child_directed_treatment = firebase::admob::kChildDirectedTreatmentStateNotTagged;
        else
            _admobRequest.tagged_for_child_directed_treatment = firebase::admob::kChildDirectedTreatmentStateUnknown;
    }

    if(_birthday.isValid()) {
        _admobRequest.birthday_day = _birthday.date().day();
        _admobRequest.birthday_month = _birthday.date().month();
        _admobRequest.birthday_year = _birthday.date().year();
    }

    if(!_keywords.isEmpty()) {
        _admobRequest.keyword_count = static_cast<unsigned int>(_keywords.size());
        _admobRequest.keywords = __keywords;
    }

    if(!_extras.isEmpty()) {
        _admobRequest.extras_count = static_cast<unsigned int>(_extras.size());
        _admobRequest.extras = __extras;

        // To debug actual extra key:value pairs
        //qDebug() << this << "::asAdMobRequest" << QString(__extras[1].key) << ":" << QString(__extras[1].value);
    }

    qDebug() << this << "::asAdMobRequest" << "Gender" << _gender;
    qDebug() << this << "::asAdMobRequest" << "Child directed treatment" << _childDirectedTreatment;
    qDebug() << this << "::asAdMobRequest" << "Birthday" << _birthday;
    qDebug() << this << "::asAdMobRequest" << "Keywords" << _keywords;
    qDebug() << this << "::asAdMobRequest" << "Extras" << _extras;

    // NOTE if no test devices are provided - use list from QtFirebaseAdMob singleton if not empty
    if(!_testDevices.isEmpty()) {
        qDebug() << this << "::asAdMobRequest" << "TestDevices" << _testDevices;
        _admobRequest.test_device_id_count = static_cast<unsigned int>(__testDevicesByteArrayList.size());
        _admobRequest.test_device_ids = __testDevices;
    } else {
        qDebug() << this << "::asAdMobRequest" << "TestDevices ( from" << qFirebaseAdMobTestDevices << ")" << qFirebaseAdMobTestDevices->testDevices();
        _admobRequest.test_device_id_count = static_cast<unsigned int>(qFirebaseAdMobTestDevices->__testDevicesByteArrayList.size());
        _admobRequest.test_device_ids = qFirebaseAdMobTestDevices->__testDevices;
    }

    return _admobRequest;
}

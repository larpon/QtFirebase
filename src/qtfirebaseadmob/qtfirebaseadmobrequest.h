#ifndef QTFIREBASE_ADMOB_REQUEST_H
#define QTFIREBASE_ADMOB_REQUEST_H

#include <QDebug>
#include <QObject>
#include <QDateTime>

#include "qtfirebaseadmobtestdevices.h"

#include "firebase/admob.h"

class QtFirebaseAdMobRequest : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int gender READ gender WRITE setGender NOTIFY genderChanged)
    Q_PROPERTY(int childDirectedTreatment READ childDirectedTreatment WRITE setChildDirectedTreatment NOTIFY childDirectedTreatmentChanged)
    Q_PROPERTY(QDateTime birthday READ birthday WRITE setBirthday NOTIFY birthdayChanged)
    Q_PROPERTY(QVariantList keywords READ keywords WRITE setKeywords NOTIFY keywordsChanged)
    Q_PROPERTY(QVariantList extras READ extras WRITE setExtras NOTIFY extrasChanged)
    Q_PROPERTY(QVariantList testDevices READ testDevices WRITE setTestDevices NOTIFY testDevicesChanged)

public:

    enum Gender
    {
        GenderUnknown,
        GenderFemale,
        GenderMale
    };
    Q_ENUM(Gender)

    enum ChildDirectedTreatment
    {
        ChildDirectedTreatmentUnknown,
        ChildDirectedTreatmentTagged,
        ChildDirectedTreatmentNotTagged
    };
    Q_ENUM(ChildDirectedTreatment)

    QtFirebaseAdMobRequest(QObject* parent = nullptr);

    int gender() const;
    void setGender(int gender);

    int childDirectedTreatment() const;
    void setChildDirectedTreatment(int childDirectedTreatment);

    QDateTime birthday() const;
    void setBirthday(const QDateTime &birthday);

    QVariantList keywords() const;
    void setKeywords(const QVariantList &keywords);

    QVariantList extras() const;
    void setExtras(const QVariantList &extras);

    QVariantList testDevices() const;
    void setTestDevices(QVariantList &testDevices);

    firebase::admob::AdRequest asAdMobRequest();
signals:
    void genderChanged();
    void childDirectedTreatmentChanged();
    void birthdayChanged();
    void keywordsChanged();
    void extrasChanged();
    void testDevicesChanged();

private:

    int _gender;

    int _childDirectedTreatment;

    QDateTime _birthday;

    QVariantList _keywords;
    // Stores the actual char* data so it remains valid throughout a request cycle
    QByteArrayList __keywordsByteArrayList;
    const char** __keywords;

    QVariantList _extras;
    // Stores the actual char* data so it remains valid throughout a request cycle
    QList<QPair<QByteArray,QByteArray>> __extrasList;
    firebase::admob::KeyValuePair* __extras;

    QVariantList _testDevices;
    // Stores the actual char* data so it remains valid throughout a request cycle
    QByteArrayList __testDevicesByteArrayList;
    const char** __testDevices;

    firebase::admob::AdRequest _admobRequest;
};

#endif // QTFIREBASE_ADMOB_REQUEST_H

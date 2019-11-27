#ifndef QTFIREBASE_ADMOB_TEST_DEVICES_H
#define QTFIREBASE_ADMOB_TEST_DEVICES_H

#ifdef QTFIREBASE_BUILD_ADMOB

#if defined(qFirebaseAdMobTestDevices)
#undef qFirebaseAdMobTestDevices
#endif
#define qFirebaseAdMobTestDevices (static_cast<QtFirebaseAdMobTestDevices *>(QtFirebaseAdMobTestDevices::instance()))

#include <QDebug>
#include <QList>

/*
 * AdMobTestDevices (Singleton object)
 * Used as fallback
 */
class QtFirebaseAdMobTestDevices : public QObject
{
    // friend class so it can read the __testDevices directly
    friend class QtFirebaseAdMobRequest;

    Q_OBJECT

    Q_PROPERTY(QVariantList testDevices READ testDevices WRITE setTestDevices NOTIFY testDevicesChanged)

public:

    explicit QtFirebaseAdMobTestDevices(QObject* parent = nullptr);
    ~QtFirebaseAdMobTestDevices();

    static QtFirebaseAdMobTestDevices *instance() {
        if(self == nullptr) {
            self = new QtFirebaseAdMobTestDevices(nullptr);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }
    bool checkInstance(const char *function) const;

    QVariantList testDevices() const;
    void setTestDevices(const QVariantList &testDevices);

signals:
    void testDevicesChanged();

private:
    static QtFirebaseAdMobTestDevices *self;
    Q_DISABLE_COPY(QtFirebaseAdMobTestDevices)

    QVariantList _testDevices;
    QByteArrayList __testDevicesByteArrayList;
    const char** __testDevices;
};

#endif // QTFIREBASE_BUILD_ADMOB
#endif // QTFIREBASE_ADMOB_TEST_DEVICES_H

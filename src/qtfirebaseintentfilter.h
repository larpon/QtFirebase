#pragma once

#include <QObject>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

namespace FirebaseIntent {

/**
 *
 * connect(&FirebaseIntentfilter::instance(), &FirebaseIntentfilter::googleAuthIntent, this, [this](const QAndroidJniObject &intent) {
 *     // ...
 * }
 *
 * connect(&FirebaseIntentfilter::instance(), &FirebaseIntentfilter::googleAuthActivityResult, this, [this](int requestCode, int resultCode, const QAndroidJniObject &data) {
 *     // ...
 * }
 *
 * connect(&FirebaseIntentfilter::instance(), &FirebaseIntentfilter::setCurrentUser, this, [this](QString token) {
 *     // ...
 * }
 *
 * connect(&FirebaseIntentfilter::instance(), &FirebaseIntentfilter::signedOut, this, [this]() {
 *     // ...
 * }
 *
 * For this item to work your ned to edit qtfirebasejnisetup.h
 * instead of the default Qt one
 */

class FirebaseIntentfilter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FirebaseIntentfilter)

public:
    virtual ~FirebaseIntentfilter() = default;

    static FirebaseIntentfilter &instance();

signals:
#ifdef Q_OS_ANDROID
    void googleAuthIntent(const QAndroidJniObject &intent);
    void googleAuthActivityResult(int requestCode, int resultCode, const QAndroidJniObject &data);
    void setCurrentUser(QString token);
    void signedOut();
#endif

private:
    explicit FirebaseIntentfilter(QObject *parent = nullptr);

};

} // FirebaseIntent

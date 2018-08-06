#include "qtfirebaseadmob.h"

//#include <QGuiApplication>
#include <qqmlfile.h>
#include <QString>

namespace admob = ::firebase::admob;

/*
 * AdMob
 * (Base AdMob Object)
 */

QtFirebaseAdMob *QtFirebaseAdMob::self = 0;

QtFirebaseAdMob::QtFirebaseAdMob(QObject* parent) : QObject(parent)
{
    if(self == 0) {
        self = this;
        qDebug() << self << "::QtFirebaseAdMob" << "singleton";
    }

    _ready = false;
    _initializing = false;

    __testDevices = 0;

    if(!qFirebase->ready()) {
        connect(qFirebase,&QtFirebase::readyChanged, this, &QtFirebaseAdMob::init);
        qFirebase->requestInit();
    }
}

QtFirebaseAdMob::~QtFirebaseAdMob()
{
    if(_ready) {
        qDebug() << this << "::~QtFirebaseAdMob" << "shutting down";
        setReady(false);
        emit shutdown();
        //admob::Terminate(); // TODO causes crash see https://github.com/firebase/quickstart-cpp/issues/19
        self = 0;
    }
}

bool QtFirebaseAdMob::checkInstance(const char *function) const
{
    bool b = (QtFirebaseAdMob::self != 0);
    if(!b)
        qWarning("QtFirebaseAdMob::%s: Please instantiate the QtFirebaseAdMob object first", function);
    return b;
}

bool QtFirebaseAdMob::ready() const
{
    return _ready;
}

void QtFirebaseAdMob::setReady(bool ready)
{
    if(_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

QString QtFirebaseAdMob::appId() const
{
    return _appId;
}

void QtFirebaseAdMob::setAppId(const QString &adMobAppId)
{
    if(_appId != adMobAppId) {
        _appId = adMobAppId;

        if(!ready())
            init();

        emit appIdChanged();
    }
}

QVariantList QtFirebaseAdMob::testDevices() const
{
    return _testDevices;
}

void QtFirebaseAdMob::setTestDevices(const QVariantList &testDevices)
{
    if(_testDevices != testDevices) {
        _testDevices = testDevices;

        __testDevicesByteArrayList.clear();

        if(__testDevices == 0)
            __testDevices = new const char*[_testDevices.size()];
        else {
            qDebug() << this << "::setTestDevices" << "potential crash - not tested";
            delete __testDevices;
            __testDevices = new const char*[_testDevices.size()];
            qDebug() << this << "::setTestDevices" << "survived potential crash!";
        }

        unsigned index = 0;
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

void QtFirebaseAdMob::init()
{
    if(!qFirebase->ready()) {
        qDebug() << this << "::init" << "base not ready";
        return;
    }

    if(!_ready && !_initializing) {

        // Notify the user of the pre-requisity
        if(_appId.isEmpty()) {
            qWarning() << this << "::init" << "failed to initialize Firebase AdMob." << "No AdMob app ID set";
            return;
        }

        _initializing = true;

        __appIdByteArray = _appId.toLatin1();
        __appId = __appIdByteArray.data();

        // Initialize the Firebase AdMob library with AdMob app ID.
        qDebug() << this << "::init" << "initializing with App ID" << __appId;

        firebase::App* app = qFirebase->firebaseApp();
        firebase::InitResult ir = admob::Initialize(*app, __appId);

        qDebug() << this << "::init" << "initialized";

        if(ir != firebase::kInitResultSuccess) {
            qWarning() << this << "::init" << "failed to initialize Firebase AdMob";
            _initializing = false;
            return;
        }

        qDebug() << this << "::init" << "initialized";
        _initializing = false;
        setReady(true);
    }
}


/*
 * AdMobRequest
 */

QtFirebaseAdMobRequest::QtFirebaseAdMobRequest(QObject *parent) : QObject(parent)
{
    _gender = QtFirebaseAdMob::GenderUnknown;
    _childDirectedTreatment = QtFirebaseAdMob::ChildDirectedTreatmentTagged;
    __keywords = 0;
    __testDevices = 0;
    __extras = 0;
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

        if(__keywords == 0)
            __keywords = new const char*[_keywords.size()];
        else {
            qDebug() << this << "::setKeywords" << "potential crash - not tested";
            delete __keywords;
            __keywords = new const char*[_keywords.size()];
            qDebug() << this << "::setKeywords" << "survived potential crash!";
        }

        unsigned index = 0;
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

        if(__extras == 0)
            __extras = new firebase::admob::KeyValuePair[_extras.size()];
        else {
            qDebug() << this << "::setExtras" << "potential crash - not tested";
            delete __extras;
            __extras = new firebase::admob::KeyValuePair[_extras.size()];
            qDebug() << this << "::setExtras" << "survived potential crash!";
        }

        unsigned index = 0;
        for (QVariantList::iterator j = _extras.begin(); j != _extras.end(); j++)
        {
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

        if(__testDevices == 0)
            __testDevices = new const char*[_testDevices.size()];
        else {
            qDebug() << this << "::setTestDevices" << "potential crash - not tested";
            delete __testDevices;
            __testDevices = new const char*[_testDevices.size()];
            qDebug() << this << "::setTestDevices" << "survived potential crash!";
        }

        unsigned index = 0;
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

admob::AdRequest QtFirebaseAdMobRequest::asAdMobRequest()
{
    qDebug() << this << "::asAdMobRequest";

    // Set some hopefully sane defaults

    // If the app is aware of the user's gender, it can be added to the targeting
    // information. Otherwise, "unknown" should be used.
    _admobRequest.gender = admob::kGenderUnknown;

    // This value allows publishers to specify whether they would like the request
    // to be treated as child-directed for purposes of the Children’s Online
    // Privacy Protection Act (COPPA).
    // See http://business.ftc.gov/privacy-and-security/childrens-privacy.
    _admobRequest.tagged_for_child_directed_treatment = admob::kChildDirectedTreatmentStateTagged;

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
    if(_gender != QtFirebaseAdMob::GenderUnknown) {
        if(_gender == QtFirebaseAdMob::GenderFemale)
            _admobRequest.gender = admob::kGenderFemale;
        else
            _admobRequest.gender = admob::kGenderMale;
    }

    if(_childDirectedTreatment != QtFirebaseAdMob::ChildDirectedTreatmentTagged) {
        if(_childDirectedTreatment == QtFirebaseAdMob::ChildDirectedTreatmentNotTagged)
            _admobRequest.tagged_for_child_directed_treatment = admob::kChildDirectedTreatmentStateNotTagged;
        else
            _admobRequest.tagged_for_child_directed_treatment = admob::kChildDirectedTreatmentStateUnknown;
    }

    if(_birthday.isValid()) {
        _admobRequest.birthday_day = _birthday.date().day();
        _admobRequest.birthday_month = _birthday.date().month();
        _admobRequest.birthday_year = _birthday.date().year();
    }

    if(!_keywords.isEmpty()) {
        _admobRequest.keyword_count = _keywords.size();
        _admobRequest.keywords = __keywords;
    }

    if(!_extras.isEmpty()) {
        _admobRequest.extras_count = _extras.size();
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
        _admobRequest.test_device_id_count = __testDevicesByteArrayList.size();
        _admobRequest.test_device_ids = __testDevices;
    } else {
        if(qFirebaseAdMob->ready()) {
            qDebug() << this << "::asAdMobRequest" << "TestDevices ( from" << qFirebaseAdMob << ")" << qFirebaseAdMob->testDevices();
            _admobRequest.test_device_id_count = qFirebaseAdMob->__testDevicesByteArrayList.size();
            _admobRequest.test_device_ids = qFirebaseAdMob->__testDevices;
        }
    }

    return _admobRequest;
}


/*
 * QtFirebaseAdMobBase
 */

QtFirebaseAdMobBase::QtFirebaseAdMobBase(QObject *parent) : QObject(parent) {
    __QTFIREBASE_ID = QString().sprintf("%8p", this);
    _ready = false;
    _loaded = false;
    _initializing = false;
    _nativeUIElement = 0;
    _isFirstInit = true;

    _visible = false;
    _request = 0;
    //connect(this,&QtFirebaseAdMobBase::visibleChanged, this, &QtFirebaseAdMobBase::onVisibleChanged);

    _initTimer = new QTimer(this);
    connect(_initTimer, &QTimer::timeout, this, &QtFirebaseAdMobBase::init);
    _initTimer->start(500);
}

QtFirebaseAdMobBase::~QtFirebaseAdMobBase()
{
    if(_ready) {
        qDebug() << this << "::~QtFirebaseAdMobBase" << "shutting down";
        setReady(false);
    }
}

bool QtFirebaseAdMobBase::ready() const {
    return _ready;
}

void QtFirebaseAdMobBase::setReady(bool ready) {
    if(_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

bool QtFirebaseAdMobBase::loaded() const {
    return _loaded;
}

void QtFirebaseAdMobBase::setLoaded(bool loaded) {
    if(_loaded != loaded) {
        _loaded = loaded;
        emit loadedChanged();
    }
}

QString QtFirebaseAdMobBase::adUnitId() const {
    return _adUnitId;
}

void QtFirebaseAdMobBase::setAdUnitId(const QString &adUnitId) {
    if(_adUnitId != adUnitId) {
        _adUnitId = adUnitId;
        __adUnitIdByteArray = _adUnitId.toLatin1();
        emit adUnitIdChanged();
    }
}

bool QtFirebaseAdMobBase::visible() const {
    return _visible;
}

QtFirebaseAdMobRequest *QtFirebaseAdMobBase::request() const {
    return _request;
}

void QtFirebaseAdMobBase::setRequest(QtFirebaseAdMobRequest *request) {
    if(_request != request) {
        _request = request;
        emit requestChanged();
    }
}

void QtFirebaseAdMobBase::init()
{
    if(!qFirebase->ready()) {
        qDebug() << this << "::init" << "base not ready";
        return;
    }

    if(!qFirebaseAdMob->ready()) {
        qDebug() << this << "::init" << "AdMob base not ready";
        return;
    }

    if(_adUnitId.isEmpty()) {
        qDebug() << this << "::init" << "adUnitId must be set in order to initialize";
        return;
    }

    if(_isFirstInit && !PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "no native ui element. Waiting for it...";
        return;
    }

    // TODO test if this actually nessecary anymore
    if(!_nativeUIElement && !PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "no native ui element";
        return;
    }

    if(!_nativeUIElement && PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "setting native ui element";
        _nativeUIElement = PlatformUtils::getNativeWindow();
    }   

    if(!_ready && !_initializing) {
        _initializing = true;

        auto future = initInternal();

        if(future.status() != firebase::kFutureStatusInvalid) // if the status is invalid then it is already handled elsewhere
        {
            future.OnCompletion([this](const firebase::FutureBase& completed_future)
            {
                // We are probably in a different thread right now.
                #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
                QMetaObject::invokeMethod(this, [this, completed_future]() {
                #endif
                    if(completed_future.error() != admob::kAdMobErrorNone) {
                        qDebug() << this << "::init" << "initializing failed." << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                        emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
                        _initializing = false;
                    }
                    else {
                        qDebug() << this << "::init" << "initialized";
                        _initializing = false;
                        _isFirstInit = false;
                        setReady(true);
                    }
                #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
                });
                #endif
            });
        }
    }
}

void QtFirebaseAdMobBase::load()
{
    if(!_ready) {
        qDebug() << this << "::load" << "not ready";
        return;
    }

    if(_request == 0) {
        qDebug() << this << "::load() no request data sat. Not loading";
        return;
    }

    qDebug() << this << "::load() getting request data";
    setLoaded(false);
    emit loading();
    firebase::FutureBase future = loadInternal();

    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != admob::kAdMobErrorNone) {
                qWarning() << this << "::load" << "load failed" << "ERROR" << "code:" << completed_future.error() << "message:" << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
            }
            else {
                qDebug() << this << "::load loaded";
                setLoaded(true);
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });
}


/*
 * Admob banner base.
 */

QtFirebaseAdMobBannerBase::QtFirebaseAdMobBannerBase(QObject *parent)
    : QtFirebaseAdMobBase(parent) {
    _x = 0;
    _y = 0;
    _width = 0;
    _height = 0;
    //connect(qGuiApp,&QtFirebaseAdMobBannerBase::applicationStateChanged, this, &QtFirebaseAdMobBannerBase::onApplicationStateChanged);
}

int QtFirebaseAdMobBannerBase::getX() const {
    return _x;
}

int QtFirebaseAdMobBannerBase::getY() const {
    return _y;
}

int QtFirebaseAdMobBannerBase::getWidth() const {
    return _width;
}

void QtFirebaseAdMobBannerBase::setWidth(int width) {
    if(_width != width) {
        _width = width;
        emit widthChanged();
    }
}

int QtFirebaseAdMobBannerBase::getHeight() const  {
    return _height;
}

void QtFirebaseAdMobBannerBase::setHeight(int height) {
    if(_height != height) {
        _height = height;
        emit heightChanged();
    }
}

void QtFirebaseAdMobBannerBase::moveTo(int x, int y)
{
    if(_ready) {
        qDebug() << this << "::moveTo moving to" << x << "," << y;
        auto future = moveToInternal(x, y);

        future.OnCompletion([this, x, y](const firebase::FutureBase& completed_future)
        {
            // We are probably in a different thread right now.
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
            QMetaObject::invokeMethod(this, [this, completed_future, x, y]() {
            #endif
                if(completed_future.error() != admob::kAdMobErrorNone) {
                    qWarning() << this << "::moveTo " << x << " x " << y << " ERROR" << "code:" << completed_future.error() << "message:" << completed_future.error_message();
                    emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
                }
                else {
                    if(_x != x) {
                        _x = x;
                        emit xChanged();
                    }

                    if(_y != y) {
                        _y = y;
                        emit yChanged();
                    }
                    qDebug() << this << "::moveTo moved to" << x << "," << y;
                }
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
            });
            #endif
        });
    }
}

void QtFirebaseAdMobBannerBase::moveTo(Position position)
{
    if(!_ready) {
        qDebug() << this << "::moveTo" << "not ready";
        return;
    }

    auto future = moveToInternal(position);
    future.OnCompletion([this, position](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future, position]() {
        #endif
            if(completed_future.error() != admob::kAdMobErrorNone) {
                qWarning() << this << "::moveTo " << position << " ERROR" << "code:" << completed_future.error() << "message:" << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
            }
            else {
                qDebug() << this << "::moveTo moved to" << position;
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });
}

void QtFirebaseAdMobBannerBase::setVisible(bool visible)
{
    if(!_ready) {
        qDebug() << this << "::setVisible native part not ready";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::setVisible native part not loaded - so not changing visiblity to" << visible;
        return;
    }

    if(_visible != visible) {
        firebase::FutureBase future = setVisibleInternal(visible);
        future.OnCompletion([this, visible](const firebase::FutureBase& completed_future)
        {
            // We are probably in a different thread right now.
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
            QMetaObject::invokeMethod(this, [this, completed_future, visible]() {
            #endif
                if(completed_future.error() != admob::kAdMobErrorNone) {
                    qDebug() << this << "::setVisible" << visible <<  " ERROR code" << completed_future.error() << "message" << completed_future.error_message();
                    emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
                }
                else {
                    qDebug() << this << "::setVisible(" << visible << ")";

                    _visible = visible;
                    emit visibleChanged();
                }
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
            });
            #endif
        });
    }
}

void QtFirebaseAdMobBannerBase::setX(int x)
{
    if(_x != x) {
        moveTo(x, _y);
    }
}

void QtFirebaseAdMobBannerBase::setY(int y)
{
    if(_y != y) {
        moveTo(_x, y);
    }
}

void QtFirebaseAdMobBannerBase::onApplicationStateChanged(Qt::ApplicationState state)
{
    // NOTE makes sure the ad banner is on top of the Qt surface
#if defined(Q_OS_ANDROID) && !defined(QTFIREBASE_DISABLE_FIX_ANDROID_AUTO_APP_STATE_VISIBILTY)
    qDebug() << this << "::onApplicationStateChanged" << "Applying visibility fix";
    if(state != Qt::ApplicationActive)
        hide();
    else
        show();
#else
    Q_UNUSED(state);
#endif
}

void QtFirebaseAdMobBannerBase::show()
{
    setVisible(true);
}

void QtFirebaseAdMobBannerBase::hide()
{
    setVisible(false);
}


/*
 * AdMobBanner
 *
 */

QtFirebaseAdMobBanner::QtFirebaseAdMobBanner(QObject *parent) : QtFirebaseAdMobBannerBase(parent)
{
}

QtFirebaseAdMobBanner::~QtFirebaseAdMobBanner()
{
    if(_ready) {
        _banner->Destroy();
        qFirebase->waitForFutureCompletion(_banner->DestroyLastResult()); // TODO MAYBE move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
        qDebug() << this << "::~QtFirebaseAdMobBanner" << "Destroyed banner";
    }
}

firebase::FutureBase QtFirebaseAdMobBanner::setVisibleInternal(bool visible)
{
    return visible ? _banner->Show() : _banner->Hide();
}

firebase::FutureBase QtFirebaseAdMobBanner::setXInternal(int x)
{
    return _banner->MoveTo(x, _y);
}

firebase::FutureBase QtFirebaseAdMobBanner::setYInternal(int y)
{
    return _banner->MoveTo(_x, y);
}

firebase::FutureBase QtFirebaseAdMobBanner::initInternal()
{
    _banner = new admob::BannerView();

    admob::AdSize ad_size;
    ad_size.ad_size_type = admob::kAdSizeStandard;
    ad_size.width = getWidth();
    ad_size.height = getHeight();

    // A reference to an iOS UIView or an Android Activity.
    // This is the parent UIView or Activity of the banner view.
    qDebug() << this << "::init initializing with AdUnitID" << __adUnitIdByteArray.constData();
    return _banner->Initialize(static_cast<admob::AdParent>(_nativeUIElement), __adUnitIdByteArray.constData(), ad_size);
}

firebase::FutureBase QtFirebaseAdMobBanner::loadInternal()
{
    admob::AdRequest request = _request->asAdMobRequest();
    return _banner->LoadAd(request);
}

firebase::FutureBase QtFirebaseAdMobBanner::moveToInternal(int x, int y)
{
    return _banner->MoveTo(x, y);
}

firebase::FutureBase QtFirebaseAdMobBanner::moveToInternal(Position position)
{
    switch (position) {
    case PositionTopCenter:
        qDebug() << this << "::moveTo position top-center";
        return _banner->MoveTo(admob::BannerView::kPositionTop);
    case PositionTopLeft:
        qDebug() << this << "::moveTo position top-left";
        return _banner->MoveTo(admob::BannerView::kPositionTopLeft);
    case PositionTopRight:
        qDebug() << this << "::moveTo position top-right";
        return _banner->MoveTo(admob::BannerView::kPositionTopRight);
    case PositionBottomCenter:
        qDebug() << this << "::moveTo position bottom-center";
        return _banner->MoveTo(admob::BannerView::kPositionBottom);
    case PositionBottomLeft:
        qDebug() << this << "::moveTo position bottom-left";
        return _banner->MoveTo(admob::BannerView::kPositionBottomLeft);
    case PositionBottomRight:
        qDebug() << this << "::moveTo position bottom-right";
        return _banner->MoveTo(admob::BannerView::kPositionBottomRight);
    }

    qDebug() << this << "::moveTo position unknown" << position;
    return firebase::FutureBase();
}

/*
 * AdMobInterstitial
 *
 */

QtFirebaseAdMobInterstitial::QtFirebaseAdMobInterstitial(QObject* parent) : QtFirebaseAdMobBase(parent)
{
    _interstitialAdListener = NULL;
}

QtFirebaseAdMobInterstitial::~QtFirebaseAdMobInterstitial()
{
    if(_ready) {
        qDebug() << this << "::~QtFirebaseAdMobInterstitial" << "Destroyed Interstitial";
        //delete _interstitial;
    }

    _initTimer->stop();
    delete _initTimer;
}

void QtFirebaseAdMobInterstitial::setVisible(bool visible)
{
    if(!_ready) {
        qDebug() << this << "::setVisible native part not ready";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::setVisible native part not loaded - so not changing visiblity to" << visible;
        return;
    }

    if(!_visible && visible) {
        show(); // NOTE show will change _visible and emit signal
    } else {
        // NOTE An interstitial can't be hidden by any other than the user
        qInfo() << this << "::setVisible" << visible << " - interstitials can't be hidden programmatically. Not hidding";
    }
}

firebase::FutureBase QtFirebaseAdMobInterstitial::initInternal()
{
    _interstitial = new admob::InterstitialAd();

    //__adUnitIdByteArray = _adUnitId.toLatin1();
    //__adUnitId = __adUnitIdByteArray.data();

    // A reference to an iOS UIView or an Android Activity.
    // This is the parent UIView or Activity of the Interstitial view.
    qDebug() << this << "::init initializing with AdUnitID" << __adUnitIdByteArray.constData();
    auto future = _interstitial->Initialize(static_cast<admob::AdParent>(_nativeUIElement), __adUnitIdByteArray.constData());
    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != admob::kAdMobErrorNone) {
                qDebug() << this << "::init" << "initializing failed." << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
                _initializing = false;
            } else {
                _initTimer->stop();
                // NOTE don't delete it as we need it on iOS to re-initalize the whole mill after every load and show
                //delete _initTimer;

                qDebug() << this << "::init initialized";
                _initializing = false;
                _isFirstInit = false;

                // Add listener (can't be done before it's initialized)
                auto cpy = _interstitialAdListener;
                _interstitialAdListener = new QtFirebaseAdMobInterstitialAdListener(this);
                _interstitial->SetListener(_interstitialAdListener);
                if(cpy) {
                    delete cpy;
                }

                setReady(true);
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });

    return firebase::FutureBase(); // invalid future because we already handled it
}

void QtFirebaseAdMobInterstitial::onPresentationStateChanged(int state)
{
    if(state == PresentationStateCoveringUI) {
        if(!_visible) {
            _visible = true;
            emit visibleChanged();
        }
    }

    if(state == PresentationStateHidden) {
        if(_visible) {
            _visible = false;
            emit visibleChanged();
        }

        setLoaded(false);
        qDebug() << this << "::onPresentationStateChanged() loaded false";

        // NOTE iOS necessities
#if defined(Q_OS_IOS)

        setReady(false);
        qDebug() << this << "::onPresentationStateChanged() ready false";

        // Will be newed when init() is called
        //delete _interstitial; // NOTE Crashes the app when used

        // NOTE Auto re-initializing because of this: https://firebase.google.com/docs/admob/ios/interstitial#only_show_gadinterstitial_once
        qDebug() << this << "::onPresentationStateChanged() re-initializing one-time use GADInterstitial";
        _initTimer->start(500);

#endif

        emit closed();
    }
}

firebase::FutureBase QtFirebaseAdMobInterstitial::loadInternal()
{
    admob::AdRequest request = _request->asAdMobRequest();
    return _interstitial->LoadAd(request);
}

void QtFirebaseAdMobInterstitial::show()
{
    if(!_ready) {
        qDebug() << this << "::show" << "not ready - so not showing";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::show" << "not loaded - so not showing";
        return;
    }

    firebase::FutureBase future = _interstitial->Show();
    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != admob::kAdMobErrorNone) {
                qDebug() << this << "::show" << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });
}

/*
 * AdMobRewardedVideoAd
 *
 */

QtFirebaseAdMobRewardedVideoAd::QtFirebaseAdMobRewardedVideoAd(QObject* parent):
    QtFirebaseAdMobBase(parent)
{
}

QtFirebaseAdMobRewardedVideoAd::~QtFirebaseAdMobRewardedVideoAd()
{
    if(_ready) {
        firebase::admob::rewarded_video::Destroy();
        qDebug() << this << "::~QtFirebaseAdMobRewardedVideoAd" << "Destroyed";
    }
    _initTimer->stop();
}

void QtFirebaseAdMobRewardedVideoAd::setVisible(bool visible)
{
    if(!_ready) {
        qDebug() << this << "::setVisible native part not ready";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::setVisible native part not loaded - so not changing visiblity to" << visible;
        return;
    }

    if(!_visible && visible) {
        show(); // NOTE show will change _visible and emit signal
    } else {
        // NOTE An interstitial can't be hidden by any other than the user
        qInfo() << this << "::setVisible" << visible << " - interstitials can't be hidden programmatically. Not hidding";
    }
}

firebase::FutureBase QtFirebaseAdMobRewardedVideoAd::initInternal()
{
    auto future = firebase::admob::rewarded_video::Initialize();

    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != admob::kAdMobErrorNone)
            {
                qDebug() << this << "::init initializing failed." << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
                _initializing = false;
            }
            else
            {
                _initTimer->stop();
                qDebug() << this << "::init initialized";
                _initializing = false;
                _isFirstInit = false;
                firebase::admob::rewarded_video::SetListener(this);
                setReady(true);
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });

    return firebase::FutureBase();
}

void QtFirebaseAdMobRewardedVideoAd::onPresentationStateChanged(int state)
{
    switch (state) {
    case QtFirebaseAdMobInterstitial::PresentationStateCoveringUI: {
        if(!_visible) {
            _visible = true;
            emit visibleChanged();
        }
    }
        break;
    case QtFirebaseAdMobInterstitial::PresentationStateHidden: {
        if(_visible) {
            _visible = false;
            emit visibleChanged();
        }

        setLoaded(false);
        qDebug() << this << "::onPresentationStateChanged() loaded false";

        // NOTE iOS necessities
        /*#if defined(Q_OS_IOS)

        setReady(false);
        qDebug() << this << "::onPresentationStateChanged() ready false";

        // Will be newed when init() is called
        //delete _interstitial; // NOTE Crashes the app when used

        // NOTE Auto re-initializing because of this: https://firebase.google.com/docs/admob/ios/interstitial#only_show_gadinterstitial_once
        qDebug() << this << "::onPresentationStateChanged() re-initializing one-time use GADInterstitial";
        _initTimer->start(500);

        #endif*/

        emit closed();
    }
        break;
    }
}

firebase::FutureBase QtFirebaseAdMobRewardedVideoAd::loadInternal()
{
    admob::AdRequest request = _request->asAdMobRequest();
    return firebase::admob::rewarded_video::LoadAd( __adUnitIdByteArray.constData(), request);
}

void QtFirebaseAdMobRewardedVideoAd::show()
{
    if(!_ready) {
        qDebug() << this << "::show() not ready - so not showing";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::show() not loaded - so not showing";
        return;
    }

    if(!_nativeUIElement && !PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "no native ui element";
        return;
    }

    if(!_nativeUIElement && PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "setting native ui element";
        _nativeUIElement = PlatformUtils::getNativeWindow();
    }

    firebase::FutureBase future = firebase::admob::rewarded_video::Show(static_cast<admob::AdParent>(_nativeUIElement));

    future.OnCompletion([this](const firebase::FutureBase& completed_future)
    {
        // We are probably in a different thread right now.
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(this, [this, completed_future]() {
        #endif
            if(completed_future.error() != admob::kAdMobErrorNone)
            {
                qDebug() << this << "::show " << "ERROR: Action failed with error code and message: " << completed_future.error() << completed_future.error_message();
                emit error(completed_future.error(), QString(QString::fromUtf8(completed_future.error_message())));
            }
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        });
        #endif
    });
}

void QtFirebaseAdMobRewardedVideoAd::OnRewarded(firebase::admob::rewarded_video::RewardItem reward)
{
    QString type = QString::fromStdString(reward.reward_type);
    qDebug() << this << QString(QStringLiteral("Rewarding user of %1 with amount %2")).arg(type).arg(QString::number(reward.amount));

    emit rewarded(type, reward.amount);
}

void QtFirebaseAdMobRewardedVideoAd::OnPresentationStateChanged(firebase::admob::rewarded_video::PresentationState state)
{
    if(state == firebase::admob::rewarded_video::kPresentationStateHidden)
    {
        qDebug() << this << "kPresentationStateHidden";
    }
    else if(state == firebase::admob::rewarded_video::kPresentationStateCoveringUI)
    {
        qDebug() << this << "kPresentationStateCoveringUI";
    }
    else if(state == firebase::admob::rewarded_video::kPresentationStateVideoHasStarted)
    {
        qDebug() << this << "kPresentationStateVideoHasStarted";
    }

    int pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;

    if(state == firebase::admob::rewarded_video::kPresentationStateHidden) {
        pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;
    } else if(state == firebase::admob::rewarded_video::kPresentationStateCoveringUI) {
        pState = QtFirebaseAdMobInterstitial::PresentationStateCoveringUI;
    }
    emit presentationStateChanged(pState);
}


#include "qtfirebaseadmob.h"

#include <QGuiApplication>
#include <qqmlfile.h>

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

    if(qFirebase->ready())
        init();
    else {
        connect(qFirebase,&QtFirebase::readyChanged, self, &QtFirebaseAdMob::init);
        qFirebase->requestInit();
    }



}

QtFirebaseAdMob::~QtFirebaseAdMob()
{
    // TODO this is crashing the shutdown?
    //admob::Terminate();
}

bool QtFirebaseAdMob::checkInstance(const char *function)
{
    bool b = (QtFirebaseAdMob::self != 0);
    if (!b)
        qWarning("QtFirebaseAdMob::%s: Please instantiate the QtFirebaseAdMob object first", function);
    return b;

}

QtFirebaseAdMob::Error QtFirebaseAdMob::convertAdMobErrorCode(int admobErrorCode)
{
    if(admobErrorCode == admob::kAdMobErrorAlreadyInitialized) {
        qDebug() << "Already Initialized";
        return ErrorAlreadyInitialized;
    } else if(admobErrorCode == admob::kAdMobErrorInternalError) {
        qDebug() << "Internal Error";
        return ErrorInternalError;
    } else if(admobErrorCode == admob::kAdMobErrorInvalidRequest) {
        qDebug() << "Invalid Request";
        return ErrorInvalidRequest;
    } else if(admobErrorCode == admob::kAdMobErrorLoadInProgress) {
        qDebug() << "Load In Progress";
        return ErrorLoadInProgress;
    } else if(admobErrorCode == admob::kAdMobErrorNetworkError) {
        qDebug() << "Network Error";
        return ErrorNetworkError;
    } else if(admobErrorCode == admob::kAdMobErrorNoFill) {
        qDebug() << "No Fill";
        return ErrorNoFill;
    } else if(admobErrorCode == admob::kAdMobErrorNoWindowToken) {
        qDebug() << "No Window Token";
        return ErrorNoWindowToken;
    } else if(admobErrorCode == admob::kAdMobErrorUninitialized) {
        qDebug() << "Uninitialized";
        return ErrorUninitialized;
    } else {
        qDebug() << "Unknown Error";
        return ErrorUnknown;
    }
}


bool QtFirebaseAdMob::ready()
{
    return _ready;
}

void QtFirebaseAdMob::setReady(bool ready)
{
    if (_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

QString QtFirebaseAdMob::appId()
{
    return _appId;
}

void QtFirebaseAdMob::setAppId(const QString &adMobAppId)
{
    if (_appId != adMobAppId) {
        _appId = adMobAppId;

        if(!ready())
            init();

        emit appIdChanged();
    }
}


QVariantList QtFirebaseAdMob::testDevices()
{
    return _testDevices;
}

void QtFirebaseAdMob::setTestDevices(QVariantList testDevices)
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
        qDebug() << self << "::init" << "base not ready";
        return;
    }

    if(!_ready && !_initializing && !_appId.isEmpty()) {
        _initializing = true;

        /*
        if(_appId.isEmpty()) {
            qWarning() << self << "::init" << "failed to initialize Firebase AdMob." << "No AdMob app ID set";
            _initializing = false;
            return;
        }*/

        __appIdByteArray = _appId.toLatin1();
        __appId = __appIdByteArray.data();

        // Initialize the Firebase AdMob library with AdMob app ID.
        qDebug() << self << "::init" << "initializing with App ID" << __appId;

        firebase::App* app = qFirebase->firebaseApp();
        firebase::InitResult ir = admob::Initialize(*app, __appId);

        qDebug() << self << "::init" << "initialized";

        if(ir != firebase::kInitResultSuccess) {
            qWarning() << self << "::init" << "failed to initialize Firebase AdMob";
            _initializing = false;
            return;
        }

        qDebug() << self << "::init" << "initialized";
        _initializing = false;
        setReady(true);
    }
}

void QtFirebaseAdMob::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    Q_UNUSED(eventId);
    Q_UNUSED(future);
}

/*
 * AdMobRequest
 *
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
    if (_gender != gender) {
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
    if (_childDirectedTreatment != childDirectedTreatment) {
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
    if (_birthday != birthday) {
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
    if (_extras != extras) {
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


QVariantList QtFirebaseAdMobRequest::testDevices()
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
 * AdMobBanner
 *
 */
QtFirebaseAdMobBanner::QtFirebaseAdMobBanner(QObject *parent) : QObject(parent)
{
    __QTFIREBASE_ID = QString().sprintf("%8p", this);
    _ready = false;
    _loaded = false;
    _initializing = false;
    _nativeUIElement = 0;
    _isFirstInit = true;

    _visible = false;

    _x = 0;
    _y = 0;
    _width = 0;
    _height = 0;

    _request = 0;
    //connect(this,&QtFirebaseAdMobBanner::visibleChanged, this, &QtFirebaseAdMobBanner::onVisibleChanged);

    _initTimer = new QTimer(this);
    connect(_initTimer, &QTimer::timeout, this, &QtFirebaseAdMobBanner::init);
    _initTimer->start(500);

    connect(qFirebase,&QtFirebase::futureEvent, this, &QtFirebaseAdMobBanner::onFutureEvent);

    connect(qGuiApp,&QGuiApplication::applicationStateChanged, this, &QtFirebaseAdMobBanner::onApplicationStateChanged);

}

QtFirebaseAdMobBanner::~QtFirebaseAdMobBanner()
{
    if(_ready) {
        _banner->Destroy();
        qFirebase->waitForFutureCompletion(_banner->DestroyLastResult()); // TODO MAYBE move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
        qDebug() << this << "::~QtFirebaseAdMobBanner" << "Destroyed banner";
    }
}

bool QtFirebaseAdMobBanner::ready()
{
    return _ready;
}

void QtFirebaseAdMobBanner::setReady(bool ready)
{
    if (_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

bool QtFirebaseAdMobBanner::loaded()
{
    return _loaded;
}

void QtFirebaseAdMobBanner::setLoaded(bool loaded)
{
    if(_loaded != loaded) {
        _loaded = loaded;
        qDebug() << this << "::setLoaded" << _loaded;
        emit loadedChanged();
    }
}

QString QtFirebaseAdMobBanner::adUnitId()
{
    return _adUnitId;
}

void QtFirebaseAdMobBanner::setAdUnitId(const QString &adUnitId)
{
    if(_adUnitId != adUnitId) {
        _adUnitId = adUnitId;
        __adUnitIdByteArray = _adUnitId.toLatin1();

        emit adUnitIdChanged();
    }
}

bool QtFirebaseAdMobBanner::visible()
{
    return _visible;
}

void QtFirebaseAdMobBanner::setVisible(bool visible)
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
        if(visible) {
            firebase::FutureBase future = _banner->Show();
            qFirebase->waitForFutureCompletion(future); // TODO move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
            if(future.error() != admob::kAdMobErrorNone) {
                qDebug() << this << "::setVisible" << visible <<  "ERROR code" << future.error() << "message" << future.error_message();
                return;
            }
            qDebug() << this << "::setVisible native showed";
        } else {
            firebase::FutureBase future = _banner->Hide();
            qFirebase->waitForFutureCompletion(future); // TODO move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
            if(future.error() != admob::kAdMobErrorNone) {
                qDebug() << this << "::setVisible" << visible << "ERROR code" << future.error() << "message" << future.error_message();
                return;
            }
            qDebug() << this << "::setVisible native hidden";
        }
        _visible = visible;
        emit visibleChanged();
    }
}

int QtFirebaseAdMobBanner::getX()
{
    return _x;
}

void QtFirebaseAdMobBanner::setX(const int &x)
{
    if(!_ready) {
        qDebug() << this << "::setX native part not ready - so not moving";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::setX native part not loaded - so not moving";
        return;
    }

    if(_x != x) {
        qDebug() << this << "::setX moving to" << x << "," << _y;
        _banner->MoveTo(x, _y); // NOTE Potential dangerous code? The code below is more safe but will hang until the "I give up limit" is reached on some devices :/
        // This can mybe be fixed by using the Listeners for the bounding rect??
        /*
        firebase::FutureBase future = _banner->MoveTo(x, _y);
        qFirebase->waitForFutureCompletion(future); // TODO move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
        if(future.Error() != admob::kAdMobErrorNone) {
            qDebug() << this << "::setX ERROR code" << future.Error() << "message" << future.ErrorMessage();
            return;
        }*/
        qDebug() << this << "::setX moved to" << x << "," << _y;
        _x = x;
        emit xChanged();
    }
}

int QtFirebaseAdMobBanner::getY()
{
    return _y;
}

void QtFirebaseAdMobBanner::setY(const int &y)
{
    if(!_ready) {
        qDebug() << this << "::setY native part not ready - so not moving";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::setY native part not loaded - so not moving";
        return;
    }

    if(_y != y) {
        qDebug() << this << "::setY moving to" << _x << "," << y;
        _banner->MoveTo(_x, y); // NOTE Potential dangerous code? The code below is more safe but will hang until the "I give up limit" is reached on some devices :/
        /*
        firebase::FutureBase future = _banner->MoveTo(_x, y);
        qFirebase->waitForFutureCompletion(future); // TODO move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
        if(future.Error() != admob::kAdMobErrorNone) {
            qDebug() << this << "::setY ERROR code" << future.Error() << "message" << future.ErrorMessage();
            return;
        }
        */
        qDebug() << this << "::setY moved to" << _x << "," << y;
        _y = y;
        emit yChanged();
    }
}

int QtFirebaseAdMobBanner::getWidth()
{
    return _width;
}

void QtFirebaseAdMobBanner::setWidth(const int &width)
{
    if(_width != width) {
        _width = width;
        emit widthChanged();
    }
}

int QtFirebaseAdMobBanner::getHeight()
{
    return _height;
}

void QtFirebaseAdMobBanner::setHeight(const int &height)
{
    if(_height != height) {
        _height = height;
        emit heightChanged();
    }
}

QtFirebaseAdMobRequest* QtFirebaseAdMobBanner::request() const
{
    return _request;
}

void QtFirebaseAdMobBanner::setRequest(QtFirebaseAdMobRequest* request)
{
    if(_request != request) {
        _request = request;
        emit requestChanged();
    }
}

void QtFirebaseAdMobBanner::init()
{
    //qDebug() << "QtFirebase.AdMobBanner pre-initialize _ready" << _ready << "_init" << _initializing;
    if(!qFirebase->ready()) {
        qDebug() << this << "::init" << "base not ready";
        return;
    }

    if(!qFirebaseAdMob->ready()) {
        qDebug() << this << "::init" << "AdMob base not ready";
        return;
    }

    if(_adUnitId.isEmpty()) {
        qDebug() << this << "::init" << "adUnitId must be set in order to initialize the banner";
        return;
    }

    if(_isFirstInit && !PlatformUtils::getNativeWindow()) {
        qDebug() << this << "::init" << "no native ui element. Waiting for it...";
        return;
    }

    // TODO test nessecity of this
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

        _banner = new admob::BannerView();

        admob::AdSize ad_size;
        ad_size.ad_size_type = admob::kAdSizeStandard;
        ad_size.width = getWidth();
        ad_size.height = getHeight();

        // A reference to an iOS UIView or an Android Activity.
        // This is the parent UIView or Activity of the banner view.
        qDebug() << this << "::init initializing with AdUnitID" << __adUnitIdByteArray.constData();
        firebase::FutureBase future = _banner->Initialize(static_cast<admob::AdParent>(_nativeUIElement), __adUnitIdByteArray.constData(), ad_size);
        qDebug() << this << "::init" << "native initialized";
        qFirebase->addFuture(__QTFIREBASE_ID + ".banner.init",future);

    }
}

void QtFirebaseAdMobBanner::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    if(eventId == __QTFIREBASE_ID+".banner.init") {

        if (future.error() != admob::kAdMobErrorNone) {
            qDebug() << this << "::onFutureEvent" << "initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
            _initializing = false;
            return;
        }

        _initTimer->stop();
        delete _initTimer;

        qDebug() << this << "::onFutureEvent" << "initialized";
        _initializing = false;
        _isFirstInit = false;
        setReady(true);
    }

    if(eventId == __QTFIREBASE_ID+".banner.loaded") {

        if (future.error() != admob::kAdMobErrorNone) {
            int errorCode = future.error();
            qWarning() << this << "::onFutureEvent" << "load failed" << "ERROR" << "code:" << errorCode << "message:" << future.error_message();
            // TODO fix me
            emit error(qFirebaseAdMob->convertAdMobErrorCode(errorCode),QString(future.error_message()));
            return;
        }

        qDebug() << this << "::onFutureEvent loaded";
        setLoaded(true);
    }
}

void QtFirebaseAdMobBanner::onApplicationStateChanged(Qt::ApplicationState state)
{
    // NOTE makes sure the ad banner is on top of the Qt surface
    #if defined(__ANDROID__)
    if(state != Qt::ApplicationActive)
        hide();
    else
        show();
    #else
    Q_UNUSED(state);
    #endif
}

void QtFirebaseAdMobBanner::load()
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
    emit loading();
    admob::AdRequest request = _request->asAdMobRequest();
    firebase::FutureBase future = _banner->LoadAd(request);
    qFirebase->addFuture(__QTFIREBASE_ID + ".banner.loaded",future);
}


void QtFirebaseAdMobBanner::show()
{
    setVisible(true);
}

void QtFirebaseAdMobBanner::hide()
{
    setVisible(false);
}

void QtFirebaseAdMobBanner::moveTo(int x, int y)
{
    if(_ready) {
        qDebug() << this << "::moveTo moving to" << x << "," << y;
        _banner->MoveTo(x, y); // NOTE Potential dangerous code? The code below is more safe but will hang until the "I give up limit" is reached on some devices :/
        /*
        firebase::FutureBase future = _banner->MoveTo(x, y);
        qFirebase->waitForFutureCompletion(future); // TODO move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
        if(future.Error() != admob::kAdMobErrorNone) {
            qDebug() << this << "::moveTo ERROR code" << future.Error() << "message" << future.ErrorMessage();
            return;
        }
        */

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
}

void QtFirebaseAdMobBanner::moveTo(int position)
{
    if(!_ready) {
        qDebug() << this << "::moveTo" << "not ready";
        return;
    }
    // TODO reflect these changes in x,y coords. Probably use the Listener callback
    // NOTE Potential dangerous code?
    // The code below is more safe but will hang until the "I give up limit" is reached on some devices if using a waitforfuture... strategy :/
    if(position == PositionTopCenter) {
        qDebug() << this << "::moveTo position top-center";
        _banner->MoveTo(admob::BannerView::kPositionTop);
    } else if(position == PositionTopLeft) {
        qDebug() << this << "::moveTo position top-left";
        _banner->MoveTo(admob::BannerView::kPositionTopLeft);
    } else if(position == PositionTopRight) {
        qDebug() << this << "::moveTo position top-right";
        _banner->MoveTo(admob::BannerView::kPositionTopRight);
    } else if(position == PositionBottomCenter) {
        qDebug() << this << "::moveTo position bottom-center";
        _banner->MoveTo(admob::BannerView::kPositionBottom);
    } else if(position == PositionBottomLeft) {
        qDebug() << this << "::moveTo position bottom-left";
        _banner->MoveTo(admob::BannerView::kPositionBottomLeft);
    } else if(position == PositionBottomRight) {
        qDebug() << this << "::moveTo position bottom-right";
        _banner->MoveTo(admob::BannerView::kPositionBottomRight);
    } else {
        qDebug() << this << "::moveTo position unknown" << position;
    }

}

/*
 * AdMobInterstitial
 *
 */
QtFirebaseAdMobInterstitial::QtFirebaseAdMobInterstitial(QObject* parent) : QObject(parent)
{
    __QTFIREBASE_ID = QString().sprintf("%8p", this);
    _ready = false;
    _loaded = false;
    _initializing = false;
    _nativeUIElement = 0;
    _isFirstInit = true;
    _visible = false;
    _request = 0;

    connect(qFirebase,&QtFirebase::futureEvent, this, &QtFirebaseAdMobInterstitial::onFutureEvent);

    connect(this,&QtFirebaseAdMobInterstitial::presentationStateChanged, this, &QtFirebaseAdMobInterstitial::onPresentationStateChanged);

    _initTimer = new QTimer(this);
    _initTimer->setSingleShot(false);
    connect(_initTimer, &QTimer::timeout, this, &QtFirebaseAdMobInterstitial::init);
    _initTimer->start(500);

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

bool QtFirebaseAdMobInterstitial::ready()
{
    return _ready;
}

void QtFirebaseAdMobInterstitial::setReady(bool ready)
{
    if (_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

bool QtFirebaseAdMobInterstitial::loaded()
{
    return _loaded;
}

void QtFirebaseAdMobInterstitial::setLoaded(bool loaded)
{
    if(_loaded != loaded) {
        _loaded = loaded;
        emit loadedChanged();
    }
}

QString QtFirebaseAdMobInterstitial::adUnitId()
{
    return _adUnitId;
}

void QtFirebaseAdMobInterstitial::setAdUnitId(const QString &adUnitId)
{
    if(_adUnitId != adUnitId) {
        _adUnitId = adUnitId;
        __adUnitIdByteArray = _adUnitId.toLatin1();

        emit adUnitIdChanged();
    }
}

bool QtFirebaseAdMobInterstitial::visible() const
{
    return _visible;
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

QtFirebaseAdMobRequest *QtFirebaseAdMobInterstitial::request() const
{
    return _request;
}

void QtFirebaseAdMobInterstitial::setRequest(QtFirebaseAdMobRequest *request)
{
    if(_request != request) {
        _request = request;
        emit requestChanged();
    }
}

void QtFirebaseAdMobInterstitial::init()
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
        qDebug() << this << "::init" << "adUnitId must be set in order to initialize the interstitial";
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

        _interstitial = new admob::InterstitialAd();

        //__adUnitIdByteArray = _adUnitId.toLatin1();
        //__adUnitId = __adUnitIdByteArray.data();

        // A reference to an iOS UIView or an Android Activity.
        // This is the parent UIView or Activity of the Interstitial view.
        qDebug() << this << "::init initializing with AdUnitID" << __adUnitIdByteArray.constData();
        firebase::FutureBase future = _interstitial->Initialize(static_cast<admob::AdParent>(_nativeUIElement), __adUnitIdByteArray.constData());
        qFirebase->addFuture(__QTFIREBASE_ID + ".interstitial.init",future);
    }
}

void QtFirebaseAdMobInterstitial::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    if(eventId == __QTFIREBASE_ID+".interstitial.init") {

        if (future.error() != admob::kAdMobErrorNone) {
            qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
            _initializing = false;
            return;
        }

        _initTimer->stop();
        // NOTE don't delete it as we need it on iOS to re-initalize the whole mill after every load and show
        //delete _initTimer;

        qDebug() << this << "::onFutureEvent initialized";
        _initializing = false;
        _isFirstInit = false;

        // Add listener (can't be done before it's initialized)
        // TODO NOTE This is probably leaking memory - try to fix it without crashes appearing
        _interstitialAdListener = new QtFirebaseAdMobInterstitialAdListener(this);
        _interstitial->SetListener(_interstitialAdListener);

        setReady(true);
    }

    if(eventId == __QTFIREBASE_ID+".interstitial.loaded") {

        if (future.error() != admob::kAdMobErrorNone) {
            int errorCode = future.error();
            qWarning() << this << "::onFutureEvent" << "load failed" << "ERROR" << "code:" << errorCode << "message:" << future.error_message();
            // TODO fix me
            emit error(qFirebaseAdMob->convertAdMobErrorCode(errorCode),QString(future.error_message()));
            return;
        }

        qDebug() << this << "::onFutureEvent loaded";
        setLoaded(true);
    }
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

void QtFirebaseAdMobInterstitial::load()
{
    if(!_ready) {
        qDebug() << this << "::load() not ready";
        return;
    }
    
    if(_request == 0) {
        qDebug() << this << "::load() no request data sat. Not loading";
        return;
    }

    qDebug() << this << "::load() getting request data";
    emit loading();
    admob::AdRequest request = _request->asAdMobRequest();
    firebase::FutureBase future = _interstitial->LoadAd(request);
    qFirebase->addFuture(__QTFIREBASE_ID + ".interstitial.loaded",future);
}

void QtFirebaseAdMobInterstitial::show()
{
    if(!_ready) {
        qDebug() << this << "::show() not ready - so not showing";
        return;
    }

    if(!_loaded) {
        qDebug() << this << "::show() not loaded - so not showing";
        return;
    }

    firebase::FutureBase future = _interstitial->Show();
    qFirebase->waitForFutureCompletion(future); // TODO move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
    if(future.error() != admob::kAdMobErrorNone) {
        qDebug() << this << "::show ERROR code" << future.error() << "message" << future.error_message();
        return;
    }

}
/*
 * AdMobRewardedVideoAd
 *
 */

QtFirebaseAdMobRewardedVideoAd::QtFirebaseAdMobRewardedVideoAd(QObject* parent):
    QObject(parent),
    _ready(false),
    _loaded(false),
    _initializing(false),
    _isFirstInit(true),
    _visible(false),
    _request(nullptr),
    _nativeUIElement(nullptr)
{
    __QTFIREBASE_ID = QString().sprintf("%8p", this);

    connect(qFirebase, &QtFirebase::futureEvent, this, &QtFirebaseAdMobRewardedVideoAd::onFutureEvent);
    connect(this,&QtFirebaseAdMobRewardedVideoAd::presentationStateChanged, this, &QtFirebaseAdMobRewardedVideoAd::onPresentationStateChanged);

    _initTimer.setSingleShot(false);
    connect(&_initTimer, &QTimer::timeout, this, &QtFirebaseAdMobRewardedVideoAd::init);
    _initTimer.start(500);
}

QtFirebaseAdMobRewardedVideoAd::~QtFirebaseAdMobRewardedVideoAd()
{
    if(_ready) {
        firebase::admob::rewarded_video::Destroy();
        qDebug() << this << "::~QtFirebaseAdMobRewardedVideoAd" << "Destroyed";
    }
    _initTimer.stop();
}

bool QtFirebaseAdMobRewardedVideoAd::ready() const
{
    return _ready;
}

void QtFirebaseAdMobRewardedVideoAd::setReady(bool ready)
{
    if (_ready != ready) {
        _ready = ready;
        emit readyChanged();
    }
}

bool QtFirebaseAdMobRewardedVideoAd::loaded() const
{
    return _loaded;
}

void QtFirebaseAdMobRewardedVideoAd::setLoaded(bool loaded)
{
    if(_loaded != loaded) {
        _loaded = loaded;
        emit loadedChanged();
    }
}

QString QtFirebaseAdMobRewardedVideoAd::adUnitId() const
{
    return _adUnitId;
}

void QtFirebaseAdMobRewardedVideoAd::setAdUnitId(const QString &adUnitId)
{
    if(_adUnitId != adUnitId) {
        _adUnitId = adUnitId;
        __adUnitIdByteArray = _adUnitId.toLatin1();
        emit adUnitIdChanged();
    }
}

bool QtFirebaseAdMobRewardedVideoAd::visible() const
{
    return _visible;
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

QtFirebaseAdMobRequest *QtFirebaseAdMobRewardedVideoAd::request() const
{
    return _request;
}

void QtFirebaseAdMobRewardedVideoAd::setRequest(QtFirebaseAdMobRequest *request)
{
    if(_request != request) {
        _request = request;
        emit requestChanged();
    }
}

void QtFirebaseAdMobRewardedVideoAd::init()
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
        qDebug() << this << "::init" << "adUnitId must be set in order to initialize the interstitial";
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
        firebase::FutureBase future = firebase::admob::rewarded_video::Initialize();
        qFirebase->addFuture(__QTFIREBASE_ID + ".rewardedvideoad.init", future);
    }
}

void QtFirebaseAdMobRewardedVideoAd::onFutureEvent(QString eventId, firebase::FutureBase future)
{
    if(!eventId.startsWith(__QTFIREBASE_ID))
        return;

    qDebug()<<this<<"QtFirebaseAdMobInterstitial::onFutureEvent";

    if(eventId == __QTFIREBASE_ID+".rewardedvideoad.init")
    {
        if (future.error() != admob::kAdMobErrorNone)
        {
            qDebug() << this << "::onFutureEvent initializing failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
            _initializing = false;
        }
        else
        {
            _initTimer.stop();
            qDebug() << this << "::onFutureEvent initialized";
            _initializing = false;
            _isFirstInit = false;
            firebase::admob::rewarded_video::SetListener(this);
            setReady(true);
        }
    }
    else if(eventId == __QTFIREBASE_ID+".rewardedvideoad.loaded")
    {
        int errorCode = future.error();
        if (future.error() != admob::kAdMobErrorNone)
        {
            QString errorMessage = future.error_message();
            QtFirebaseAdMob::Error qtFirebaseErrorCode = qFirebaseAdMob->convertAdMobErrorCode(errorCode);
            qWarning() << this << "::onFutureEvent" << "load failed" << "ERROR" << "code:" << errorCode << "message:" << errorMessage;
            emit error(static_cast<int>(qtFirebaseErrorCode), errorMessage);
        }
        else
        {
            qDebug() << this << "::onFutureEvent loaded";
            setLoaded(true);
        }
    }
    future.Release();
}

void QtFirebaseAdMobRewardedVideoAd::onPresentationStateChanged(int state)
{
    if(state == QtFirebaseAdMobInterstitial::PresentationStateCoveringUI) {
        if(!_visible) {
            _visible = true;
            emit visibleChanged();
        }
    }

    if(state == QtFirebaseAdMobInterstitial::PresentationStateHidden) {
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
}

void QtFirebaseAdMobRewardedVideoAd::load()
{
    if(!_ready) {
        qDebug() << this << "::load() not ready";
        return;
    }

    if(_request == 0) {
        qDebug() << this << "::load() no request data sat. Not loading";
        return;
    }

    qDebug() << this << "::load() getting request data";
    emit loading();
    admob::AdRequest request = _request->asAdMobRequest();
    firebase::FutureBase future = firebase::admob::rewarded_video::LoadAd( __adUnitIdByteArray.constData(), request);
    qFirebase->addFuture(__QTFIREBASE_ID + ".rewardedvideoad.loaded",future);
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
    qFirebase->waitForFutureCompletion(future); // TODO move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
    if(future.error() != admob::kAdMobErrorNone) {
        qDebug() << this << "::show ERROR code" << future.error() << "message" << future.error_message();
        return;
    }
}

void QtFirebaseAdMobRewardedVideoAd::OnRewarded(firebase::admob::rewarded_video::RewardItem reward)
{
    QString type = reward.reward_type.c_str();
    qDebug()<<this<<QString("Rewarding user of %1 with amount %2")
              .arg(type)
              .arg(QString::number(reward.amount));

    emit rewarded(type, reward.amount);

}
void QtFirebaseAdMobRewardedVideoAd::OnPresentationStateChanged(firebase::admob::rewarded_video::PresentationState state)
{
    if(state == firebase::admob::rewarded_video::kPresentationStateHidden)
    {
        qDebug()<<this<<"kPresentationStateHidden";
    }
    else if(state == firebase::admob::rewarded_video::kPresentationStateCoveringUI)
    {
        qDebug()<<this<<"kPresentationStateCoveringUI";
    }
    else if(state == firebase::admob::rewarded_video::kPresentationStateVideoHasStarted)
    {
        qDebug()<<this<<"kPresentationStateVideoHasStarted";
    }

    int pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;

    if(state == firebase::admob::rewarded_video::kPresentationStateHidden) {
        pState = QtFirebaseAdMobInterstitial::PresentationStateHidden;
    } else if(state == firebase::admob::rewarded_video::kPresentationStateCoveringUI) {
         pState = QtFirebaseAdMobInterstitial::PresentationStateCoveringUI;
    }
    emit presentationStateChanged(pState);
}

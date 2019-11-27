#include "qtfirebaseadmobbanner.h"

/*
 * Admob banner base.
 */
QtFirebaseAdMobBannerBase::QtFirebaseAdMobBannerBase(QObject *parent)
    : QtFirebaseAdMobBase(parent)
    , _x(0)
    , _y(0)
    , _width(0)
    , _height(0)
{
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
                if(completed_future.error() != firebase::admob::kAdMobErrorNone) {
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
            if(completed_future.error() != firebase::admob::kAdMobErrorNone) {
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
                if(completed_future.error() != firebase::admob::kAdMobErrorNone) {
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
 */
QtFirebaseAdMobBanner::QtFirebaseAdMobBanner(QObject *parent)
    : QtFirebaseAdMobBannerBase(parent)
    , _banner(nullptr)
    , _bannerViewListener(nullptr)
    , m_boundingBox()
    , m_presentationState()
{

}

QtFirebaseAdMobBanner::~QtFirebaseAdMobBanner()
{
    if(_ready) {
        _banner->SetListener(nullptr);
        delete _bannerViewListener;

        _banner->Destroy();
        qFirebase->waitForFutureCompletion(_banner->DestroyLastResult()); // TODO MAYBE move or duplicate to QtFirebaseAdMob with admob::kAdMobError* checking? (Will save ALOT of cycles on errors)
        qDebug() << this << "::~QtFirebaseAdMobBanner" << "Destroyed banner";

        delete _banner;
    }
}

firebase::FutureBase QtFirebaseAdMobBanner::setVisibleInternal(bool visible)
{
    return visible ? _banner->Show() : _banner->Hide();
}

void QtFirebaseAdMobBanner::onInitialized()
{
    // Add listener (can't be done before it's initialized)
    auto cpy = _bannerViewListener;
    _bannerViewListener = new QtFirebaseAdMobBannerViewListener(this);
    _banner->SetListener(_bannerViewListener);
    if(cpy) {
        delete cpy;
    }
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
    _banner = new firebase::admob::BannerView();

    firebase::admob::AdSize ad_size;
    ad_size.ad_size_type = firebase::admob::kAdSizeStandard;
    ad_size.width = getWidth();
    ad_size.height = getHeight();

    // A reference to an iOS UIView or an Android Activity.
    // This is the parent UIView or Activity of the banner view.
    qDebug() << this << "::init initializing with AdUnitID" << __adUnitIdByteArray.constData();
    return _banner->Initialize(static_cast<firebase::admob::AdParent>(_nativeUIElement), __adUnitIdByteArray.constData(), ad_size);
}

firebase::FutureBase QtFirebaseAdMobBanner::loadInternal()
{
    firebase::admob::AdRequest request = _request->asAdMobRequest();
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
        return _banner->MoveTo(firebase::admob::BannerView::kPositionTop);
    case PositionTopLeft:
        qDebug() << this << "::moveTo position top-left";
        return _banner->MoveTo(firebase::admob::BannerView::kPositionTopLeft);
    case PositionTopRight:
        qDebug() << this << "::moveTo position top-right";
        return _banner->MoveTo(firebase::admob::BannerView::kPositionTopRight);
    case PositionBottomCenter:
        qDebug() << this << "::moveTo position bottom-center";
        return _banner->MoveTo(firebase::admob::BannerView::kPositionBottom);
    case PositionBottomLeft:
        qDebug() << this << "::moveTo position bottom-left";
        return _banner->MoveTo(firebase::admob::BannerView::kPositionBottomLeft);
    case PositionBottomRight:
        qDebug() << this << "::moveTo position bottom-right";
        return _banner->MoveTo(firebase::admob::BannerView::kPositionBottomRight);
    }

    qDebug() << this << "::moveTo position unknown" << position;
    return firebase::FutureBase();
}

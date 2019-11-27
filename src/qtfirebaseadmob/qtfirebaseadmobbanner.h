#ifndef QTFIREBASE_ADMOB_BANNER_H
#define QTFIREBASE_ADMOB_BANNER_H

#include "src/qtfirebase.h"

#include "qtfirebaseadmob.h"
#include "qtfirebaseadmobbase.h"
#include "qtfirebaseadmobrequest.h"

#include "firebase/admob/banner_view.h"

class QtFirebaseAdMobBannerBase : public QtFirebaseAdMobBase
{
    Q_OBJECT

    Q_PROPERTY(int x READ getX WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ getY WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ getHeight WRITE setHeight NOTIFY heightChanged)

public:
    enum Position
    {
        PositionTopCenter,
        PositionTopLeft,
        PositionTopRight,
        PositionBottomCenter,
        PositionBottomLeft,
        PositionBottomRight
    };
    Q_ENUM(Position)

    explicit QtFirebaseAdMobBannerBase(QObject *parent = nullptr);


    virtual int getX() const;
    virtual void setX(int x);

    virtual int getY() const;
    virtual void setY(int y);

    virtual int getWidth() const;
    virtual void setWidth(int width);

    virtual int getHeight() const;
    virtual void setHeight(int height);

    virtual void setVisible(bool visible) override;

public slots:
    virtual void show() override;
    virtual void hide();
    virtual void moveTo(int x, int y);
    virtual void moveTo(Position position);

protected slots:
    virtual void onApplicationStateChanged(Qt::ApplicationState state);

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();

protected:
    virtual firebase::FutureBase moveToInternal(int x, int y) = 0;
    virtual firebase::FutureBase moveToInternal(Position position) = 0;

    virtual firebase::FutureBase setXInternal(int x) = 0;
    virtual firebase::FutureBase setYInternal(int y) = 0;

    virtual firebase::FutureBase setVisibleInternal(bool visible) = 0;

protected:
    int _x;
    int _y;
    int _width;
    int _height;

};

/*
 * AdMobBanner
 */
class QtFirebaseAdMobBannerViewListener;

class QtFirebaseAdMobBanner : public QtFirebaseAdMobBannerBase
{
    Q_OBJECT
    Q_PROPERTY(QRect boundingBox READ boundingBox NOTIFY boundingBoxChanged)
    Q_PROPERTY(PresentationState presentationState READ presentationState NOTIFY presentationStateChanged)

    friend class QtFirebaseAdMobBannerViewListener;
public:
    enum PresentationState {
        PresentationStateHidden = firebase::admob::BannerView::kPresentationStateHidden,
        PresentationStateVisibleWithoutAd = firebase::admob::BannerView::kPresentationStateVisibleWithoutAd,
        PresentationStateVisibleWithAd = firebase::admob::BannerView::kPresentationStateVisibleWithAd,
        PresentationStateOpenedPartialOverlay = firebase::admob::BannerView::kPresentationStateOpenedPartialOverlay,
        PresentationStateCoveringUI = firebase::admob::BannerView::kPresentationStateCoveringUI
    };
    Q_ENUM(PresentationState)

    QtFirebaseAdMobBanner(QObject* parent = nullptr);
    ~QtFirebaseAdMobBanner() override;

    QRect boundingBox() const
    {
        return m_boundingBox;
    }

    PresentationState presentationState() const
    {
        return m_presentationState;
    }

signals:
    void presentationStateChanged(PresentationState state);
    void boundingBoxChanged(const QRect& boundingBox);

private:
    firebase::FutureBase initInternal() override;
    firebase::FutureBase loadInternal() override;

    firebase::FutureBase moveToInternal(int x, int y) override;
    firebase::FutureBase moveToInternal(Position position) override;

    firebase::FutureBase setXInternal(int x) override;
    firebase::FutureBase setYInternal(int y) override;

    firebase::FutureBase setVisibleInternal(bool visible) override;
    void onInitialized() override;

    void setBoundingBox(QRect boundingBox)
    {
        if (m_boundingBox == boundingBox)
            return;

        m_boundingBox = boundingBox;
        emit boundingBoxChanged(m_boundingBox);
    }

    void setPresentationState(PresentationState presentationState)
    {
        if (m_presentationState == presentationState)
            return;

        m_presentationState = presentationState;
        emit presentationStateChanged(m_presentationState);
    }

private:
    firebase::admob::BannerView* _banner;
    QtFirebaseAdMobBannerViewListener* _bannerViewListener;
    QRect m_boundingBox;
    PresentationState m_presentationState;
};

// A listener class to an BannerView.
class QtFirebaseAdMobBannerViewListener : public firebase::admob::BannerView::Listener {

    friend class QtFirebaseAdMobBanner;

public:
    QtFirebaseAdMobBannerViewListener(QtFirebaseAdMobBanner* qtFirebaseAdMobBanner) {
        _qtFirebaseAdMobBanner = qtFirebaseAdMobBanner;
    }

    void OnPresentationStateChanged(firebase::admob::BannerView* banner_view, firebase::admob::BannerView::PresentationState state) override {
        Q_UNUSED(banner_view)

        qDebug() << _qtFirebaseAdMobBanner << "::OnPresentationStateChanged";
        qDebug("BannerView PresentationState has changed to %d.", state);

        switch(state) {
            case firebase::admob::BannerView::kPresentationStateHidden:
                qDebug("BannerView PresentationState has changed to kPresentationStateHidden");
                break;
            case firebase::admob::BannerView::kPresentationStateVisibleWithoutAd:
                qDebug("BannerView PresentationState has changed to PresentationStateVisibleWithoutAd");
                break;
            case firebase::admob::BannerView::kPresentationStateVisibleWithAd:
                qDebug("BannerView PresentationState has changed to PresentationStateVisibleWithAd");
                break;
            case firebase::admob::BannerView::kPresentationStateOpenedPartialOverlay:
                qDebug("BannerView PresentationState has changed to PresentationStateOpenedPartialOverlay");
                break;
            case firebase::admob::BannerView::kPresentationStateCoveringUI:
                qDebug("BannerView PresentationState has changed to PresentationStateCoveringUI");
                break;
        }

        QMetaObject::invokeMethod(_qtFirebaseAdMobBanner, [this, state]() {
            _qtFirebaseAdMobBanner->setPresentationState( static_cast<QtFirebaseAdMobBanner::PresentationState>(state) );
        });
    }

    void OnBoundingBoxChanged(firebase::admob::BannerView *banner_view, firebase::admob::BoundingBox box) override {
        Q_UNUSED(banner_view)
        qDebug() << _qtFirebaseAdMobBanner << "::OnBoundingBoxChanged";
        QRect boundingBox(box.x, box.y, box.width, box.height);
        qDebug() << "BannerView BoundingBox has changed to" << boundingBox;

        QMetaObject::invokeMethod(_qtFirebaseAdMobBanner, [this, boundingBox]() {
            _qtFirebaseAdMobBanner->setBoundingBox(boundingBox);
        });
    }

private:
    QtFirebaseAdMobBanner* _qtFirebaseAdMobBanner;
};

#endif // QTFIREBASE_ADMOB_BANNER_H

#include "platformutils.h"

#import <UIKit/UIKit.h>

PlatformUtils::PlatformUtils()
{

}

void* PlatformUtils::getNativeWindow()
{

    //QWindow *window = QGuiApplication::topLevelAt(QPoint(1,1));
    // Hints from https://forum.qt.io/topic/46297/qt-5-3-2-qml-on-ios-8-cannot-get-uiview-from-qquickwindow-anymore-solved
    //UIView *view = static_cast<UIView *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", window));
    //UIView *view = static_cast<UIView *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", QGuiApplication::focusWindow()));
    UIView *view = (__bridge UIView *)reinterpret_cast<void *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", QGuiApplication::focusWindow()));
    //UIView *view = (__bridge UIView *)reinterpret_cast<void *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", window));

    //UIView* view = (__bridge UIView*)reinterpret_cast<void*>(window->winId());
    //qDebug() << "Getting UIView" << view;

    return (__bridge void *)reinterpret_cast<UIView *>(view);

    //return QGuiApplication::platformNativeInterface()->nativeResourceForWindow("uiview", QGuiApplication::focusWindow());
}

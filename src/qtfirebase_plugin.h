#ifndef QTFIREBASE_PLUGIN_H
#define QTFIREBASE_PLUGIN_H

#include <QQmlExtensionPlugin>

class QtFirebasePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "QtFirebase")

public:
    void registerTypes(const char *uri);
};

#endif // QTFIREBASE_PLUGIN_H

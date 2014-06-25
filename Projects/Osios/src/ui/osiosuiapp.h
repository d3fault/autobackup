#ifndef OSIOSUIAPP_H
#define OSIOSUIAPP_H

#include <QObject>

class IOsiosUiFactory;
class ObjectOnThreadGroup;

class OsiosUiApp : public QObject
{
    Q_OBJECT
public:
    explicit OsiosUiApp(IOsiosUiFactory *uiFactory, QObject *parent = 0);
    ~OsiosUiApp();
private:
    IOsiosUiFactory *m_UiFactory;
    ObjectOnThreadGroup *m_OsiosThread;
public slots:
    void handleOsiosReadyForConnections(QObject *osiosAsQObject);
};

#endif // OSIOSUIAPP_H

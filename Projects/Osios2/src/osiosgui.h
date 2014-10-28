#ifndef OSIOSGUI_H
#define OSIOSGUI_H

#include <QObject>

class Osios;
class OsiosMainWindow;

class OsiosGui : public QObject
{
    Q_OBJECT
public:
    explicit OsiosGui(QObject *parent = 0);
    ~OsiosGui();
private:
    Osios *m_Osios;
    OsiosMainWindow *m_MainWindow;

    void usageAndQuit();
    void quit();
    void connectBackendToAndFromFrontendSignalsAndSlots();
    void showMainWindow();
};

#endif // OSIOSGUI_H

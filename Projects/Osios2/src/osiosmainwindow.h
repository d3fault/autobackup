#ifndef OSIOSMAINWINDOW_H
#define OSIOSMAINWINDOW_H

#include <QMainWindow>
#include "iosiosclient.h"

#include "osios.h"

class QTabWidget;

class Osios;

class OsiosMainWindow : public QMainWindow, IOsiosClient /* the ONLY actual osios client. all other implementers (so far (one other actual client would be the cli app)) are simply relaying signals to this one */
{
    Q_OBJECT
public:
    OsiosMainWindow(Osios *osios, QWidget *parent = 0);
    QObject *asQObject();
private:
    QTabWidget *m_MainMenuItemsTabWidget;
signals:
    void actionOccurred(const ITimelineNode &action);
private slots:
    void handleMainMenuItemsTabWidgetCurrentTabChanged();
};

#endif // OSIOSMAINWINDOW_H

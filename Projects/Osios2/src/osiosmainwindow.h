#ifndef OSIOSMAINWINDOW_H
#define OSIOSMAINWINDOW_H

#include <QMainWindow>
#include "iosiosclient.h"

#include "osios.h"
#include "itimelinenode.h"

class QTabWidget;

class Osios;

class OsiosMainWindow : public QMainWindow, IOsiosClient /* the ONLY actual osios client. all other implementers (so far (one other actual client would be the cli app)) are simply relaying signals to this one */
{
    Q_OBJECT
public:
    OsiosMainWindow(Osios *osios, QWidget *parent = 0);
    QObject *asQObject();
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    QTabWidget *m_MainMenuItemsTabWidget;
signals:
    void actionOccurred(TimelineNode action);
public slots:
    void changeConnectionColor(int color);
    void presentNotification(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel = OsiosNotificationLevels::StandardNotificationLevel);
private slots:
    void handleMainMenuItemsTabWidgetCurrentTabChanged();
};

#endif // OSIOSMAINWINDOW_H

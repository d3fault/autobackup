#ifndef OSIOSMAINWINDOW_H
#define OSIOSMAINWINDOW_H

#include <QMainWindow>

#include "osios.h"

class QTabWidget;

class OsiosMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    OsiosMainWindow(QWidget *parent = 0);
private:
    QTabWidget *m_MainMenuItemsTabWidget;
signals:
    void actionOccurred(const ITimelineNode &action);
private slots:
    void handleMainMenuItemsTabWidgetCurrentTabChanged();
};

#endif // OSIOSMAINWINDOW_H

#ifndef ADDOUBLESRAWMATHLAUNCHSIMGUI_H
#define ADDOUBLESRAWMATHLAUNCHSIMGUI_H

#include <QObject>
#include <QCoreApplication>

#include "addoublesrawmathlaunchsim.h"
#include "addoublesrawmathlaunchsimwidget.h"
#include "objectonthreadhelper.h"

class AdDoublesRawMathLaunchSimGui : public QObject
{
    Q_OBJECT
public:
    explicit AdDoublesRawMathLaunchSimGui(QObject *parent = 0);
private:
    AdDoublesRawMathLaunchSimWidget m_Gui;
    ObjectOnThreadHelper<AdDoublesRawMathLaunchSim> m_BusinessThread;
public slots:
    void handleAdDoublesRawMathLaunchSimReadyForConnections();
    void handleAboutToQuit();
};

#endif // ADDOUBLESRAWMATHLAUNCHSIMGUI_H

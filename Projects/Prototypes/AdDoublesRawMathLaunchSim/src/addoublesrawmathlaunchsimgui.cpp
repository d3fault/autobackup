#include "addoublesrawmathlaunchsimgui.h"

AdDoublesRawMathLaunchSimGui::AdDoublesRawMathLaunchSimGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleAdDoublesRawMathLaunchSimReadyForConnections()));
    m_BusinessThread.start();
}
void AdDoublesRawMathLaunchSimGui::handleAdDoublesRawMathLaunchSimReadyForConnections()
{
    AdDoublesRawMathLaunchSim *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(slotAvailableAtPrice(uint,double)), &m_Gui, SLOT(handleSlotAvailableAtPrice(uint,double)));
    connect(business, SIGNAL(slotFillerForSaleChanged(uint)), &m_Gui, SLOT(handleSlotFillerForSaleChanged(uint)));
    connect(business, SIGNAL(slotFillerOnDisplayChanged(int)), &m_Gui, SLOT(handleSlotFillerOnDisplayChanged(int)));
    connect(&m_Gui, SIGNAL(launchRequested(double,uint)), business, SLOT(launch(double,uint)));
    connect(&m_Gui, SIGNAL(buyAttemptRequested(uint,double)), business, SLOT(buyAttempt(uint,double)));
    connect(&m_Gui, SIGNAL(stopRequested()), business, SLOT(stop()));
    m_Gui.show();
}
void AdDoublesRawMathLaunchSimGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

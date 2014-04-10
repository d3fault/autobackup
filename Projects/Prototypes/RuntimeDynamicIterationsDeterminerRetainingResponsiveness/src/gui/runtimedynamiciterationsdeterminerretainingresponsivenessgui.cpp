#include "runtimedynamiciterationsdeterminerretainingresponsivenessgui.h"

#include <QCoreApplication>

RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui::RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui(QObject *parent) :
    QObject(parent)
{
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRuntimeDynamicIterationsDeterminerRetainingResponsivenessReadyForConnections()));
    m_BusinessThread.start();

    //darn, if only QThread::wait was a slot too (yea it'd need to be invoke via direct connection, but whatever) :-P
    //connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), &m_BusinessThread, SLOT(quit()));
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}
void RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui::handleRuntimeDynamicIterationsDeterminerRetainingResponsivenessReadyForConnections()
{
    RuntimeDynamicIterationsDeterminerRetainingResponsiveness *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(determineIterationsToRetainResponsivenessWithinRequested(qint64)), business, SLOT(determineIterationsToRetainResponsivenessWithin(qint64)));
    connect(&m_Gui, SIGNAL(startDoingWorkUsingDeterminedIterationsRequested()), business, SLOT(startDoingWorkUtilizingDeterminedNumberOfIterations()));
    connect(&m_Gui, SIGNAL(pingRequested()), business, SLOT(ping()));
    connect(business, SIGNAL(pong()), &m_Gui, SLOT(handlePong()));
    connect(&m_Gui, SIGNAL(stopDoingWorkUsingDeterminedIterationsRequested()), business, SLOT(stopDoingWork()));

    m_Gui.show();
}
void RuntimeDynamicIterationsDeterminerRetainingResponsivenessGui::handleAboutToQuit()
{
    //this isn't much harder, just gets boring to type
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

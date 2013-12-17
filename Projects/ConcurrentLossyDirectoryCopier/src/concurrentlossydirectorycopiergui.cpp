#include "concurrentlossydirectorycopiergui.h"

ConcurrentLossyDirectoryCopierGui::ConcurrentLossyDirectoryCopierGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleConcurrentLossyDirectoryCopierReadyForConnections()));
    m_BusinessThread.start();
}
void ConcurrentLossyDirectoryCopierGui::handleConcurrentLossyDirectoryCopierReadyForConnections()
{
    ConcurrentLossyDirectoryCopier *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(business, SIGNAL(progressMinsAndMaxesCalculated(int,int)), &m_Gui, SLOT(recordUpdatedProgressMinsAndMaxes(int,int)));
    connect(business, SIGNAL(progressValueUpdated(int)), &m_Gui, SLOT(updateProgressValue(int)));
    connect(&m_Gui, SIGNAL(lossilyCopyDirectoryUsingAllAvailableCoresRequested(QString,QString)), business, SLOT(lossilyCopyDirectoryUsingAllAvailableCores(QString,QString)));
    connect(&m_Gui, SIGNAL(cancelAfterAllCurrentlyEncodingVideosFinishRequested()), business, SLOT(cancelAfterAllCurrentlyEncodingVideosFinish()));

    m_Gui.show();
}
void ConcurrentLossyDirectoryCopierGui::handleAboutToQuit()
{
    //ehh TODOreq this won't work with current design. somehow need to cancel the future (rofl read that out loud)
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

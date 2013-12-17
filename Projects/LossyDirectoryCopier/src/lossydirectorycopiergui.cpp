#include "lossydirectorycopiergui.h"

LossyDirectoryCopierGui::LossyDirectoryCopierGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleLossyDirectoryCopierReadyForConnections()));
    m_BusinessThread.start();
}
void LossyDirectoryCopierGui::handleLossyDirectoryCopierReadyForConnections()
{
    LossyDirectoryCopier *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(lossilyCopyEveryDirectoryEntryAndJustCopyWheneverCantCompressRequested(QString,QString)), business, SLOT(lossilyCopyEveryDirectoryEntryAndJustCopyWheneverCantCompress(QString,QString)));
    connect(&m_Gui, SIGNAL(stopRequested()), business, SLOT(requestStopAfterCurrentFileIsDone()));

    m_Gui.show();
}
void LossyDirectoryCopierGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

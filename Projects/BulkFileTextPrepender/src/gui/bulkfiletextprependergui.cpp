#include "bulkfiletextprependergui.h"

BulkFileTextPrependerGui::BulkFileTextPrependerGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleBulkFileTextPrependerReadyForConnections()));
    m_BusinessThread.start();
}
void BulkFileTextPrependerGui::handleBulkFileTextPrependerReadyForConnections()
{
    BulkFileTextPrepender *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(prependStringToBeginningOfAllTextFilesInDirRequested(QString,QString,QStringList)), business, SLOT(prependStringToBeginningOfAllTextFilesInDir(QString,QString,QStringList)));

    m_Gui.show();
}
void BulkFileTextPrependerGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

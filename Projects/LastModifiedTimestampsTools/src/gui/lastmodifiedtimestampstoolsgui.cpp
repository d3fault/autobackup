#include "lastmodifiedtimestampstoolsgui.h"

LastModifiedTimestampsToolsGui::LastModifiedTimestampsToolsGui(QObject *parent)
    : QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleSimplifiedHeirarchyMolesterReadyForConnections()));
    m_BusinessThread.start();
}
void LastModifiedTimestampsToolsGui::handleSimplifiedHeirarchyMolesterReadyForConnections()
{
    LastModifiedTimestampsTools *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(lastModifiedTimestampsFileGenerationRequested(QString,QString)), business, SLOT(beginGenerateLastModifiedTimestampsFile(QString,QString)));
    connect(business, SIGNAL(lastModifiedTimestampsFileGenerated(bool)), &m_Gui, SLOT(reEnableStartButton()));
    connect(&m_Gui, SIGNAL(heirarchyMolestationRequested(QString,QString)), business, SLOT(beginMolestHeirarchy(QString,QString)));
    connect(business, SIGNAL(heirarchyMolested(bool)), &m_Gui, SLOT(reEnableStartButton()));
    m_Gui.show();
}
void LastModifiedTimestampsToolsGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

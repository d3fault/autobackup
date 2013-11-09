#include "lastmodifieddateheirarchymolestergui.h"

LastModifiedDateHeirarchyMolesterGui::LastModifiedDateHeirarchyMolesterGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleLastModifiedDateHeirarchyMolesterReadyForConnections()));
    m_BusinessThread.start();
}
void LastModifiedDateHeirarchyMolesterGui::handleLastModifiedDateHeirarchyMolesterReadyForConnections()
{
    LastModifiedDateHeirarchyMolester *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(&m_Gui, SIGNAL(lastModifiedDateHeirarchyMolestationRequested(QString,QString,bool)), business, SLOT(molestLastModifiedDateHeirarchy(QString,QString,bool)));
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));

    m_Gui.show();
}
void LastModifiedDateHeirarchyMolesterGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

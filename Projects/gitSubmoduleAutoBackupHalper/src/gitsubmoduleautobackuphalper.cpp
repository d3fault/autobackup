#include "gitsubmoduleautobackuphalper.h"

GitSubmoduleAutoBackupHalper::GitSubmoduleAutoBackupHalper(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_Business, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleGitSubmoduleAutoBackupHalperBusinessInstantiated()));
}
void GitSubmoduleAutoBackupHalper::handleGitSubmoduleAutoBackupHalperBusinessInstantiated()
{
    GitSubmoduleAutoBackupHalperBusiness *business = m_Business.getObjectPointerForConnectionsOnly();

    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));



    m_Gui.show();
}
void GitSubmoduleAutoBackupHalper::handleAboutToQuit()
{
    m_Business.quit();
    m_Business.wait();
}

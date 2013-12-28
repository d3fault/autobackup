#include "recursivefilestringreplacerenamergui.h"

RecursiveFileStringReplaceRenamerGui::RecursiveFileStringReplaceRenamerGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRecursiveFileStringReplaceRenamerReadyForConnections()));
    m_BusinessThread.start();
}
void RecursiveFileStringReplaceRenamerGui::handleRecursiveFileStringReplaceRenamerReadyForConnections()
{
    RecursiveFileStringReplaceRenamer *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(recursivelyRenameFilesDoingSimpleStringReplaceRequested(QString,QString,QString)), business, SLOT(recursivelyRenameFilesDoingSimpleStringReplace(QString,QString,QString)));

    m_Gui.show();
}
void RecursiveFileStringReplaceRenamerGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

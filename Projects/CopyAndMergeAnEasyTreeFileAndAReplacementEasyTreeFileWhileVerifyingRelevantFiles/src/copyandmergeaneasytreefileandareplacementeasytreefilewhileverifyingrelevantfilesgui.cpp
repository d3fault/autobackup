#include "copyandmergeaneasytreefileandareplacementeasytreefilewhileverifyingrelevantfilesgui.h"

CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesGui::CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesGui(QObject *parent) :
    QObject(parent)
{
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleCopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesInstantiated()));
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    m_BusinessThread.start();
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesGui::handleCopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesInstantiated()
{
    CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles *business = m_BusinessThread.getObjectPointerForConnectionsOnly();

    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesRequested(QString,QString,QString,QString,QString)), business, SLOT(copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFiles(QString,QString,QString,QString,QString)));

    m_Gui.show();
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

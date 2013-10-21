#include "easytreegui.h"

EasyTreeGui::EasyTreeGui(QObject *parent) :
    QObject(parent)
{
    connect(&m_EasyTreeGuiBusinessThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleEasyTreeGuiBusinessInstantiated()));
    m_EasyTreeGuiBusinessThreadHelper.start();

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}
void EasyTreeGui::handleEasyTreeGuiBusinessInstantiated()
{
    EasyTreeGuiBusiness *easyTreeGuiBusiness = m_EasyTreeGuiBusinessThreadHelper.getObjectPointerForConnectionsOnly();

    connect(&m_EasyTreeGuiWidget, SIGNAL(generateEasyTreeFileRequested(QString,QString,bool)), easyTreeGuiBusiness, SLOT(generateEasyTreeFile(QString,QString,bool)));
    connect(easyTreeGuiBusiness, SIGNAL(doneGeneratingEasyTreeFile()), &m_EasyTreeGuiWidget, SLOT(handleEasyTreeGuiBusinessFinishedGeneratingEasyTreeFile()));
    connect(easyTreeGuiBusiness, SIGNAL(d(QString)), &m_EasyTreeGuiWidget, SLOT(handleD(QString)));

    //in other projects I had this show() call in teh constructor above... but it makes more sense to have it here because we don't want to show the user anything if there is nothing to interact with (the business is the core of the app after all)
    m_EasyTreeGuiWidget.show();
}
void EasyTreeGui::handleAboutToQuit()
{
    m_EasyTreeGuiBusinessThreadHelper.quit();
    m_EasyTreeGuiBusinessThreadHelper.wait();
}

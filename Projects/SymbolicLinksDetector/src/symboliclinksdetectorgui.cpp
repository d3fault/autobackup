#include "symboliclinksdetectorgui.h"

SymbolicLinksDetectorGui::SymbolicLinksDetectorGui(QObject *parent) :
    QObject(parent)
{
    connect(&m_Business, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleSymbolicLinksDetectorBusinessInstantiated()));
    m_Business.start();

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}
void SymbolicLinksDetectorGui::handleSymbolicLinksDetectorBusinessInstantiated()
{
    SymbolicLinksDetectorBusiness *symbolicLinksDetectorBusiness = m_Business.getObjectPointerForConnectionsOnly();
    connect(symbolicLinksDetectorBusiness, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(startDetectingSymbolicLinksRequested(QString,bool)), symbolicLinksDetectorBusiness, SLOT(detectSymbolicLinks(QString,bool)));

    m_Gui.show();
}
void SymbolicLinksDetectorGui::handleAboutToQuit()
{
    m_Business.quit();
    m_Business.wait();
}

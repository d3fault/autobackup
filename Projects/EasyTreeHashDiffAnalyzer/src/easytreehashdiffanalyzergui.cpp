#include "easytreehashdiffanalyzergui.h"

EasyTreeHashDiffAnalyzerGui::EasyTreeHashDiffAnalyzerGui(QObject *parent) :
    QObject(parent)
{
    connect(&m_Business, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleEasyTreeHashDiffAnalyzerBusinessInstantiated()));
    m_Business.start();
}
void EasyTreeHashDiffAnalyzerGui::handleEasyTreeHashDiffAnalyzerBusinessInstantiated()
{
    EasyTreeHashDiffAnalyzerBusiness *easyTreeHashDiffAnalyzerBusiness = m_Business.getObjectPointerForConnectionsOnly();

    connect(easyTreeHashDiffAnalyzerBusiness, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(diffAndAnalyzeRequested(QString,QString)), easyTreeHashDiffAnalyzerBusiness, SLOT(diffAndAnalyze(QString,QString)));

    m_Gui.show();
}

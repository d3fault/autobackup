#include "easytreetotalsizecalculatorgui.h"

EasyTreeTotalSizeCalculatorGui::EasyTreeTotalSizeCalculatorGui(QObject *parent) :
    QObject(parent)
{
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleEasyTreeTotalSizeCalculatorBusinessInstantiated()));
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    m_BusinessThread.start();
}
void EasyTreeTotalSizeCalculatorGui::handleEasyTreeTotalSizeCalculatorBusinessInstantiated()
{
    EasyTreeTotalSizeCalculator *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(&m_Gui, SIGNAL(calculateTotalSizeOfEasyTreeEntriesRequested(QString,bool)), business, SLOT(calculateTotalSizeOfEasyTreeEntries(QString,bool)));
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));

    m_Gui.show();
}
void EasyTreeTotalSizeCalculatorGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}

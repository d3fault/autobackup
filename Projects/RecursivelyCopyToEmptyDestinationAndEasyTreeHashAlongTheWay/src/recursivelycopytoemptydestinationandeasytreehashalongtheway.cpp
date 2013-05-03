#include "recursivelycopytoemptydestinationandeasytreehashalongtheway.h"

RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay::RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(QObject *parent) :
    QObject(parent)
{
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleBusinessInstantiated()));
    m_BusinessThread.start();

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}
void RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay::handleBusinessInstantiated()
{
    RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWayBusiness *business = m_BusinessThread.getObjectPointerForConnectionsOnly();

    connect(&m_Gui, SIGNAL(copyOperationRequested(QString,QString,QString,QCryptographicHash::Algorithm)), business, SLOT(recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(QString,QString,QString,QCryptographicHash::Algorithm)));
    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(business, SIGNAL(copyOperationComplete()), &m_Gui, SLOT(handleCopyOperationComplete()));

    m_Gui.show();
}
void RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay::handleAboutToQuit()
{
    m_BusinessThread.quit(); //TODOreq: utilize "Cancel" functionality once implemented
    m_BusinessThread.wait();
}

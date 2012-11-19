#include "multiclienthelloertest.h"

MultiClientHelloerTest::MultiClientHelloerTest(QObject *parent) :
    QObject(parent), m_CleanedUpBackends(false)
{
    connect(&m_MultiClientBusinessThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(multiClientHelloerReadyForConnections()));
    m_MultiClientBusinessThreadHelper.start();

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(cleanupBackendObjectsIfNeeded()));

    m_Gui.show();
}
MultiClientHelloerTest::~MultiClientHelloerTest()
{
    cleanupBackendObjectsIfNeeded();
}
void MultiClientHelloerTest::multiClientHelloerReadyForConnections()
{
    MultiClientBusiness *backend = m_MultiClientBusinessThreadHelper.getObjectPointerForConnectionsOnly();

    connect(&m_Gui, SIGNAL(addSslClientRequested()), backend, SLOT(addSslClient()));
    connect(backend, SIGNAL(d(const QString &)), &m_Gui, SLOT(handleD(const QString &)));
}
void MultiClientHelloerTest::cleanupBackendObjectsIfNeeded()
{
    if(!m_CleanedUpBackends)
    {
        m_MultiClientBusinessThreadHelper.quit();
        m_MultiClientBusinessThreadHelper.wait();
        m_CleanedUpBackends = true;
    }
}

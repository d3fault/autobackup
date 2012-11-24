#include "multiserverhelloertest.h"

MultiServerHelloerTest::MultiServerHelloerTest(QObject *parent) :
    QObject(parent), m_CleanedUpBackends(false)
{
    connect(&m_MultiServerBusinessThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(multiServerHelloerReadyForConnections()));
    m_MultiServerBusinessThreadHelper.start();

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    m_Gui.show();
}
MultiServerHelloerTest::~MultiServerHelloerTest()
{
    cleanupBackendObjectsIfNeeded();
}
void MultiServerHelloerTest::cleanupBackendObjectsIfNeeded()
{
    if(!m_CleanedUpBackends)
    {
        m_MultiServerBusinessThreadHelper.quit();
        m_MultiServerBusinessThreadHelper.wait();
        m_CleanedUpBackends = true;
    }
}
void MultiServerHelloerTest::multiServerHelloerReadyForConnections()
{
    MultiServerBusiness *backend = m_MultiServerBusinessThreadHelper.getObjectPointerForConnectionsOnly();

    connect(&m_Gui, SIGNAL(startAll3ListeningRequested()), backend, SLOT(startAll3Listening()));
    connect(&m_Gui, SIGNAL(messageSendRequested(QString)), backend, SLOT(DBG_sendMessageToClient(QString)));
    connect(backend, SIGNAL(atLeastOneClientIsConnected(bool)), &m_Gui, SLOT(setReadyForBusinessMessageInteraction(bool)));

    connect(backend, SIGNAL(d(const QString &)), &m_Gui, SLOT(handleD(const QString &)));
}
void MultiServerHelloerTest::handleAboutToQuit()
{
    cleanupBackendObjectsIfNeeded();
}

#include "multiserverhelloertest.h"

MultiServerHelloerTest::MultiServerHelloerTest(QObject *parent) :
    QObject(parent)
{
    connect(&m_MultiServerBusinessThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(multiServerHelloerReadyForConnections()));
}
void MultiServerHelloerTest::multiServerHelloerReadyForConnections()
{
    MultiServerBusiness *backend = m_MultiServerBusinessThreadHelper.getObjectPointerForConnectionsOnly();

    connect(&m_Gui, SIGNAL(startAll3ListeningRequested()), backend, SLOT(startAll3Listening()));
}

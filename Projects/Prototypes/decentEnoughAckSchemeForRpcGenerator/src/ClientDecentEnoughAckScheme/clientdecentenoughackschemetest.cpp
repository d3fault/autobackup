#include "clientdecentenoughackschemetest.h"

ClientDecentEnoughAckSchemeTest::ClientDecentEnoughAckSchemeTest(QObject *parent) :
    QObject(parent)
{
    connect(&m_Backend, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleBackendReadyForConnections()));
}
void ClientDecentEnoughAckSchemeTest::handleBackendReadyForConnections()
{
    ClientDecentEnoughAckSchemeBackend *backend = m_Backend.getObjectPointerForConnectionsOnly();

    connect(&m_Gui, SIGNAL(startClientBackendRequested()), backend, SLOT(startClientBackend()));
    connect(&m_Gui, SIGNAL(stopClientBackendRequested()), backend, SLOT(stopClientBackend()));

    connect(backend, SIGNAL(d(const QString &)), &m_Gui, SLOT(handleD(const QString &)));
}

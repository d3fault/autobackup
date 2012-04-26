#include "ourserverforwtfrontends.h"

OurServerForWtFrontEnds::OurServerForWtFrontEnds(QObject *parent) :
    QObject(parent), m_SslTcpServer(0)
{
}
void OurServerForWtFrontEnds::startListeningForWtFrontEnds()
{
    //TODO: start the SSL server (contained. as in, it's logic should not mix with this class's logic........ which considering i have to implement QSslSocket anyways... shouldn't be TOO hard. but idk if i should have yet another class encapsulating even that). if i were to have it contained, the contained layer would have to know about my protocol. i could maybe send a generic protocol to it in it's constructor.. but that doesn't sound worth the effort [atm!]. this way the container would just have a signal like "protocolMessageDetected(GenericProtocolMessage *message)"... and i'd [again] have to figure out what kind it is using a switch() based on the same shit i used as input for the constructor... AND it would also have 2 slots... sendMessageToSpecificClient and broadcastMessage......... OR SOMETHING. idfk and don't care to figure it out right now. i just know that my handleNewIncomingConnectionNotYetEncrypted() shit could probably be encapsulated/re-used

    if(!m_SslTcpServer)
    {
        m_SslTcpServer = new SslTcpServer(this, ":/serverCa.pem", ":/clientCa.pem", ":/serverPrivateEncryptionKey.pem", ":/serverPublicLocalCertificate.pem", "fuckyou" /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */);
        connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
        connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(uint)), this, SLOT(handleClientConnectedAndEncrypted(uint)));
        m_SslTcpServer->initAndStartListening();
    }
}
void OurServerForWtFrontEnds::handleResponseFromAppLogic(AppLogicRequestResponse *response)
{
}
void OurServerForWtFrontEnds::handleClientConnectedAndEncrypted(uint clientId)
{
    //TODOreq: send them the list of usernames that have a bank account already created
}

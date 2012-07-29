#include "ssltcpserverandbankserverprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

SslTcpServerAndBankServerProtocolKnower::SslTcpServerAndBankServerProtocolKnower(QObject *parent)
    : IBankServerProtocolKnower(parent)
{
    m_SslTcpServer = new SslTcpServer(this, ":/serverCa.pem", ":/clientCa.pem", ":/serverPrivateEncryptionKey.pem", ":/serverPublicLocalCertificate.pem", "fuckyou" /* TODOopt: make it so when starting the server we are prompted for this passphrase. this way it is only ever stored in memory... which is an improvement but still not perfect */);
    connect(m_SslTcpServer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_SslTcpServer, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
}
void SslTcpServerAndBankServerProtocolKnower::init()
{
    if(m_SslTcpServer->init())
    {
        emit initialized();
    }
}
void SslTcpServerAndBankServerProtocolKnower::start()
{
    if(m_SslTcpServer->start())
    {
        emit started();
    }
}
void SslTcpServerAndBankServerProtocolKnower::stop()
{
    m_SslTcpServer->stop();
    emit stopped();
}
void SslTcpServerAndBankServerProtocolKnower::handleClientConnectedAndEncrypted(QSslSocket *newClient)
{
    connect(newClient, SIGNAL(readyRead()), this, SLOT(handleMessageReceivedFromRpcClientOverNetwork()));
}
void SslTcpServerAndBankServerProtocolKnower::handleMessageReceivedFromRpcClientOverNetwork()
{
    //Action Requests
    //in some handleMessageReceivedFromClient method, we read the header and use our message dispensers to get and emit the right message type
    if(QSslSocket *secureSocket = static_cast<QSslSocket*>(sender()))
    {
        QDataStream stream(secureSocket);
        while(!stream.atEnd())
        {
            RpcBankServerHeader header; //stack alloc because there is no dptr. might change this if i want header to hold client response data etc
            stream >> header;
#if 0
            if(header.MessageSize > 0)
            {
#endif
            switch(header.MessageType)
            {
            case RpcBankServerHeader::CreateBankAccountMessageType:
                {
                    CreateBankAccountMessage *createBankAccountMessage = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
                    createBankAccountMessage->Header.MessageType = header.MessageType;
                    createBankAccountMessage->Header.MessageId = header.MessageId;
                    stream >> *createBankAccountMessage;
                    processCreateBankAccountMessage(createBankAccountMessage, SslTcpServer::getClientUniqueId(secureSocket));
                }
            break;
            case RpcBankServerHeader::GetAddFundsKeyMessageType:
                {
                    GetAddFundsKeyMessage *getAddFundsKeyMessage = m_GetAddFundsKeyMessageDispenser->getNewOrRecycled();
                    getAddFundsKeyMessage->Header.MessageType = header.MessageType;
                    getAddFundsKeyMessage->Header.MessageId = header.MessageId;
                    stream >> *getAddFundsKeyMessage;
                    processGetAddFundsKeyMessage(getAddFundsKeyMessage, SslTcpServer::getClientUniqueId(secureSocket));
                }
            break;
            case RpcBankServerHeader::InvalidMessageType:
            default:
                emit d("error: got invalid message type from client");
            break;
            }
            //} if(header.MessageSize > 0)
        }
    }
}
void SslTcpServerAndBankServerProtocolKnower::myTransmit(IMessage *message, uint uniqueRpcClientId)
{
    //Action Responses + Broadcasts

    //TODOreq -- uniqueRpcClientId is 0 for broadcasts... and perhaps if the action broadcasts it's response?
    //TODOreq: this isn't directly related to this method, but i need to make note of it somewhere. only one of the broadcast receivers should write the broadcast value to the db (assuming a shared-among-broadcast-receivers couchbase cluster)... so maybe that implies i should just broadcast to one client and then let him notify his siblings? or something? idfk

    //TODOreq: handle the case where none of broadcast's servers (clients) are online, nobody to send to wtf

    QSslSocket *socket = m_SslTcpServer->getSocketByUniqueId(uniqueRpcClientId);
    QDataStream stream(socket);
    stream << message->Header;
    stream << message;

    //TODOreq: stream the header first, most importantly, the size. actually size isn't that important i can probably disregard it. qt puts it in for me whenever needed lol
    //to get that enum, we could keep the header around
    //the ENUM is key. the enum is the only useful part of the header
    //the enum IS the header..
    //i could store it in IMessage and just stream it in/out right before my pure virtual streamIn/streamOut inherited class calls
    //same with the uniqueClientId... saves me from having to look it up later (i look up the client id so i can look up the socket, 2 lookups. there would only be one if i do what this comment says. and there would be equal data storage. it's just faster/better to put uniqueClient in IMessage. and might as well do the header enum too... ??? may need to sleep on this...)
}

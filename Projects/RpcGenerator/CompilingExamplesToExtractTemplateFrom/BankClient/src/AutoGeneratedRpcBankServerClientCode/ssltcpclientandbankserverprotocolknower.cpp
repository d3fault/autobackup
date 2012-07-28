#include "ssltcpclientandbankserverprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

SslTcpClientAndBankServerProtocolKnower::SslTcpClientAndBankServerProtocolKnower(QObject *parent)
    : IBankServerClientProtocolKnower(parent)
{
    m_SslTcpClient = new SslTcpClient(this, ":/clientCa.pem", ":/serverCa.pem", ":/clientPrivateEncryptionKey.pem", ":/clientPublicLocalCertificate.pem", "fuckyou");
    connect(m_SslTcpClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_SslTcpClient, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleConnectedAndEncrypted(QSslSocket*)));
}
void SslTcpClientAndBankServerProtocolKnower::init()
{
    if(m_SslTcpClient->init())
    {
        emit initialized();
    }
}
void SslTcpClientAndBankServerProtocolKnower::start()
{
    if(m_SslTcpClient->start())
    {
        emit started();
    }
}
void SslTcpClientAndBankServerProtocolKnower::stop()
{
    m_SslTcpClient->stop();
    emit stopped();
}
void SslTcpClientAndBankServerProtocolKnower::handleConnectedAndEncrypted(QSslSocket *socket)
{
    connect(m_Server, SIGNAL(readyRead()), this, SLOT(handleMessageReceivedFromRpcServerOverNetwork()));
}
void SslTcpClientAndBankServerProtocolKnower::handleMessageReceivedFromRpcServerOverNetwork()
{
    //Action Responses and Broadcasts
    QDataStream stream(m_SslTcpClient);
    while(!stream.atEnd())
    {
        RpcBankServerHeader header;
        stream >> header;
        if(header.MessageSize > 0) //could compare m_SslTcpClient->bytesAvailable() and return if not enough, but if there aren't enough left then we need to eh put the header BACK so we re-read it next time the slot gets called or something??? maybe i should just scrap headers altogether... but i like how i can delegate the stream operator to the message type itself :(. perhaps i can have a 2-stage header?? doesn't help with solving this particular problem, but does help with the message ID thing a bit (maybe? unsure)
        {
            switch(header.MessageType)
            {
            //Action Responses
            case RpcBankServerHeader::CreateBankAccountMessageType:
                {
                    processCreateBankAccountResponseReceived();
                    //TODOreq: i think i need to put a unique message ID in the header so i can match it up with the pending message?? idfk but something definitely needs to be done here
                    //the main problem with this that i can think of is that we do NOT recycle the headers
                }
            break;
            case RpcBankServerHeader::GetAddFundsKeyMessageType:
                {
                    processGetAddFundsKeyResponseReceived();
                }
            break;
            //Broadcasts
            case RpcBankServerHeader::PendingBalanceDetectedMessageType:
                {
                    PendingBalanceDetectedMessage *pendingBalanceDetectedMessage = m_PendingBalanceDetectedMessageDispenser->getNewOrRecycled();
                    emit pendingBalanceDetected(pendingBalanceDetectedMessage);
                }
            break;
            case RpcBankServerHeader::ConfirmedBalanceDetectedMessageType:
                {
                    ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = m_ConfirmedBalanceDetectedMessageDispenser->getNewOrRecycled();
                    emit confirmedBalanceDetected(confirmedBalanceDetectedMessage);
                }
            break;
            case RpcBankServerHeader::InvalidMessageType:
            default:
                emit d("error: got invalid message type from server");
            break;
            }
        }
    }
}
void SslTcpClientAndBankServerProtocolKnower::myTransmit(IMessage *messaage)
{
    //TODOreq: stream header
    QDataStream stream(m_SslTcpClient);
    stream << messaage;
}

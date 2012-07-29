#include "ssltcpclientandbankserverprotocolknower.h"

#include "../../RpcBankServerAndClientShared/rpcbankserverheader.h"

SslTcpClientAndBankServerProtocolKnower::SslTcpClientAndBankServerProtocolKnower(QObject *parent)
    : IBankServerClientProtocolKnower(parent)
{
    m_SslTcpClient = new SslTcpClient(this, ":/RpcBankServerClientCA.pem", ":/RpcBankServerCA.pem", ":/RpcBankServerClientPrivateKey.pem", ":/RpcBankServerClientPublicCert.pem", "fuckyou");
    connect(m_SslTcpClient, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(m_SslTcpClient, SIGNAL(connectedAndEncrypted(QSslSocket*)), this, SLOT(handleConnectedAndEncrypted(QSslSocket*)));
}
void SslTcpClientAndBankServerProtocolKnower::init()
{
    emit d("SslTcpClientAndBankServerProtocolKnower received init message");
    if(m_SslTcpClient->init())
    {
        emit d("SslTcpClient successfully initialized");
        emit initialized();
    }
}
void SslTcpClientAndBankServerProtocolKnower::start()
{
    emit d("SslTcpClientAndBankServerProtocolKnower received start message");
    if(m_SslTcpClient->start())
    {
        emit d("SslTcpClient successfully started");
        emit started();
    }
}
void SslTcpClientAndBankServerProtocolKnower::stop()
{
    emit d("SslTcpClientAndBankServerProtocolKnower received stopped message");
    m_SslTcpClient->stop();
    emit stopped();
}
void SslTcpClientAndBankServerProtocolKnower::handleConnectedAndEncrypted(QSslSocket *socket)
{
    //TODOoptimization: perhaps with connection management stuff, i can check to see if m_SslTcpClient == socket... and if not, then socket is the new connection so we should drop/forget the old one or something idfk
    connect(socket, SIGNAL(readyRead()), this, SLOT(handleMessageReceivedFromRpcServerOverNetwork()));
}
void SslTcpClientAndBankServerProtocolKnower::handleMessageReceivedFromRpcServerOverNetwork()
{
    //Action Responses and Broadcasts
    QDataStream stream(static_cast<QSslSocket*>(sender()));
    while(!stream.atEnd())
    {
        RpcBankServerHeader header;
        stream >> header;
#if 0
        if(header.MessageSize > 0) //could compare m_SslTcpClient->bytesAvailable() and return if not enough, but if there aren't enough left then we need to eh put the header BACK so we re-read it next time the slot gets called or something??? maybe i should just scrap headers altogether... but i like how i can delegate the stream operator to the message type itself :(. perhaps i can have a 2-stage header?? doesn't help with solving this particular problem, but does help with the message ID thing a bit (maybe? unsure)
        {
#endif
        switch(header.MessageType)
        {
        //Action Responses
        case RpcBankServerHeader::CreateBankAccountMessageType:
            {
                CreateBankAccountMessage *createBankAccountMessage = getPendingCreateBankAccountMessageById(header.MessageId);
                if(createBankAccountMessage)
                {
                    stream >> *createBankAccountMessage;
                    emit createBankAccountCompleted(createBankAccountMessage);
                    //processCreateBankAccountResponseReceived();
                }
                else
                {
                    //TODOreq: deal with response that was never requested
                }
            }
        break;
        case RpcBankServerHeader::GetAddFundsKeyMessageType:
            {
                GetAddFundsKeyMessage *getAddFundsKeyMessage = getPendingGetAddFundsKeyMessageById(header.MessageId);
                if(getAddFundsKeyMessage)
                {
                    stream >> *getAddFundsKeyMessage;
                    emit getAddFundsKeyCompleted(getAddFundsKeyMessage);
                    //processGetAddFundsKeyResponseReceived();
                }
                else
                {
                    //TODOreq: deal with response that was never requested
                }
            }
        break;
        //Broadcasts
        case RpcBankServerHeader::PendingBalanceDetectedMessageType:
            {
                PendingBalanceDetectedMessage *pendingBalanceDetectedMessage = m_PendingBalanceDetectedMessageDispenser->getNewOrRecycled();
                stream >> *pendingBalanceDetectedMessage;
                emit pendingBalanceDetected(pendingBalanceDetectedMessage);
            }
        break;
        case RpcBankServerHeader::ConfirmedBalanceDetectedMessageType:
            {
                ConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = m_ConfirmedBalanceDetectedMessageDispenser->getNewOrRecycled();
                stream >> *confirmedBalanceDetectedMessage;
                emit confirmedBalanceDetected(confirmedBalanceDetectedMessage);
            }
        break;
        case RpcBankServerHeader::InvalidMessageType:
        default:
            emit d("error: got invalid message type from server");
        break;
        }
        //} if(header.MessageSize > 0)
    }
}
void SslTcpClientAndBankServerProtocolKnower::myTransmit(IMessage *message)
{
    QDataStream stream(m_SslTcpClient);
    stream << message->Header;
    stream << message;
}

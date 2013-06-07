#include "ssltcpclientandbankserverprotocolknower.h"

#if 0
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
    if(m_SslTcpClient->initialize())
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
        quint32 failedReasonEnum; //TODO: sure aren't streaming it every time, but do we really have to alloc it every time? just stack alloc so w/e... but still. Fuck you, I've since learned stack allocs are practically free lmfao. especially a measily quint32 (no heap d ptr)
        if(!header.Success)
        {
            stream >> failedReasonEnum;
        }
#if 0
        if(header.MessageSize > 0) //could compare m_SslTcpClient->bytesAvailable() and return if not enough, but if there aren't enough left then we need to eh put the header BACK so we re-read it next time the slot gets called or something??? maybe i should just scrap headers altogether... but i like how i can delegate the stream operator to the message type itself :(. perhaps i can have a 2-stage header?? doesn't help with solving this particular problem, but does help with the message ID thing a bit (maybe? unsure)
        {
#endif
        switch(header.MessageType)
        {
        //Action Responses
        case RpcBankServerHeader::CreateBankAccountMessageType:
            {
                //TODOreq: get our new timestamp, find out the difference between the pre-transmit one, and record that in some list thingy lol

                ClientCreateBankAccountMessage *createBankAccountMessage = getPendingCreateBankAccountMessageById(header.MessageId);
                if(createBankAccountMessage)
                {
                    stream >> *createBankAccountMessage;
                    if(header.Success)
                    {
                        emit createBankAccountCompleted(createBankAccountMessage);
                    }
                    else
                    {
                        switch(failedReasonEnum)
                        {
                        case ClientCreateBankAccountMessage::FailedUsernameAlreadyExists:
                            {
                                emit createBankAccountFailedUsernameAlreadyExists(createBankAccountMessage);
                            }
                        break;
                        case ClientCreateBankAccountMessage::FailedPersistError:
                            {
                                emit createBankAccountFailedPersistError(createBankAccountMessage);
                            }
                        break;
                        default:
                            //TODOreq: unknown error? maybe a generic one that they all have?
                        break;
                        }
                    }
                }
                else
                {
                    //TODOreq: deal with response that was never requested
                }
            }
        break;
        case RpcBankServerHeader::GetAddFundsKeyMessageType:
            {
                //TODOreq: get our new timestamp, find out the difference between the pre-transmit one, and record that in some list thingy lol

                ClientGetAddFundsKeyMessage *getAddFundsKeyMessage = getPendingGetAddFundsKeyMessageById(header.MessageId);
                if(getAddFundsKeyMessage)
                {
                    stream >> *getAddFundsKeyMessage;
                    if(header.Success)
                    {
                        emit getAddFundsKeyCompleted(getAddFundsKeyMessage);
                    }
                    else
                    {
                        switch(failedReasonEnum)
                        {
                        case ClientGetAddFundsKeyMessage::FailedUsernameDoesntExist:
                            {
                                emit getAddFundsKeyFailedUsernameDoesntExist(getAddFundsKeyMessage);
                            }
                        break;
                        case ClientGetAddFundsKeyMessage::FailedUseExistingKeyFirst:
                            {
                                emit getAddFundsKeyFailedUseExistingKeyFirst(getAddFundsKeyMessage);
                            }
                        break;
                        case ClientGetAddFundsKeyMessage::FailedWaitForPendingToBeConfirmed:
                            {
                                emit getAddFundsKeyFailedWaitForPendingToBeConfirmed(getAddFundsKeyMessage);
                            }
                        break;
                        default:
                            //unknown/generic error?
                        break;
                        }
                    }
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
                ClientPendingBalanceDetectedMessage *pendingBalanceDetectedMessage = m_PendingBalanceDetectedMessageDispenser->getNewOrRecycled();
                stream >> *pendingBalanceDetectedMessage;
                emit pendingBalanceDetected(pendingBalanceDetectedMessage);
            }
        break;
        case RpcBankServerHeader::ConfirmedBalanceDetectedMessageType:
            {
                ClientConfirmedBalanceDetectedMessage *confirmedBalanceDetectedMessage = m_ConfirmedBalanceDetectedMessageDispenser->getNewOrRecycled();
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
    stream << *message;
}
#endif

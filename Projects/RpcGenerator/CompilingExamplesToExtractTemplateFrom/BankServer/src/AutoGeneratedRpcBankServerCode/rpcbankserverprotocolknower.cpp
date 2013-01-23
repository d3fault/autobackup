#include "rpcbankserverprotocolknower.h"

RpcBankServerProtocolKnower::RpcBankServerProtocolKnower(QObject *parent) :
    IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries(parent)
{
    m_CreateBankAccountMessageDispenser = new ServerCreateBankAccountMessageDispenser(this, this);
}

//TODOoptimization: the datastream never changes so we don't need to pass it in as an arg. we could set it in the constructor etc
void RpcBankServerProtocolKnower::messageReceived(QDataStream *messageDataStream)
{
    RpcBankServerMessageHeader header;
    *messageDataStream >> header;
    switch(header.MessageType)
    {
        case RpcBankServerMessageHeader::CreateBankAccountMessageType:
        {
            CreateBankAccountMessage *createBankAccountMessage = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
            *messageDataStream >> *createBankAccountMessage; //this double deref seems sexy for some reason
            processCreateBankAccountMessage(createBankAccountMessage);
        }
        break;

        case RpcBankServerMessageHeader::InvalidMessageType:
        default:
        {
            //TODOreq
        }
        break;
    }
}

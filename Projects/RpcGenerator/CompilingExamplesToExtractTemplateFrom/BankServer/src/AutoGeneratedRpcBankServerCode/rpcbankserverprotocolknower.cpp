#include "rpcbankserverprotocolknower.h"

RpcBankServerProtocolKnower::RpcBankServerProtocolKnower(IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack *signalRelayHackEmitter, QObject *parent) :
    m_SignalRelayHackEmitter(signalRelayHackEmitter), IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries(parent)
{
    m_CreateBankAccountMessageDispenser = new ServerCreateBankAccountMessageDispenser(this, this);
    m_GetAddFundsKeyMessageDispenser = new ServerGetAddFundsKeyMessageDispenser(this, this);
    //etc for each Action
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
            if(processNewRpcBankServerActionMessage_AND_RecordMessageInBusinessAndReturnTrueIfNeedEmitToBusiness(createBankAccountMessage, &m_CreateBankAccountMessagesAwaitingLazyResponseAck, &m_CreateBankAccountMessagesPendingInBusiness))
            {
                m_SignalRelayHackEmitter->emitCreateBankAccountRequested(createBankAccountMessage);
            }
        }
        break;
        case RpcBankServerMessageHeader::GetAddFundsKeyMessageType:
        {
            GetAddFundsKeyMessage *getAddFundsKeyMessage = m_GetAddFundsKeyMessageDispenser->getNewOrRecycled();
            *messageDataStream >> *getAddFundsKeyMessage;
            if(processNewRpcBankServerActionMessage_AND_RecordMessageInBusinessAndReturnTrueIfNeedEmitToBusiness(getAddFundsKeyMessage, &m_GetAddFundsKeyMessagesAwaitingLazyResponseAck, &m_GetAddFundsKeyMessagesPendingInBusiness))
            {
                m_SignalRelayHackEmitter->emitGetAddFundsKeyRequested(getAddFundsKeyMessage);
            }
        }
        break;
        //etc for each Action

        case RpcBankServerMessageHeader::InvalidMessageType:
        default:
        {
            //TODOreq
        }
        break;
    }
}
void RpcBankServerProtocolKnower::createBankAccountDelivery()
{
    removeFromPendingInBusiness_AND_addToAwaitingAck_And_StreamToByteArray_And_Transmit(static_cast<IActionMessage*>(sender()), &m_CreateBankAccountMessagesAwaitingLazyResponseAck, &m_CreateBankAccountMessagesPendingInBusiness);
}
void RpcBankServerProtocolKnower::getAddFundsKeyDelivery()
{
    removeFromPendingInBusiness_AND_addToAwaitingAck_And_StreamToByteArray_And_Transmit(static_cast<IActionMessage*>(sender()), &m_GetAddFundsKeyMessagesAwaitingLazyResponseAck, &m_GetAddFundsKeyMessagesPendingInBusiness);
}

#include "rpcbankserverprotocolknower.h"

IAcceptRpcBankServerBroadcastDeliveries_AND_IEmitActionsForSignalRelayHack * RpcBankServerProtocolKnower::m_SignalRelayHackEmitter = 0;

RpcBankServerProtocolKnower::RpcBankServerProtocolKnower(QObject *parent) :
    IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptActionDeliveries(parent)
{
    //TODOoptional: now might be a good place to runtime check that the signal relay hack emitter static is not zero, but i trust my segfaults (though I don't trust the segfaults themselves GAH i need a dedicated test box)

    m_CreateBankAccountMessageDispenser = new ServerCreateBankAccountMessageDispenser(this, this);
    m_GetAddFundsKeyMessageDispenser = new ServerGetAddFundsKeyMessageDispenser(this, this);
    //etc for each Action
}
//TO DOneoptimization: the datastream never changes so we don't need to pass it in as an arg. we could set it in the constructor etc
void RpcBankServerProtocolKnower::messageReceived() //QDataStream *messageDataStream)
{
    RpcBankServerMessageHeader header;
    *m_MessageReceivedDataStream >> header;
    switch(header.GenericRpcMessageType)
    {
        case RpcBankServerMessageHeader::ActionRequestGenericClient2ServerMessageType:
        {
            switch(header.RpcServiceSpecificMessageType)
            {
                case RpcBankServerMessageHeader::CreateBankAccountMessageType:
                {
                    CreateBankAccountMessage *createBankAccountMessage = m_CreateBankAccountMessageDispenser->getNewOrRecycled();
                    *m_MessageReceivedDataStream >> *createBankAccountMessage; //this double deref seems sexy for some reason
                    if(processNewRpcBankServerActionMessage_AND_RecordMessageInBusinessAndReturnTrueIfNeedEmitToBusiness(createBankAccountMessage, &m_CreateBankAccountMessagesAwaitingLazyResponseAck, &m_CreateBankAccountMessagesPendingInBusiness))
                    {
                        m_SignalRelayHackEmitter->emitCreateBankAccountRequested(createBankAccountMessage);
                    }
                }
                break;
                case RpcBankServerMessageHeader::GetAddFundsKeyMessageType:
                {
                    GetAddFundsKeyMessage *getAddFundsKeyMessage = m_GetAddFundsKeyMessageDispenser->getNewOrRecycled();
                    *m_MessageReceivedDataStream >> *getAddFundsKeyMessage;
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
        break; //case RpcBankServerMessageHeader::ActionRequestGenericClient2ServerMessageType:
        case SHIT DEFINITELY BROKE MY DESIGN TODAY because we already have/had retry code in the above case and yet this is where it would go with the refactor TODO LEFT OFF or just kill yourself
    }
}
void RpcBankServerProtocolKnower::notifyThatQueueActionResponsesHasBeenEnabled()
{
    //we should compile and maintain a list (for each message action type) for all the ones that are in business pending at this very moment
    m_CreateBankAccountMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode.clear();
    m_GetAddFundsKeyMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode.clear();
    //etc for each Action

    m_CreateBankAccountMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode.append(m_CreateBankAccountMessagesPendingInBusiness.keys());
    m_GetAddFundsKeyMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode.append(m_GetAddFundsKeyMessagesPendingInBusiness.keys());
    //etc for each Action
    //mfw when i almost coded an inline function to do the above operation generically for all actions then lol'd when i decided i'd call it "append" and the solution already existed!

    //then, in their respective delivery slots, we utilize the lists we created here --- TODOreq: how? did i forget to code something? fml. they aren't referenced/"used" in any way except clear/append above :-/ so i think so. nvm i'm just blind.
}
void RpcBankServerProtocolKnower::createBankAccountDelivery()
{
    removeFromPendingInBusiness_AND_addToAwaitingAck_And_StreamToByteArray_And_Transmit(static_cast<IActionMessage*>(sender()), &m_CreateBankAccountMessagesAwaitingLazyResponseAck, &m_CreateBankAccountMessagesPendingInBusiness, &m_CreateBankAccountMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode);
}
void RpcBankServerProtocolKnower::getAddFundsKeyDelivery()
{
    removeFromPendingInBusiness_AND_addToAwaitingAck_And_StreamToByteArray_And_Transmit(static_cast<IActionMessage*>(sender()), &m_GetAddFundsKeyMessagesAwaitingLazyResponseAck, &m_GetAddFundsKeyMessagesPendingInBusiness, &m_GetAddFundsKeyMessagesPendingInBusinessWhenEnablingQueueActionResponsesMode);
}

#include "rpcbankserverprotocolknower.h"

#define A_SINGLE_SWITCH_CASE_FOR_A_SINGLE_ACTION(actionWithLeadingUppercase, actionWithLeadingLowercase, methodToCall) \
{ \
    case RpcBankServerMessageHeader::actionWithLeadingUppercase##RpcBankServerSpecificMessageType: \
    { \
        actionWithLeadingUppercase##Message *actionWithLeadingLowercase##Message = m_##actionWithLeadingUppercase##MessageDispenser->getNewOrRecycled(); \
        *m_MessageReceivedDataStream >> *actionWithLeadingLowercase##Message; /*this double deref seems sexy for some reason*/ \
        if(methodToCall(actionWithLeadingLowercase##Message, &m_##actionWithLeadingUppercase##MessagesAwaitingLazyResponseAck, &m_##actionWithLeadingUppercase##MessagesPendingInBusiness)) \
        { \
            m_SignalRelayHackEmitter->emit##actionWithLeadingUppercase##Requested(actionWithLeadingLowercase##Message); \
        } \
    } \
    break; \
}

#define SWITCH_SPECIFIC_MESSAGE_TYPE_AND_THEN_CALL_SPECIFIED_METHOD_THEN_INVOKE_BUSINESS_IF_NECESSARY(methodToCall) \
{ \
    switch(header.RpcServiceSpecificMessageType) \
    { \
        A_SINGLE_SWITCH_CASE_FOR_A_SINGLE_ACTION(CreateBankAccount, createBankAccount, methodToCall) \
        A_SINGLE_SWITCH_CASE_FOR_A_SINGLE_ACTION(GetAddFundsKey, getAddFundsKey, methodToCall) \
        /*etc for each Action */ \
        case RpcBankServerMessageHeader::InvalidRpcBankServerSpecificMessageType: \
        default: \
        { \
            /*TODOreq*/ \
        } \
        break; \
    } \
}

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
        case RpcBankServerMessageHeader::ActionRequestGenericClient2ServerMessageType: //can invoke business
        {
            SWITCH_SPECIFIC_MESSAGE_TYPE_AND_THEN_CALL_SPECIFIED_METHOD_THEN_INVOKE_BUSINESS_IF_NECESSARY(processNewRpcBankServerActionMessage_AND_RecordMessageInBusinessAndReturnTrueIfNeedEmitToBusiness)

#if 0 //leaving the below here so I can see easily what the expanded macro should look like
            //I'm thinking the below switch statement can/should/will be re-used in a macro that allows me to specify the method it calls ('processNew...' atm), so I can call methods specifically if it's a retry1, retry2, etc... for each of the Client2ServerMessageTypes. Each type will invoke that macro (unless it's unrelated to Actions, such as a Broadcast Ack). So I guess the method signatures should match in all those cases so that they can decide whether or not to invoke business as well. It might even be possible to call a macro in my macro so I only have to write the first 'case' statement and then swap out the wording specific to that Action Request ('createBankAccount' && 'CreateBankAccount' (only difference is casing lol))
            //^^duplicate 'code' (in binary from the macro) isn't such a big deal (i care more about duplicate code that has been copy/pasted, and this macro solution solves that <3), but we mainly should take care to not needlessly evaluate the same switch/case statements over and over. each should only ever be utilized ONCE for a given code path

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
#endif
        }
        break;
        case RpcBankServerMessageHeader::ActionRequestRetry1Status1GenericClient2ServerMessageType: //can invoke business, but only does that in certain cases such as when the original request got lost on the network (etc). We'd most likely find this request in business pending
        {
            SWITCH_SPECIFIC_MESSAGE_TYPE_AND_THEN_CALL_SPECIFIED_METHOD_THEN_INVOKE_BUSINESS_IF_NECESSARY(processRetryRpcBankServerActionMessage_AND_ReturnTrueIfNeedEmitToBusiness)
        }
        break;
        case RpcBankServerMessageHeader::ActionRequestRetry2Status2GenericClient2ServerMessageType: //ditto as above, and perhaps I can/should combine the two pieces of code (make both cases in a row before a single break)
        {
            //TODOreq
        }
        break;
        case RpcBankServerMessageHeader::BroadcastManualAckGenericClient2ServerMessageType: //never invokes business, so does not use the switch macro used with actions here
        {
            //TODOreq
        }
        break;
        case RpcBankServerMessageHeader::DisconnectRequestedGenericClient2ServerMessageType:
        {
            //TODOreq: don't accept any more action requests (and set up the "readyToDisconnect" response to be sent after all Action Responses are sent + ack'd). also don't broadcast to this client anymore (TODOreq: don't some broadcasts come as a delayed response to an action request (GetAddFundsKey, for example)? I guess in that case I'd have to tell another server or client or something to basically re-route those broadcasts... OR SOMETHING IDFK)
        }
        break;
        case RpcBankServerMessageHeader::InvalidGenericClient2ServerMessageType:
        default:
        {
            //TODOreq: errors etc
        }
        break;
        //case SHIT DEFINITELY BROKE MY DESIGN TODAY because we already have/had retry code in the above case and yet this is where it would go with the refactor TO/* DO LEFT */ OFF or just kill yourself (opting towards not killing myself (mfw i have to not kill myself in order to opt))
    }
}
void RpcBankServerProtocolKnower::queueActionResponsesHasBeenEnabledSoBuildLists()
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

#undef SWITCH_SPECIFIC_MESSAGE_TYPE_AND_THEN_CALL_SPECIFIED_METHOD_THEN_INVOKE_BUSINESS_IF_NECESSARY
#undef A_SINGLE_SWITCH_CASE_FOR_A_SINGLE_ACTION

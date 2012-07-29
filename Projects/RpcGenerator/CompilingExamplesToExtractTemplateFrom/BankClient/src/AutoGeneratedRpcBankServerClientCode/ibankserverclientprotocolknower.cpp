#include "ibankserverclientprotocolknower.h"

IBankServerClientProtocolKnower::IBankServerClientProtocolKnower(QObject *parent) :
    IEmitRpcBankServerBroadcastAndActionResponseSignalsWithMessageAsParam(parent), m_MessageIdCounter(0)
{ }
void IBankServerClientProtocolKnower::setBroadcastDispensers(RpcBankServerBroadcastDispensers *rpcBankServerBroadcastDispensers)
{
    m_RpcBankServerBroadcastDispensers = rpcBankServerBroadcastDispensers;
}
void IBankServerClientProtocolKnower::takeOwnershipOfBroadcastsAndSetupDelivery()
{
    m_PendingBalanceDetectedMessageDispenser = m_RpcBankServerBroadcastDispensers->takeOwnershipOfPendingBalanceDetectedMessageDispenserRiggedForDelivery(this);
    m_ConfirmedBalanceDetectedMessageDispenser = m_RpcBankServerBroadcastDispensers->takeOwnershipOfConfirmedBalanceDetectedMessageDispenserRiggedForDelivery(this);
}
#if 0
void IBankServerClientProtocolKnower::processCreateBankAccountResponseReceived(CreateBankAccountMessage *createBankAccountMessage)
{
    int index = m_PendingCreateBankAccountMessagePointers.indexOf(createBankAccountMessage);
    if(index > -1)
    {
        m_PendingCreateBankAccountMessagePointers.removeAt(index);
        //TODOreq: errors etc?
        emit createBankAccountCompleted(createBankAccountMessage);
    }
    else
    {
        //TODOreq: deal with error where for some reason the response was never requested???
    }
}
void IBankServerClientProtocolKnower::processGetAddFundsKeyResponseReceived(GetAddFundsKeyMessage *getAddFundsKeyMessage)
{
    int index = m_PendingGetAddFundsKeyMessagePointers.indexOf(getAddFundsKeyMessage);
    if(index > -1)
    {
        m_PendingGetAddFundsKeyMessagePointers.removeAt(index);
        emit getAddFundsKeyCompleted(getAddFundsKeyMessage);
    }
    else
    {

    }
}
#endif
void IBankServerClientProtocolKnower::createBankAccountDelivery()
{
    CreateBankAccountMessage *createBankAccountMessage = static_cast<CreateBankAccountMessage*>(sender());
    //TODOoptimization: perhaps use friend class shit to hide the Header from the user? right now it's just public. ESPECIALLY since they can access the header (right after .getNewOrRecycled()) and before it is even set (right here)
    createBankAccountMessage->Header.MessageId = getUniqueMessageId();
    createBankAccountMessage->Header.MessageType = RpcBankServerHeader::CreateBankAccountMessageType;
    //TODOreq: checking conflicting pending
    m_PendingCreateBankAccountMessagesById.insert(createBankAccountMessage->Header.MessageId, createBankAccountMessage);
    myTransmit(createBankAccountMessage);
    //createBankAccountMessage->doneWithMessage();
    //^^we aren't done with it because we use the same message after getting it out of pending... noob!
    //after we process the response, THEN we are done with it
}
void IBankServerClientProtocolKnower::getAddFundsKeyDelivery()
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = static_cast<GetAddFundsKeyMessage*>(sender());
    getAddFundsKeyMessage->Header.MessageId = getUniqueMessageId();
    getAddFundsKeyMessage->Header.MessageType = RpcBankServerHeader::GetAddFundsKeyMessageType;
    //conflict resolution
    m_PendingGetAddFundsKeyMessagesById.insert(getAddFundsKeyMessage->Header.MessageId, getAddFundsKeyMessage);
    myTransmit(getAddFundsKeyMessage);
}
void IBankServerClientProtocolKnower::pendingBalanceDetectedDelivery()
{
    //no need for this on client, just recycle it if user calls it accidentally
    IMessage *message = static_cast<IMessage*>(sender());
    message->doneWithMessage();
}
void IBankServerClientProtocolKnower::confirmedBalanceDetectedDelivery()
{
    //no need for this on client, just recycle it if user calls it accidentally
    IMessage *message = static_cast<IMessage*>(sender());
    message->doneWithMessage();
}
CreateBankAccountMessage *IBankServerClientProtocolKnower::getPendingCreateBankAccountMessageById(uint messageId)
{
    CreateBankAccountMessage *createBankAccountMessage = m_PendingCreateBankAccountMessagesById.value(messageId, 0);
    if(createBankAccountMessage)
        m_PendingCreateBankAccountMessagesById.remove(messageId);
    return createBankAccountMessage;
    //so a goal of mine was to keep the message matching OUT of the Transporter impl... but seeing as i'm trying to stream onto a cached message... how the fuck am i supposed to do that?? this line should somehow be in IBankServerClientProtocolKnower. maybe a regular protected method call actually? want to get it working first fuck it
    //^^decided it's easier to make the methods than to change the hashes from private to protected (it isn't really, but this is better design anyways)
}
GetAddFundsKeyMessage *IBankServerClientProtocolKnower::getPendingGetAddFundsKeyMessageById(uint messageId)
{
    GetAddFundsKeyMessage *getAddFundsKeyMessage = m_PendingGetAddFundsKeyMessagesById.value(messageId, 0);
    if(getAddFundsKeyMessage)
        m_PendingGetAddFundsKeyMessagesById.remove(messageId);
    return getAddFundsKeyMessage;
}
uint IBankServerClientProtocolKnower::getUniqueMessageId()
{
    return ++m_MessageIdCounter; //TODOreq: use better message id mechanism... like a hash or something. for now/testing, simple increment shit is good enough :-P. and also, right now we DON'T re-use a message id... but actually we probably can (so long as the message id has already been returned from pending, ya know?). doesn't mean we should (but that doesn't mean we shouldn't. i simply do not know)
    //our current method will overflow :-/ -- though i doubt it will happen while testing (in production it surely would)
    //maybe a hash (md5/sha1/crc32) of the current timestamp + the message pointer as an int concatenated? that would solve dupes problem fo sho
}

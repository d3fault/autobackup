#include "rpcbankserverclientprotocolknower.h"

IAcceptRpcBankServerClientActionDeliveries_AND_IEmitBroadcastsForSignalRelayHack *RpcBankServerClientProtocolKnower::m_SignalRelayHackEmitter = 0;

RpcBankServerClientProtocolKnower::RpcBankServerClientProtocolKnower(QObject *parent) :
    IAcceptRpcBankServerActionDeliveries(parent)
{
    m_PendingBalanceDetectedMessageDispenser = new ClientPendingBalanceDetectedMessageDispenser(this, this); //TODOreq: not 100% sure that the destination is correct here... but I also recall that it might just be ignored/irrelevant in this case so maybe it's fine being inaccurate!?
    m_ConfirmedBalanceDetectedMessageDispenser = new ClientConfirmedBalanceDetectedMessageDispenser(this, this);
}
void RpcBankServerClientProtocolKnower::messageReceived()
{
    RpcBankServerMessageHeader header;
    *m_MessageReceivedDataStream >> header;
    switch(header.GenericRpcMessageType)
    {
        case RpcBankServerMessageHeader::ActionResponseGenericServer2ClientMessageType:
        {
            //TODOreq
        }
        break;

        //etc TODOreq

    case RpcBankServerMessageHeader::InvalidGenericServer2ClientMessageType:
    default:
        //TODOreq
        break;
    }
}

#include "clientpendingbalancedetectedmessage.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/pendingbalancedetectedmessagebase.h"

ClientPendingBalanceDetectedMessage::ClientPendingBalanceDetectedMessage(QObject *parent) :
    IRecycleableAndStreamable(parent)
{ }
void ClientPendingBalanceDetectedMessage::streamIn(QDataStream &in)
{
    PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(in,>>);
}
void ClientPendingBalanceDetectedMessage::streamOut(QDataStream &out)
{
    PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(out,<<);
}

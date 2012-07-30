#include "clientconfirmedbalancedetectedmessage.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Broadcasts/confirmedbalancedetectedmessagebase.h"

ClientConfirmedBalanceDetectedMessage::ClientConfirmedBalanceDetectedMessage(QObject *parent) :
    IRecycleableAndStreamable(parent)
{ }
void ClientConfirmedBalanceDetectedMessage::streamIn(QDataStream &in)
{
    CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(in,>>);
}
void ClientConfirmedBalanceDetectedMessage::streamOut(QDataStream &out)
{
    CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(out,<<);
}

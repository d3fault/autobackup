#include "serverpendingbalancedetectedmessage.h"

ServerPendingBalanceDetectedMessage::ServerPendingBalanceDetectedMessage(QObject *owner)
    : IMessage(owner)
{ }
void ServerPendingBalanceDetectedMessage::streamIn(QDataStream &in)
{
    PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(in,>>);
}
void ServerPendingBalanceDetectedMessage::streamOut(QDataStream &out)
{
    PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(out,<<);
}

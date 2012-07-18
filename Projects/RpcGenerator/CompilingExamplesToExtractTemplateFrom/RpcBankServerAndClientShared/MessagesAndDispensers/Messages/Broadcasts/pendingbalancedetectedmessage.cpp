#include "pendingbalancedetectedmessage.h"

PendingBalanceDetectedMessage::PendingBalanceDetectedMessage(QObject *owner)
    : IMessage(owner)
{ }
void PendingBalanceDetectedMessage::streamIn(QDataStream &in)
{
    PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(in,>>);
}
void PendingBalanceDetectedMessage::streamOut(QDataStream &out)
{
    PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(out,<<);
}

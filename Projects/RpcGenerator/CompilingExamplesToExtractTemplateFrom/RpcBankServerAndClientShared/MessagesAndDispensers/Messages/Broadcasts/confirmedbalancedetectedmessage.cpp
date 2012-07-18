#include "confirmedbalancedetectedmessage.h"

ConfirmedBalanceDetectedMessage::ConfirmedBalanceDetectedMessage(QObject *owner)
    : IMessage(owner)
{ }
void ConfirmedBalanceDetectedMessage::streamIn(QDataStream &in)
{
    CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(in,>>);
}
void ConfirmedBalanceDetectedMessage::streamOut(QDataStream &out)
{
    CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(out,<<);
}

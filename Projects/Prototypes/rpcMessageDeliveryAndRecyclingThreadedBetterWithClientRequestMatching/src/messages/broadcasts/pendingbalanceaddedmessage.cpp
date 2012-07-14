#include "pendingbalanceaddedmessage.h"

PendingBalanceAddedMessage::PendingBalanceAddedMessage(QObject *mandatoryParent)
    : IMessage(mandatoryParent)
{ }
void PendingBalanceAddedMessage::streamIn(QDataStream &in)
{
    in >> Username;
    in >> PendingBalance;
}
void PendingBalanceAddedMessage::streamOut(QDataStream &out)
{
    out << Username;
    out << PendingBalance;
}

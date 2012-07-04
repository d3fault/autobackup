#include "pendingbalanceaddeddetectedmessage.h"

/*
void PendingBalanceAddedDetectedMessage::privateDeliver()
{
    emit this->pendingBalanceAddedDetected();
}
*/
void PendingBalanceAddedDetectedMessage::myDeliver()
{
    emit pendingBalanceAddedDetected();
}

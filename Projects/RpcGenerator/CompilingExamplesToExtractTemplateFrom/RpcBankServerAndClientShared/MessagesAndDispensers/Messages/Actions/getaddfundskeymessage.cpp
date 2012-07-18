#include "getaddfundskeymessage.h"

GetAddFundsKeyMessage::GetAddFundsKeyMessage(QObject *owner)
    : IMessage(owner)
{ }
void GetAddFundsKeyMessage::streamIn(QDataStream &in)
{

    TODO LEFT OFF
    //fuck, for the rpc server we'll be streaming OUT, but we want to use the 'input params' Username, not AddFundsKey. i need to waste protocol space in order to make this easy... but i think there's a better solution somehow, i just haven't thought of it yet

    //input 'params'
    in >> Username;
}
void GetAddFundsKeyMessage::streamOut(QDataStream &out)
{
    //return type or 'out' params
    out << AddFundsKey;
}
void GetAddFundsKeyMessage::getAddFundsKeyFailedUseExistingKeyFirst()
{
    emit getAddFundsKeyFailedUseExistingKeyFirstSignal();
}
void GetAddFundsKeyMessage::getAddFundsKeyFailedWaitForPendingToBeConfirmed()
{
    emit getAddFundsKeyFailedWaitForPendingToBeConfirmedSignal();
}

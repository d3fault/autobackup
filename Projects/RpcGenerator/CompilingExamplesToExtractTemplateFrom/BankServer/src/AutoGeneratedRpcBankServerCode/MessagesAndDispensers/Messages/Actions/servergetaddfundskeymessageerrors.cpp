#include "servergetaddfundskeymessageerrors.h"

ServerGetAddFundsKeyMessageErrors::ServerGetAddFundsKeyMessageErrors(QObject *parent) :
    IMessage(parent)
{ }
void ServerGetAddFundsKeyMessageErrors::getAddFundsKeyFailedUseExistingKeyFirst()
{
    emit getAddFundsKeyFailedUseExistingKeyFirstSignal();
}
void ServerGetAddFundsKeyMessageErrors::getAddFundsKeyFailedWaitForPendingToBeConfirmed()
{
    emit getAddFundsKeyFailedWaitForPendingToBeConfirmedSignal();
}
void ServerGetAddFundsKeyMessageErrors::getAddFundsKeyFailedUsernameDoesntExist()
{
    emit getAddFundsKeyFailedUsernameDoesntExistSignal();
}

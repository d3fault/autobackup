#include "servercreatebankaccountmessageerrors.h"

ServerCreateBankAccountMessageErrors::ServerCreateBankAccountMessageErrors(QObject *parent) :
    IMessage(parent)
{ }
void ServerCreateBankAccountMessageErrors::createBankAccountFailedUsernameAlreadyExists()
{
    emit createBankAccountFailedUsernameAlreadyExistsSignal();
}
void ServerCreateBankAccountMessageErrors::createBankAccountFailedPersistError()
{
    emit createBankAccountFailedPersistErrorSignal();
}

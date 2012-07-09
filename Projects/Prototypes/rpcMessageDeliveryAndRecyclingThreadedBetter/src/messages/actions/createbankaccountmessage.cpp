#include "createbankaccountmessage.h"

CreateBankAccountMessage::CreateBankAccountMessage(QObject *mandatoryParent)
    : IMessage(mandatoryParent)
{
}
void CreateBankAccountMessage::createBankAccountFailedUsernameAlreadyExists()
{
    emit createBankAccountFailedUsernameAlreadyExistsSignal();
}
void CreateBankAccountMessage::createBankAccountFailedPersistError()
{
    emit createBankAccountFailedPersistErrorSignal();
}
void CreateBankAccountMessage::streamIn(QDataStream &in)
{
    in >> Username;
}
void CreateBankAccountMessage::streamOut(QDataStream &out)
{
    out << Username;
}

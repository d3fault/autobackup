#include "createbankaccountmessage.h"

CreateBankAccountMessage::CreateBankAccountMessage(QObject *owner)
    : IMessage(owner)
{ }
void CreateBankAccountMessage::streamIn(QDataStream &in)
{
    in >> Username;
}
void CreateBankAccountMessage::streamOut(QDataStream &out)
{
    out << Username;
}

#include "clientcreatebankaccountmessage.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.h"

ClientCreateBankAccountMessage::ClientCreateBankAccountMessage(QObject *owner)
    : IMessage(owner)
{ }
void ClientCreateBankAccountMessage::streamIn(QDataStream &in)
{
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(in,>>);
}
void ClientCreateBankAccountMessage::streamOut(QDataStream &out)
{
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(out,<<);
}

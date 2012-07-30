#include "servercreatebankaccountmessage.h"

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessagebase.h"

ServerCreateBankAccountMessage::ServerCreateBankAccountMessage(QObject *owner)
    : ServerCreateBankAccountMessageErrors(owner)
{ }
void ServerCreateBankAccountMessage::streamIn(QDataStream &in)
{
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(in,>>);
}
void ServerCreateBankAccountMessage::streamOut(QDataStream &out)
{
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(out,<<);
}

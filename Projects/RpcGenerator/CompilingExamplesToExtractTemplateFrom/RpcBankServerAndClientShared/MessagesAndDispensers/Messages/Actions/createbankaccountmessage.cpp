#include "createbankaccountmessage.h"

CreateBankAccountMessage::CreateBankAccountMessage(QObject *parent)
    : IActionMessage(parent)
{ }
void CreateBankAccountMessage::resetMessageParameters()
{
    Username.clear();
}
void CreateBankAccountMessage::streamIn(QDataStream &in)
{
#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
    IActionMessage::streamSuccessAndErrorCodeIn(in);
#endif
#ifdef WE_ARE_RPC_BANK_SERVER_CODE
    IActionMessage::streamToggleBitIn(in);
#endif
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(in,>>);
}
void CreateBankAccountMessage::streamOut(QDataStream &out)
{
#ifdef WE_ARE_RPC_BANK_SERVER_CODE
    IActionMessage::streamSuccessAndErrorCodeOut(out);
#endif
#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
    IActionMessage::streamToggleBitIn(out);
#endif
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(out,<<);
}

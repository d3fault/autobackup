#include "createbankaccountmessage.h"

void CreateBankAccountMessage::setFailedUsernameAlreadyExists()
{
    setErrorCode(CreateBankAccountMessage::FailedUsernameAlreadyExists);
}
void CreateBankAccountMessage::setFailedPersistError()
{
    setErrorCode(CreateBankAccountMessage::FailedPersistError);
}

void ServerCreateBankAccountMessage::streamIn(QDataStream &in)
{
#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE
    IActionMessage::streamSuccessAndErrorCodeIn(in);
#endif
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(in,>>);
}
void ServerCreateBankAccountMessage::streamOut(QDataStream &out)
{
#ifdef WE_ARE_RPC_BANK_SERVER_CODE
    IActionMessage::streamSuccessAndErrorCodeOut(out);
#endif

    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(out,<<);
}

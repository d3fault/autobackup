#include "createbankaccountmessage.h"

CreateBankAccountMessage::CreateBankAccountMessage(QObject *owner)
    : IMessage(owner)
{ }
void CreateBankAccountMessage::streamIn(QDataStream &in)
{
    /*
    if(m_IsResponse)
    {
        in >> Success; //because if i don't have a bool Success, there is nothing i would put in here to indicate success...?????
    }
    else
    {
        in >> Username;
    }
    */
#ifdef RPC_BANK_SERVER_CLIENT_CODE
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(in,>>);
#else
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(in,>>);
#endif
}
void CreateBankAccountMessage::streamOut(QDataStream &out)
{
    /*
    if(m_IsResponse)
    {
        out << Success;
    }
    else
    {
        out << Username; //probably don't need this... well i mean we do for rpc client lol... but we don't need to send the Username back in the response... that's for sure
    }
    */
#ifdef RPC_BANK_SERVER_CODE
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(out,<<);
#else
    CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(out,<<);
#endif
}

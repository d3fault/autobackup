#ifndef CREATEBANKACCOUNTMESSAGEBASE_H
#define CREATEBANKACCOUNTMESSAGEBASE_H

#include <QString>

#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) qds strop Success;
#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) qds strop Username;

class CreateBankAccountMessageBase
{
public:
    enum CreateBankAccountFailedReasonsEnum
    {
        FailedUsernameAlreadyExists = 0,
        FailedPersistError
    };
    bool Success; //keeping this here so my stream operator doesn't break, will deal with it later TODOreq. i don't need or want it though... but have nothing else to return lmao (code needs to be ok with that)
    QString Username;
};

#endif // CREATEBANKACCOUNTMESSAGEBASE_H

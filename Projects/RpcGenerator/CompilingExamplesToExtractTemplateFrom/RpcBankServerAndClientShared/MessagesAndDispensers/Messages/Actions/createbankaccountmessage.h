#ifndef CREATEBANKACCOUNTMESSAGEBASE_H
#define CREATEBANKACCOUNTMESSAGEBASE_H

#include <QString>

#include "iactionmessage.h"

#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) qds strop Success;
#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) qds strop Username;

class CreateBankAccountMessage : public IActionMessage
{
    Q_OBJECT
public:
    enum CreateBankAccountFailedReasonsEnum
    {
        FailedUsernameAlreadyExists = 0x1, //TODOreq: make all my failed reason enums start at index of 1, because the Generic error type will be 0x0
        FailedPersistError
    };
    //bool Success; //keeping this here so my stream operator doesn't break, will deal with it later TODOreq. i don't need or want it though... but have nothing else to return lmao (code needs to be ok with that) <-- verify that not having a return value is ok. the success value is not a return value. CBAM doesn't have one, but GetAddFundsKey(username) does: the key!

    //Parameters
    QString Username;

    //Errors
    inline void setFailedUsernameAlreadyExists() { setErrorCode(CreateBankAccountMessage::FailedUsernameAlreadyExists); }
    inline void setFailedPersistError() { setErrorCode(CreateBankAccountMessage::FailedPersistError); }
protected:
    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // CREATEBANKACCOUNTMESSAGEBASE_H

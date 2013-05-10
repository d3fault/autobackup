#ifndef CREATEBANKACCOUNTMESSAGEBASE_H
#define CREATEBANKACCOUNTMESSAGEBASE_H

#include <QString>

#include "iactionmessage.h"

//TODOreq: verify that a response with no parameters like this works. had success their eariler just as a placeholder because I was afraid to have it empty. that semicolon is just a no-op, but might be able to be blank (compiler removes it anyways)
//#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) qds strop Success;
#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) Q_UNUSED(qds);
#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) qds strop Username;

class CreateBankAccountMessage : public IActionMessage
{
    Q_OBJECT
public:
    CreateBankAccountMessage(QObject *parent);
    enum CreateBankAccountFailedReasonsEnum
    {
        FailedUsernameAlreadyExists = 0x1, //TODOreq: make all my failed reason enums start at index of 1, because the Generic error type will be 0x0
        FailedPersistError
    };

    //Action Parameters
    QString Username;

    void resetMessageParameters();

    //Errors
    inline void setFailedUsernameAlreadyExists() { setErrorCode(CreateBankAccountMessage::FailedUsernameAlreadyExists); }
    inline void setFailedPersistError() { setErrorCode(CreateBankAccountMessage::FailedPersistError); }
protected:
    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // CREATEBANKACCOUNTMESSAGEBASE_H

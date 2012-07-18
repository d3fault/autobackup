#ifndef CREATEBANKACCOUNTMESSAGE_H
#define CREATEBANKACCOUNTMESSAGE_H

#include "../imessage.h"

#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) (qds strop Success;)
#define CREATE_BANK_ACCOUNT_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) (qds strop Username;)

class CreateBankAccountMessage : public IMessage
{
public:
    CreateBankAccountMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    QString Username;

    void createBankAccountFailedUsernameAlreadyExists();
    void createBankAccountFailedPersistError();
signals:
    void createBankAccountFailedUsernameAlreadyExistsSignal();
    void createBankAccountFailedPersistErrorSignal();
};

#endif // CREATEBANKACCOUNTMESSAGE_H

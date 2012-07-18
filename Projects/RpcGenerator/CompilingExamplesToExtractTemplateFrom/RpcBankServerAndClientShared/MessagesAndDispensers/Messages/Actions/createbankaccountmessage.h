#ifndef CREATEBANKACCOUNTMESSAGE_H
#define CREATEBANKACCOUNTMESSAGE_H

#include "../imessage.h"

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

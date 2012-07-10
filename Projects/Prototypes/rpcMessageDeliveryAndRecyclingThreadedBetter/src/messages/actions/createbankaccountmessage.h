#ifndef CREATEBANKACCOUNTMESSAGE_H
#define CREATEBANKACCOUNTMESSAGE_H

#include "../imessage.h"

class CreateBankAccountMessage : public IMessage
{
    Q_OBJECT
public:
    CreateBankAccountMessage(QObject *mandatoryParent);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    QString Username;
    //maybe bank sub-account username is associated with? create bank account messages need this information too at some point... however, actions know who to respond to because the request is pending... but broadcasts have to figure it out some other (more persistent) way..

    void createBankAccountFailedUsernameAlreadyExists();
    void createBankAccountFailedPersistError();
signals:
    void createBankAccountFailedUsernameAlreadyExistsSignal();
    void createBankAccountFailedPersistErrorSignal();
};

#endif // CREATEBANKACCOUNTMESSAGE_H

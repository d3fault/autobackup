#ifndef SERVERCREATEBANKACCOUNTMESSAGEERRORS_H
#define SERVERCREATEBANKACCOUNTMESSAGEERRORS_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"

class ServerCreateBankAccountMessageErrors : public IMessage
{
    Q_OBJECT
public:
    explicit ServerCreateBankAccountMessageErrors(QObject *parent = 0);

    void createBankAccountFailedUsernameAlreadyExists();
    void createBankAccountFailedPersistError();
signals:
    void createBankAccountFailedUsernameAlreadyExistsSignal();
    void createBankAccountFailedPersistErrorSignal();
};

#endif // SERVERCREATEBANKACCOUNTMESSAGEERRORS_H

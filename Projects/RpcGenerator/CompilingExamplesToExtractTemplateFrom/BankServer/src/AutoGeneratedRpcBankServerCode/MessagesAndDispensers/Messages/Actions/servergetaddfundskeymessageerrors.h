#ifndef SERVERGETADDFUNDSKEYMESSAGEERRORS_H
#define SERVERGETADDFUNDSKEYMESSAGEERRORS_H

#include "../../../../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/imessage.h"

class ServerGetAddFundsKeyMessageErrors : public IMessage
{
    Q_OBJECT
public:
    explicit ServerGetAddFundsKeyMessageErrors(QObject *parent = 0);

    void getAddFundsKeyFailedUsernameDoesntExist();
    void getAddFundsKeyFailedUseExistingKeyFirst();
    void getAddFundsKeyFailedWaitForPendingToBeConfirmed();
signals:
    void getAddFundsKeyFailedUsernameDoesntExistSignal();
    void getAddFundsKeyFailedUseExistingKeyFirstSignal();
    void getAddFundsKeyFailedWaitForPendingToBeConfirmedSignal();
};

#endif // SERVERGETADDFUNDSKEYMESSAGEERRORS_H

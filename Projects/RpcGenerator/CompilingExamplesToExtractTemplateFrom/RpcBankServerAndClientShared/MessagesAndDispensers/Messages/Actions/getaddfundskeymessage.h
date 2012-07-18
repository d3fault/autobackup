#ifndef GETADDFUNDSKEYMESSAGE_H
#define GETADDFUNDSKEYMESSAGE_H

#include "../imessage.h"

class GetAddFundsKeyMessage : public IMessage
{
public:
    GetAddFundsKeyMessage(QObject *owner);

    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);

    QString Username;
    QString AddFundsKey;

    void getAddFundsKeyFailedUseExistingKeyFirst();
    void getAddFundsKeyFailedWaitForPendingToBeConfirmed();
signals:
    void getAddFundsKeyFailedUseExistingKeyFirstSignal();
    void getAddFundsKeyFailedWaitForPendingToBeConfirmedSignal();
};

#endif // GETADDFUNDSKEYMESSAGE_H

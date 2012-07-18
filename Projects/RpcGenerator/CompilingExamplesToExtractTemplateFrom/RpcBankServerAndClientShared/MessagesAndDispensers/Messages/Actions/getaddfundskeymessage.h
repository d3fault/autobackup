#ifndef GETADDFUNDSKEYMESSAGE_H
#define GETADDFUNDSKEYMESSAGE_H

#include "../imessage.h"

#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) (qds strop AddFundsKey;)
#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) (qds strop Username;)

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

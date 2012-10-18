#ifndef GETADDFUNDSKEYMESSAGEBASE_H
#define GETADDFUNDSKEYMESSAGEBASE_H

#include <QString>

#include "iactionmessage.h"

#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) qds strop AddFundsKey;
#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) qds strop Username;

class GetAddFundsKeyMessage : public IActionMessage
{
    Q_OBJECT
public:
    enum GetAddFundsKeyFailedReasonsEnum
    {
        FailedUsernameDoesntExist = 0x1,
        FailedUseExistingKeyFirst,
        FailedWaitForPendingToBeConfirmed
    };

    //Parameters
    QString Username;
    QString AddFundsKey;

    //Errors
    inline void setFailedUsernameDoesntExist();
    inline void setFailedUseExistingKeyFirst();
    inline void setFailedWaitForPendingToBeConfirmed();
protected:
    void streamIn(QDataStream &in);
    void streamOut(QDataStream &out);
};

#endif // GETADDFUNDSKEYMESSAGEBASE_H

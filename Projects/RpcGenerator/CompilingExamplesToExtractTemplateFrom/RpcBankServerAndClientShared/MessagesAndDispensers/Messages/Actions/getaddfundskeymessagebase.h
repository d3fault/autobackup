#ifndef GETADDFUNDSKEYMESSAGEBASE_H
#define GETADDFUNDSKEYMESSAGEBASE_H

#include <QString>

#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_RESPONSE_VALUES(qds,strop) qds strop AddFundsKey;
#define GET_ADD_FUNDS_KEY_MESSAGE_PARSE_AS_ACTION_REQUEST_PARAMS(qds,strop) qds strop Username;

class GetAddFundsKeyMessageBase
{
public:
    enum GetAddFundsKeyFailedReasonsEnum
    {
        FailedUsernameDoesntExist = 0,
        FailedUseExistingKeyFirst,
        FailedWaitForPendingToBeConfirmed
    };
    QString Username;
    QString AddFundsKey;
};

#endif // GETADDFUNDSKEYMESSAGEBASE_H

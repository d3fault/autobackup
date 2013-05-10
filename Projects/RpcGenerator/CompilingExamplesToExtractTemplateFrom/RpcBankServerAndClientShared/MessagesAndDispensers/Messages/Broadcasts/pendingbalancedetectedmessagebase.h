#ifndef PENDINGBALANCEDETECTEDMESSAGEBASE_H
#define PENDINGBALANCEDETECTEDMESSAGEBASE_H

#include <QString>

#define PENDING_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(qds,strop) qds strop Username; qds strop PendingBalance;

class PendingBalanceDetectedMessageBase
{
public:
    QString Username;
    double PendingBalance;

    void resetMessageParametersBase();
};

#endif // PENDINGBALANCEDETECTEDMESSAGEBASE_H

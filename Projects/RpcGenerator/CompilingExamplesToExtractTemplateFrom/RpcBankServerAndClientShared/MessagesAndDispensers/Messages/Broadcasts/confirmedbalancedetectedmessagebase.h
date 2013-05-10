#ifndef CONFIRMEDBALANCEDETECTEDMESSAGEBASE_H
#define CONFIRMEDBALANCEDETECTEDMESSAGEBASE_H

#include <QString>

#define CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(qds,strop) qds strop Username; qds strop ConfirmedBalance;

class ConfirmedBalanceDetectedMessageBase
{
public:
    QString Username;
    double ConfirmedBalance;

    void resetMessageParametersBase();
};

#endif // CONFIRMEDBALANCEDETECTEDMESSAGEBASE_H

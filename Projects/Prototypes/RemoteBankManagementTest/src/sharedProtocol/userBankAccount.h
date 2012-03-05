#ifndef USERBANKACCOUNT_H
#define USERBANKACCOUNT_H

#include <QString>

#define DEFAULT_EMPTY_ADD_FUNDS_BITCOIN_KEY "lolwut"

struct UserBankAccount
{
    enum AddFundsStatuses
    {
        AwaitingPayment,
        PaymentPending,
        PaymentConfirmed
    };

    double Balance;
    QString AddFundsBitcoinKey;
    AddFundsStatuses AddFundsStatus;
};

#endif // USERBANKACCOUNT_H

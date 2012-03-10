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

    double Balance; //i think a pending balance is in order, and when i change AddFundsStatus to Confirmed, i zero it out and add it to my regular balance
    QString AddFundsBitcoinKey;
    AddFundsStatuses AddFundsStatus;
};

#endif // USERBANKACCOUNT_H

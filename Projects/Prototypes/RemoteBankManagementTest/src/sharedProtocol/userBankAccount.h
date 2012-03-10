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

    double Balance; //i think a pending balance is in order, and when i change AddFundsStatus to Confirmed, i zero it out and add it to my regular balance.... or i do nothing with the signal that the gui receives except present it to the user... and then add the confirmed signal to the Balance. but if they log in after the signal has been sent, they won't see the pending balance (unless i add the proposed PendingBalance here lol)
    QString AddFundsBitcoinKey;
    AddFundsStatuses AddFundsStatus;
};

#endif // USERBANKACCOUNT_H

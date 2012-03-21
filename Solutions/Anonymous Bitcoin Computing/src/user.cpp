#include "user.h"

#include <Wt/Dbo/Impl>

DBO_INSTANTIATE_TEMPLATES(User);

User::User()
    : PendingBalance(0.0), ConfirmedBalance(0.0), AddFundStatus(PaymentConfirmed)
{
}

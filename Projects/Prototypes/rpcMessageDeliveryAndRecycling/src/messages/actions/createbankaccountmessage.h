#ifndef CREATEBANKACCOUNTMESSAGE_H
#define CREATEBANKACCOUNTMESSAGE_H

#include "../imessage.h"

class CreateBankAccountMessage : public IMessage
{
signals:
    void createBankAccountCompleted();
};

#endif // CREATEBANKACCOUNTMESSAGE_H

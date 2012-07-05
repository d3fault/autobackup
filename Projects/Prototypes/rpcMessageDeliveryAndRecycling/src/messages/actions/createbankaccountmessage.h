#ifndef CREATEBANKACCOUNTMESSAGE_H
#define CREATEBANKACCOUNTMESSAGE_H

#include "../imessage.h"

class CreateBankAccountMessage : public IMessage
{
    Q_OBJECT
public:
    void myDeliver();
signals:
    void createBankAccountCompleted();
protected:
    //void privateDeliver();
};

#endif // CREATEBANKACCOUNTMESSAGE_H

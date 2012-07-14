#ifndef ACTIONDISPENSERS_H
#define ACTIONDISPENSERS_H

#include "actions/createbankaccountmessagedispenser.h"

class ActionDispensers
{
public:
    ActionDispensers();
    void setCreateBankAccountMessageDispenser(CreateBankAccountMessageDispenser *createBankAccountMessageDispenser);
    CreateBankAccountMessageDispenser *createBankAccountMessageDispenser();
    //etc

    //bool everyDispenserIsCreated();
private:
    CreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    //etc
};

#endif // ACTIONDISPENSERS_H

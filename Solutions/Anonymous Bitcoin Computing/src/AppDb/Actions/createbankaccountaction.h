#ifndef CREATEBANKACCOUNTACTION_H
#define CREATEBANKACCOUNTACTION_H

#include "../applogicaction.h"

class CreateBankAccountAction : public AppLogicAction
{
public:
    bool raceCondition1detected();
    bool raceCondition2detected();
    bool needsToCommunicateWithBankServer();

    BankServerAction *buildBankRequest(); //unsure if this is Action or Request (will responses be in same object?)

    void performAction();
};

#endif // CREATEBANKACCOUNTACTION_H

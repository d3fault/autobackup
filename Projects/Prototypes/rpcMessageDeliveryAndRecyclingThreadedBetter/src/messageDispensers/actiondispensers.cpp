#include "actiondispensers.h"

ActionDispensers::ActionDispensers()
    : m_CreateBankAccountMessageDispenser(0)
{ }
void ActionDispensers::setCreateBankAccountMessageDispenser(CreateBankAccountMessageDispenser *createBankAccountMessageDispenser)
{
    m_CreateBankAccountMessageDispenser = createBankAccountMessageDispenser;
}
CreateBankAccountMessageDispenser * ActionDispensers::createBankAccountMessageDispenser()
{
    return m_CreateBankAccountMessageDispenser;
}
//no need to check if every action dispenser is created on rpc server, but this code will come in handy on rpc client
/*bool ActionDispensers::everyDispenserIsCreated()
{
    if(!m_CreateBankAccountMessageDispenser)
        return false;

    //etc for every action dispenser

    //then:
    return true;
}*/

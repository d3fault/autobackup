#include "actiondispensers.h"

ActionDispensers::ActionDispensers(IAcceptMessageDeliveries *destination)
    : m_Destination(destination)
{
}
CreateBankAccountMessageDispenser *ActionDispensers::takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner)
{
    m_CreateBankAccountMessageDispenser = new CreateBankAccountMessageDispenser(m_Destination, owner);
}

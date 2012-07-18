#ifndef ACTIONDISPENSERS_H
#define ACTIONDISPENSERS_H

class ActionDispensers
{
public:
    ActionDispensers(IAcceptMessageDeliveries *destination);
    CreateBankAccountMessageDispenser *takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner);
private:
    IAcceptMessageDeliveries *m_Destination;
};

#endif // ACTIONDISPENSERS_H

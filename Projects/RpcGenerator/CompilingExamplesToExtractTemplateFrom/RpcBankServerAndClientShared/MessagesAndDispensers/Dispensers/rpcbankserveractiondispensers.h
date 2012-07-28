#ifndef RPCBANKSERVERACTIONDISPENSERS_H
#define RPCBANKSERVERACTIONDISPENSERS_H

#include "../../MessagesAndDispensers/Dispensers/Actions/createbankaccountmessagedispenser.h"
#include "../../MessagesAndDispensers/Dispensers/Actions/getaddfundskeymessagedispenser.h"

// the action dispenser is the same on rpc client and server. it uses a destination and owner on both ends, so we can still leave this in the rpc client/server shared folder

//EXCEPT blah
//we use .deliver() in two different ways with Actions
//once in rpc client for sending it to IBankServerClientProtocolKnower (hiding behind RpcBankServerHelper)
//another in rpc server when responding back to IBankServerProtocolKnower after processing request

//HOWEVER, we can't use the same messages or else i'd need to rig onto the same interface. hence those weird macros i have in the message stream operator code
//we can do the streaming fine by means of macros, but the interface that the object connects it's own deliverSignal() to?
//is that a question?
//i don't think they match

//meh i'm going to lazy-figure-out if i can use ActionDispensers in the client. for now, just make it a server-only class, same with the IRpcBankServer class
class RpcBankServerActionDispensers
{
public:
    RpcBankServerActionDispensers(QObject *destination);
    CreateBankAccountMessageDispenser *takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(QObject *owner);
    GetAddFundsKeyMessageDispenser *takeOwnershipOfGetAddFundsKeyMessageDispenserRiggedForDelivery(QObject *owner);
    bool everyDispenserIsCreated();
private:
    QObject *m_Destination;

    CreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
    GetAddFundsKeyMessageDispenser *m_GetAddFundsKeyMessageDispenser;
};

#endif // RPCBANKSERVERACTIONDISPENSERS_H

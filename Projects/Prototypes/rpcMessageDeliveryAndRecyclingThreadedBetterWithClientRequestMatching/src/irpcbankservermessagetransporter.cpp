#include "irpcbankservermessagetransporter.h"

#include "rpcbankserverclientshelper.h"
#include "messageDispensers/actions/createbankaccountmessagedispenser.h"

IRpcBankServerMessageTransporter::IRpcBankServerMessageTransporter()
{ }
void IRpcBankServerMessageTransporter::takeOwnershipOfAllActionDispensers(RpcBankServerClientsHelper *rpcBankServerClientsHelper)
{
    m_CreateBankAccountMessageDispenser = rpcBankServerClientsHelper->takeOwnershipOfCreateBankAccountMessageDispenserRiggedForDelivery(this);
    //we could also do dispenser->setDestinationObject(this)... but takeOwnership will do it for us to remain uniform with broadcast dispensers and to simplify code generation
    //not that it matters, just a mental note: for actions, the dispenser owner and the message destination are both this same (this)
}

#include "createbankaccountmessagedispenser.h"

IMessage *CreateBankAccountMessageDispenser::getNewOfTypeAndConnectToDestinationObject()
{
    CreateBankAccountMessage *newMessage = new CreateBankAccountMessage();
    connect(newMessage, SIGNAL(createBankAccountCompleted()), m_DestinationObject, SLOT(createBankAccountCompleted()));
    //TODOreq: failed reasons
    //TODOreq: do they also have a specific public method that triggers these signals? void complete() { emit createBankAccountCompleted(); } etc? now i'm getting back into IDeliver territory. just don't forget the failed reasons shit~!!!!

    return newMessage;
}

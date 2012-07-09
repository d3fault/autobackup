#include "irpcbusiness.h"

IRpcBusiness::IRpcBusiness(QObject *parent) :
    QObject(parent)
{
    m_BroadcastDispensers = new BroadcastDispensers();
}
void IRpcBusiness::organizeBackendBusinessObjectsThreadsAndStartThem(IRpcBusinessController *rpcBusinessController)
{
    setParentBackendBusinessObjectForEveryBroadcastDispenser(rpcBusinessController);

    //since the impl is in charge of making sure every dispenser has an owner elected, we check his work before continuing
    if(!m_BroadcastDispensers->everyDispenserIsCreated())
    {
        //TODOreq: error out or stop somehow if not every dispenser is created. indicates error of usage in business impl
    }
    else
    {
        //then, after the electing above, the dispensers are children of the threads about to be moved... so now we move them... and then start them
        moveBackendBusinessObjectsToTheirOwnThreadsAndStartThem();
    }
}
BroadcastDispensers *IRpcBusiness::broadcastDispensers()
{
    return m_BroadcastDispensers;
}

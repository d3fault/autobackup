#include "irpcbusiness.h"

IRpcBusiness::IRpcBusiness(QObject *parent) :
    QObject(parent)
{
    m_BroadcastDispensers = new BroadcastDispensers();
}
void IRpcBusiness::organizeThreads(IRpcClientsHelper *clientsHelper)
{
    setParentForEveryBroadcastDispenser(clientsHelper);

    //since the impl is in charge of making sure every dispenser has an owner elected, we check his work before continuing
    if(!m_BroadcastDispensers->everyDispenserIsCreated())
    {
        //TODOreq: error out or stop somehow if not every dispenser is created. indicates error of usage in business impl
    }
    else
    {
        //then, after the electing above, the dispensers are children of the threads about to be moved (and so also get moved)
        organizeThreadsImpl();

        //hack: start the thread because it's a thread that's behind the scenes and we want to make sure we're on the GUI thread when we **PUSH** the object onto the right thread. (when .moveToThread actually does it's shit?). idfk if this is a valid argument or not anymore
        startBackendImplThreads();
    }
}
BroadcastDispensers *IRpcBusiness::broadcastDispensers()
{
    return m_BroadcastDispensers;
}

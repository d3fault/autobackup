#ifndef PENDINGBALANCEADDEDDETECTEDARGS_H
#define PENDINGBALANCEADDEDDETECTEDARGS_H

#include <QList>
#include <QAtomicInt>

struct PendingBalanceAddedDetectedArgs
{
    static PendingBalanceAddedDetectedArgs *getNewOrRecycled()
    {
        QList<PendingBalanceAddedDetectedArgs*> m_PullingList;
        QList<PendingBalanceAddedDetectedArgs*> m_RecycleList;
        QAtomicInt m_SharedAtomicIntInsteadOfMutexLocking; //if i can't solve 'wat do', just use a mutex

        if(ourListIsEmpty())
        {
            if(recycleListHasSome())
            {
                //swap lists. might need to increment atomic int yet again during the swap so the recycler doesn't try to access it during the swap?
            }
            else
            {
                //new some for immediate use
            }
        }
        else
        {
            //deQueue it and return it
        }
    }
    static recycleUsed(PendingBalanceAddedDetectedArgs *used)
    {
        if(listsArentCurrentlyBeingSwapped())
        {
            //append to recycled list
        }
        else
        {
            //wat do
        }
    }
};

#endif // PENDINGBALANCEADDEDDETECTEDARGS_H

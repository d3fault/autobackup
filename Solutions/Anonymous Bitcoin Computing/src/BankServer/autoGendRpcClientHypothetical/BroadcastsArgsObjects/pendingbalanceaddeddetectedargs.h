#ifndef PENDINGBALANCEADDEDDETECTEDARGS_H
#define PENDINGBALANCEADDEDDETECTEDARGS_H

#include <QList>
#include <QMutex>

//one of them will have to use a Mutex I've decided
//and I've also decided that it's the ::get
//the reason being is that the ::recycle will be in the network thread, and he is busy with Actions and does not want to wait on a mutex
//broadcasts are less frequent, especially in this use case

//still am having a hard time conceptualizing how this will work
//but i CAN conceptualize the opposite: recyle locking the mutex every time it calls
//so ::get only has to lock the mutex whenever the lists are swapped

//so i just need to invert ^ that logic
//i'm going to try to do it without thinking
//as in, just typing and replacing the opposite thread
//then examining afterwards to see if it makes sense
//this should be fun

//but i CAN conceptualize the one i want: get locking the mutex every time it calls
//so ::recycle only has to lock the mutex whenever the lists are swapped

//fuck
//dunno if this will work
//because it's the ::get that does the check to see if lists need to be swapped (if the get list is empty)

//basically the problem is this:
//the list that is to be checked if empty (triggering a swap) cannot be the list we mutex guard
//and it is, in the second scenario (the one i 'want')

//there might be an optimization using 2 readwrite locks both in read mode to check sizes etc
//and only get a write lock when swapping?

//or are threads faster at this point
//thread vs mutex
//that is the question

//the real answer: it depends on the 'object' size

//so if you were to write a test, you'd need to scale that oject up
//actually considering this is all pointer shit, it shouldn't matter
//but if it weren't, it would


//OR, are both of those optimizations stupid and i should just use stack + reference counting (which means frequent dealloc/realloc methinks)?????


//damn ActionArgs are so much easier since the ::get happens in the same thread as the ::recycle


//ok I've convinced myself I'm going to write a test for each case (including the same-thread case?)
//should be like 6 tests in total...
//gonna use md5 summing as the data generating thread (bitcoin thread in this case), then sha1 as the data processing thread (network thread in this case)

//might even post my results/findings on teh forums

//yat :(

struct PendingBalanceAddedDetectedArgs
{
    static QList<PendingBalanceAddedDetectedArgs*> *m_PullingList;
    static QList<PendingBalanceAddedDetectedArgs*> *m_RecycleList;
    static QAtomicInt m_SharedAtomicIntInsteadOfMutexLocking; //if i can't solve 'wat do', just use a mutex

    static PendingBalanceAddedDetectedArgs *getNewOrRecycled()
    {
        if(m_PullingList.isEmpty())
        {
            //if(!m_RecycleList.isEmpty()) -- can't do this because that list is accessed on other thread. this is what the atomic int is for
            if(m_SharedAtomicIntInsteadOfMutexLocking.testAndSetAcquire(1, 2))
            {
                qSwap(m_PullingList, m_RecycleList);
                return m_PullingList->takeLast();
            }
            else
            {
                return new PendingBalanceAddedDetectedArgs();
            }
        }
        else
        {
            return m_PullingList->takeLast();
        }
    }
    static recycleUsed(PendingBalanceAddedDetectedArgs *used)
    {
        if(m_SharedAtomicIntInsteadOfMutexLocking != 2)
        {

            if(m_SharedAtomicIntInsteadOfMutexLocking == 1)
            {
                //race condition lists could enter swapping before i call this append
                m_RecycleList->append(used);
            }
            else if(m_SharedAtomicIntInsteadOfMutexLocking.testAndSetAcquire(0, 1))
            {
                m_RecycleList->append(used);
            }
        }
        else
        {
            //wat do
        }
    }
};

QList<PendingBalanceAddedDetectedArgs*> PendingBalanceAddedDetectedArgs::m_PullingList = new QList<PendingBalanceAddedDetectedArgs*>();
QList<PendingBalanceAddedDetectedArgs*> PendingBalanceAddedDetectedArgs::m_RecycleList = new QList<PendingBalanceAddedDetectedArgs*>();


#endif // PENDINGBALANCEADDEDDETECTEDARGS_H

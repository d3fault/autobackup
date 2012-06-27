#ifndef PENDINGBALANCEADDEDDETECTEDARGS_H
#define PENDINGBALANCEADDEDDETECTEDARGS_H

#include <QObject>
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
    static QList<PendingBalanceAddedDetectedArgs*> *m_RecycleList;

    static PendingBalanceAddedDetectedArgs *getNewOrRecycled()
    {
        if(!m_RecycleList.isEmpty())
        {
            return m_RecycleList->takeLast();
        }
        else
        {
            PendingBalanceAddedDetectedArgs *newArgs = new PendingBalanceAddedDetectedArgs();
            connect(newArgs, SIGNAL(doneUsingArgObject()), this, SLOT(recycledArgObject())); //Auto-Connection means I don't have to worry about cross-threads :-D. same-thread (Actions) will be handled for me. Hmm I think does this also mean they can share a base type?
            //I would say the answer to that is yes, TODOreq: SO LONG AS the base type doesn't pull from a shared List<NetworkMessages> to use as their backing object. Actions would pull from the list in network thread, broadcasts would pull from it in Bitcoin thread. The solution: either have all actions share a NetworkMessageList and each Broadcast gets it's own NetworkMessageList, OR both every Action and Broadcast get their own NetworkMessageList. for actions they can most likely share~, but Broadcasts most likely cannot (even with each other). I am not willing to use a QMutex
            return newArgs;
        }
    }
    static recycleUsed(PendingBalanceAddedDetectedArgs *used)
    {
        m_RecycleList->append(used);
    }
};

#endif // PENDINGBALANCEADDEDDETECTEDARGS_H

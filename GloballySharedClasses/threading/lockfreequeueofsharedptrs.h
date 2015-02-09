#ifndef LOCKFREEQUEUEOFSHAREDPTRS_H
#define LOCKFREEQUEUEOFSHAREDPTRS_H

#include <boost/lockfree/queue.hpp>
#include <boost/shared_ptr.hpp>

//BOOST lockfree::queue of BOOST shared_ptr -- shared_ptr can't go in lockfree::queue, but a pointer to a shared_ptr can, so this class gives the illusion of using a shared_ptr directly in a lockfree::queue

//#define LOCKFREEQUEUEOFSHAREDPTRS_USE_ADDITIONAL_QUEUE_FOR_GET_NEW_OR_RECYCLED_HEAP_COPY 1

#ifndef LOCKFREEQUEUEOFSHAREDPTRS_USE_ADDITIONAL_QUEUE_FOR_GET_NEW_OR_RECYCLED_HEAP_COPY

template <class T>
class LockfreeQueueOfSharedPtrs //Thought about inheriting lockfree::queue, but I'd be hiding pop/push methods instead of reimplementing them, which could lead to programming errors. Any lockfree::queue method I need can easily be forwarded on an as-needed basis through this class, however
{
private:
    boost::lockfree::queue<boost::shared_ptr<T>*> m_Queue;
    LockfreeQueueOfSharedPtrs(const LockfreeQueueOfSharedPtrs &other) { (void)other; } //disable copy constructor
public:
    LockfreeQueueOfSharedPtrs(int initialCapacityOfLockfreeQueue) : m_Queue(initialCapacityOfLockfreeQueue) { }
    inline bool push(const boost::shared_ptr<T> &toPush)
    {
        boost::shared_ptr<T> *heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage = new boost::shared_ptr<T>(toPush); //reference count += 1
        if(!m_Queue.push(heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage))
        {
            delete heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage;
            return false;
        }
        return true;
    }
    inline bool pop(boost::shared_ptr<T> &popTo)
    {
        boost::shared_ptr<T> *theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime;
        if(m_Queue.pop(theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime))
        {
            popTo = *theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime;
            delete theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime; //reference count -= 1
            return true;
        }
        return false;
    }
};

#else // def LOCKFREEQUEUEOFSHAREDPTRS_USE_ADDITIONAL_QUEUE_FOR_GET_NEW_OR_RECYCLED_HEAP_COPY //because "new" uses a mutex underneath xD -- however, my testing showed that this "optimization" is slower xD. Even though heap allocs are much more expensive than stack allocs, this class buys me readability, simplicity, understandability, etc... not [cpu] efficiency (arguably does give me 'codan' efficiency tho)

template <class T>
class LockfreeQueueOfSharedPtrs
{
private:
    boost::lockfree::queue<boost::shared_ptr<T>*> m_Queue;
    boost::lockfree::queue<boost::shared_ptr<T>*> m_RecycledQueue;
    LockfreeQueueOfSharedPtrs(const LockfreeQueueOfSharedPtrs &other) { (void)other; } //disable copy constructor
public:
    LockfreeQueueOfSharedPtrs(int initialCapacityOfLockfreeQueue) : m_Queue(initialCapacityOfLockfreeQueue), m_RecycledQueue(initialCapacityOfLockfreeQueue) { }
    inline bool push(const boost::shared_ptr<T> &toPush)
    {
        boost::shared_ptr<T> *heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage;
        if(m_RecycledQueue.pop(heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage))
        {
            //recycled (cache hit)
            *heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage = toPush; //reference count += 1
        }
        else
        {
            //new (cache miss)
            heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage = new boost::shared_ptr<T>(toPush); //reference count += 1
        }

        if(!m_Queue.push(heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage))
        {
            heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage->reset();
            if(!m_RecycledQueue.push(heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage))
            {
                delete heapAllocatedSharedPointerJustToKeepReferenceCountPositiveDuringLockfreeQueueStorage;
            }
            return false;
        }
        return true;
    }
    inline bool pop(boost::shared_ptr<T> &popTo)
    {
        boost::shared_ptr<T> *theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime;
        if(m_Queue.pop(theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime))
        {
            popTo = *theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime;
            theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime->reset(); //reference count -= 1
            if(!m_RecycledQueue.push(theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime)) //recycle
            {
                delete theHeapAllocatedSharedPointerLiterallyStoredInLockfreeQueueButAlsoHackilyUsedToKeepReferenceCountPositiveDuringThatTime;
            }
            return true;
        }
        return false;
    }
};

#endif // LOCKFREEQUEUEOFSHAREDPTRS_USE_ADDITIONAL_QUEUE_FOR_GET_NEW_OR_RECYCLED_HEAP_COPY

#endif // LOCKFREEQUEUEOFSHAREDPTRS_H

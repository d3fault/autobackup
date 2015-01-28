#ifndef LOCKFREEQUEUEOFSHAREDPTRS_H
#define LOCKFREEQUEUEOFSHAREDPTRS_H

#include <boost/lockfree/queue.hpp>
#include <boost/shared_ptr.hpp>

//BOOST lockfree::queue of BOOST shared_ptr -- shared_ptr can't go in lockfree::queue, but a pointer to a shared_ptr can, so this class gives the illusion of using a shared_ptr directly in a lockfree::queue
template <class T>
class LockfreeQueueOfSharedPtrs //Thought about inheriting lockfree::queue, but I'd be hiding pop/push methods instead of reimplementing them, which could lead to programming errors. Any lockfree::queue method I need can easily be forwarded on an as-needed basis through this class, however
{
private:
    boost::lockfree::queue<boost::shared_ptr<T>*> m_Queue;
    LockfreeQueueOfSharedPtrs(const LockfreeQueueOfSharedPtrs &other) { (void)other; } //disable copy constructor
public:
    LockfreeQueueOfSharedPtrs(int capacityOfLockfreeQueue) : m_Queue(capacityOfLockfreeQueue) { }
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

#endif // LOCKFREEQUEUEOFSHAREDPTRS_H

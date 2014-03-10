#ifndef BOOSTTHREADINITIALIZATIONSYNCHRONIZATIONKIT_H
#define BOOSTTHREADINITIALIZATIONSYNCHRONIZATIONKIT_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/lock_guard.hpp>

template<class T>
class BoostThreadInitializationSynchronizationKit //high fives self
{
public:
    BoostThreadInitializationSynchronizationKit()
        : isInitialized(false)
    { }
    boost::mutex InitializationMutex;
    boost::condition_variable InitializationWaitCondition;
    bool isInitialized;
    T *ObjectThatFinishedInitializing;
    void notifyThatInitializationIsComplete(T *objectThatFinishedInitializing)
    {
        {
            boost::lock_guard<boost::mutex> initializationLock(InitializationMutex);
            isInitialized = true;
            ObjectThatFinishedInitializing = objectThatFinishedInitializing;
        }
        InitializationWaitCondition.notify_one();
    }
};

#endif // BOOSTTHREADINITIALIZATIONSYNCHRONIZATIONKIT_H

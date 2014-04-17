#include "objectssynchronizer.h"

#if 0 //orig idea/design
class InitializationSynchronizer : public QObject
{
Q_OBJECT
public:
    explicit InitializationSynchronizer(bool cancelAllWhenOneFails = true, QObject *parent = 0);

    //async api 'begin'
    void addAndConstructAndInitializeAndStartObject<T-QObject-derived>(“initializeAndStartSlot”, “initializedAndStartedSignal_AKA_object_ready_for_connections”, int separateThreadGroupId = -1 /* -1 means same thread as 'this' */);
    void stopAndDestroyAndDestructAndRemoveAllObjects(); //this is, or can be, considered "cancel".. but the implementation of it needs to be smartish. each non-failing object needs to be given time to finish initializing and starting before doing the stopping. but that's the uncommon case. the common case it just begins an asynchronous stop

    //sync api
    bool waitForAllObjectsToConstructAndInitializeAndStart(); //returns false if at least one fails, cancels too depending on constructor arg. we could optionally call stopAndWaitForAllObjectsToFinishStoppingAndDestroyingAndRemoving if one fails before returning false, so the user doesn't have to. what i mean is that it definitely STARTS "stopping" if cancelAllWhenOneFails is true, but it doesn't necessarily mean that we wait for the stopping to finish (but could)
    void stopAndWaitForAllObjectsToFinishStoppingAndDestroyingAndRemoving();

    //async api 'finish'
signals:
    allIObjectsInitialized();
    atLeastOneObjectFailedToInitialize(); //emitted after all "initializedAndStartedSignal"s, so as long as all of the slots connected to said signals are in same thread as slot connected to this signal, we know we received it after they are all initialized and started
};

#endif

QObject *ObjectIsReadyForConnectionsOnlyRelayer::m_SynchronizerNotifiee = 0;

//objects ON threads (staying there, 'processing events' (messages)). objects AS events/messages passed between threads all the time. my head hurts
//i can do this but not as pro as i want to, can't add an [template] object to a thread already running [and have the object instantiate on the thread]

ObjectsSynchronizer::ObjectsSynchronizer(QObject *synchronizerNotifiee, bool cancelAllWhenOneFails, QObject *parent)
    : QObject(parent)
    , m_SynchronizerNotifiee(synchronizerNotifiee)
    , m_CancelAllWhenOneFails(cancelAllWhenOneFails)
{
    ObjectIsReadyForConnectionsOnlyRelayer::m_SynchronizerNotifiee = synchronizerNotifiee;
}

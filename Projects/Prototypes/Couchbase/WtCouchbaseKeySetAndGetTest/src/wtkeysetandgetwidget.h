#ifndef WTKEYSETANDGETWIDGET_H
#define WTKEYSETANDGETWIDGET_H

#ifndef NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS
#define NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS 10 //TODOreq: single definition... but this is just a hacky prototype anyways (turning out to be useful/functional as fuck though :-P)
#endif

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WBreak>

#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/random/taus88.hpp> //fast as hell and we only need a fraction of the range
#include <boost/random/uniform_smallint.hpp>

using namespace Wt;
using namespace boost::interprocess;
using namespace boost::random;

#ifdef __cplusplus
extern "C" {
#endif
#include "event2/event.h"
#ifdef __cplusplus
} // extern "C"
#endif

class WtKeySetAndGetWidget : public WApplication
{
public:
    WtKeySetAndGetWidget(const WEnvironment& env);
    static message_queue *m_SetValueByKeyRequestFromWtMessageQueues[NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS];
    static int getNumberOfThreadsGuess();
    static void newAndOpenSetValueByKeyMessageQueue(int arrayIndex, const char *messageQueueName);
    static void deleteSetValueByKeyMessageQueue(int arrayIndex);
    static event *m_SetValueByKeyRequestFromWtEvent[NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS];

    void valueSetByKeyCallback(std::string key, std::string value);
    //^should probably friend class in a final impl to keep it private...
private:
    WLineEdit *m_KeyLineEdit;
    WLineEdit *m_ValueLineEdit;
    WContainerWidget *m_Canvas;
    void setValueByKey();

    taus88 m_RandomNumberGenerator;
    uniform_smallint<> m_ZeroThroughNineDistribution;

    static boost::mutex m_MutexArray[NUMBER_OF_MUTEXES_AND_MESSAGE_QUEUES_SHOULD_BE_ROUGHLY_EQUAL_TO_WTS_THREAD_POOL_MAX_THREADS]; //reading this plain array is thread safe :)
};

#endif // WTKEYSETANDGETWIDGET_H

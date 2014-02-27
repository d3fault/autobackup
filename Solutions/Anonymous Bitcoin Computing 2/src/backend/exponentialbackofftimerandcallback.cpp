#include "exponentialbackofftimerandcallback.h"

#include "anonymousbitcoincomputingcouchbasedb.h"

//TODOreq: during our 'shutdown' procedure, we need to wait for all these timeouts to finish (and of course, set up flagging or maybe that part is already done... so that no more timeouts are scheduled). this also applies to get and subscribe, which may or may not be done as well idfk (never thought about it till now). maybe just freeing the timeouts before they time out is sufficient (so long as i still delete underlying request ofc)
AnonymousBitcoinComputingCouchbaseDB *AutoRetryingWithExponentialBackoffCouchbaseGetRequest::m_AnonymousBitcoinComputingCouchbaseDB = NULL;
struct event_base *AutoRetryingWithExponentialBackoffCouchbaseGetRequest::m_EventBase = NULL;

void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setAnonymousBitcoinComputingCouchbaseDB(AnonymousBitcoinComputingCouchbaseDB *anonymousBitcoinComputingCouchbaseDB)
{
    m_AnonymousBitcoinComputingCouchbaseDB = anonymousBitcoinComputingCouchbaseDB;
}
void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setEventBase(event_base *eventBase)
{
    m_EventBase = eventBase;
}
AutoRetryingWithExponentialBackoffCouchbaseGetRequest::AutoRetryingWithExponentialBackoffCouchbaseGetRequest()
{
    m_MyLibEventTimer = evtimer_new(m_EventBase, &AutoRetryingWithExponentialBackoffCouchbaseGetRequest::timeoutCallbackStatic, this);
}
AutoRetryingWithExponentialBackoffCouchbaseGetRequest::~AutoRetryingWithExponentialBackoffCouchbaseGetRequest()
{
    event_free(m_MyLibEventTimer);
}
void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setNewRequestToRetry(GetCouchbaseDocumentByKeyRequest *newRequestToRetry)
{
    m_RequestToRetry = newRequestToRetry;

    //reset timeval
    m_MyBackoffTimeval.tv_sec = 0;
    m_MyBackoffTimeval.tv_usec = 2500; //5ms is how long we wait before our first retry (2.5ms is first doubled), then 10, 20, 40, etc (hmmm that formula seems familiar xD)
}
GetCouchbaseDocumentByKeyRequest *AutoRetryingWithExponentialBackoffCouchbaseGetRequest::requestRetrying()
{
    return m_RequestToRetry;
}
void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::backoffAndRetryAgain()
{
    //TODOreq: verify that libevent isn't messing with the timevals passed to it, which really only means i need to keep another copy..
    if(m_MyBackoffTimeval.tv_sec > 0)
    {
        //once we get to SECONDS, we don't care about microseconds/milliseconds anymore (simplifies the math)
        m_MyBackoffTimeval.tv_sec *= 2;
    }
    else
    {
        m_MyBackoffTimeval.tv_usec *= 2;
        if(m_MyBackoffTimeval.tv_usec > 1000000) //one second, switch to seconds mode above [next time]
        {
            //lol 2014
            m_MyBackoffTimeval.tv_sec = 1;
            m_MyBackoffTimeval.tv_usec = 0; //it'll be off a little, big whoop (the bug that ended the world prematurely)
        }
    }

    event_add(m_MyLibEventTimer, &m_MyBackoffTimeval);
}
void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::timeoutCallbackStatic(int unusedSocket, short events, void *userData)
{
    (void)unusedSocket;
    (void)events;
    static_cast<AutoRetryingWithExponentialBackoffCouchbaseGetRequest*>(userData)->timeoutCallback();
}
void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::timeoutCallback()
{
    m_AnonymousBitcoinComputingCouchbaseDB->scheduleGetRequest(this);
}

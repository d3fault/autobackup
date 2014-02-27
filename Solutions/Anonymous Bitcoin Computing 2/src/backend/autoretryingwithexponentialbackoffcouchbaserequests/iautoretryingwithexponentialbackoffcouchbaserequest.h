#ifndef IAUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASEREQUEST_H
#define IAUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASEREQUEST_H

#include <event2/event.h>

class AnonymousBitcoinComputingCouchbaseDB;

class IAutoRetryingWithExponentialBackoffCouchbaseRequest
{
public:
    static void setAnonymousBitcoinComputingCouchbaseDB(AnonymousBitcoinComputingCouchbaseDB *anonymousBitcoinComputingCouchbaseDB);
    static void setEventBase(struct event_base *eventBase);

    IAutoRetryingWithExponentialBackoffCouchbaseRequest();
    ~IAutoRetryingWithExponentialBackoffCouchbaseRequest();
    void backoffAndRetryAgain();
protected:
    static AnonymousBitcoinComputingCouchbaseDB *m_AnonymousBitcoinComputingCouchbaseDB;
    static struct event_base *m_EventBase;

    void resetTimeval();
    struct event *m_MyLibEventTimer;
    struct timeval m_MyBackoffTimeval;
    static void timeoutCallbackStatic(int unusedSocket, short events, void *userData);
    virtual void timeoutCallback()=0;
};

#endif // IAUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASEREQUEST_H

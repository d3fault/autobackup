#ifndef EXPONENTIALBACKOFFTIMERANDCALLBACK_H
#define EXPONENTIALBACKOFFTIMERANDCALLBACK_H

#include <event2/event.h>

class AnonymousBitcoinComputingCouchbaseDB;
class GetCouchbaseDocumentByKeyRequest;

class AutoRetryingWithExponentialBackoffCouchbaseGetRequest
{
public:
    static void setAnonymousBitcoinComputingCouchbaseDB(AnonymousBitcoinComputingCouchbaseDB *anonymousBitcoinComputingCouchbaseDB);
    static void setEventBase(struct event_base *eventBase);

    AutoRetryingWithExponentialBackoffCouchbaseGetRequest();
    ~AutoRetryingWithExponentialBackoffCouchbaseGetRequest();
    void setNewRequestToRetry(GetCouchbaseDocumentByKeyRequest *newRequestToRetry);
    GetCouchbaseDocumentByKeyRequest *requestRetrying();
    void backoffAndRetryAgain();
private:
    static AnonymousBitcoinComputingCouchbaseDB *m_AnonymousBitcoinComputingCouchbaseDB;
    static struct event_base *m_EventBase;

    struct event *m_MyLibEventTimer;
    struct timeval m_MyBackoffTimeval;
    static void timeoutCallbackStatic(int unusedSocket, short events, void *userData);
    void timeoutCallback();
    GetCouchbaseDocumentByKeyRequest *m_RequestToRetry;
};

#endif // EXPONENTIALBACKOFFTIMERANDCALLBACK_H

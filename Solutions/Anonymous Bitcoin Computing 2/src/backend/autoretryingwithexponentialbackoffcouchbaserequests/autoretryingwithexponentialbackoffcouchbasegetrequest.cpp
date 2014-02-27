#include "autoretryingwithexponentialbackoffcouchbasegetrequest.h"

#include "../anonymousbitcoincomputingcouchbasedb.h" //TODOoptional: could make this modular by having the interface maintain a void* (or even templateify it (but i rarely make templates because i suck at life)), but ehh too lazy for now

void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setNewGetRequestToRetry(GetCouchbaseDocumentByKeyRequest *newRequestToRetry)
{
    m_RequestToRetry = newRequestToRetry;
    resetTimeval();
}
GetCouchbaseDocumentByKeyRequest *AutoRetryingWithExponentialBackoffCouchbaseGetRequest::getRequestRetrying()
{
    return m_RequestToRetry;
}
void AutoRetryingWithExponentialBackoffCouchbaseGetRequest::timeoutCallback()
{
    m_AnonymousBitcoinComputingCouchbaseDB->scheduleGetRequest(this);
}

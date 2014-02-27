#ifndef AUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASEGETREQUEST_H
#define AUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASEGETREQUEST_H

#include "iautoretryingwithexponentialbackoffcouchbaserequest.h"

class GetCouchbaseDocumentByKeyRequest;

class AutoRetryingWithExponentialBackoffCouchbaseGetRequest : public IAutoRetryingWithExponentialBackoffCouchbaseRequest
{
public:
    void setNewGetRequestToRetry(GetCouchbaseDocumentByKeyRequest *newRequestToRetry);
    GetCouchbaseDocumentByKeyRequest *getRequestRetrying();
private:
    void timeoutCallback();
    GetCouchbaseDocumentByKeyRequest *m_RequestToRetry;
};

#endif // AUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASEGETREQUEST_H

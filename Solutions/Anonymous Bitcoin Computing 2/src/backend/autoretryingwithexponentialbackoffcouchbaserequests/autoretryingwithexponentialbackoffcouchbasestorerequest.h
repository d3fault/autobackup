#ifndef AUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASESTOREREQUEST_H
#define AUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASESTOREREQUEST_H

#include "iautoretryingwithexponentialbackoffcouchbaserequest.h"

class StoreCouchbaseDocumentByKeyRequest;

class AutoRetryingWithExponentialBackoffCouchbaseStoreRequest : public IAutoRetryingWithExponentialBackoffCouchbaseRequest
{
public:
    void setNewStoreRequestToRetry(StoreCouchbaseDocumentByKeyRequest *newRequestToRetry);
    StoreCouchbaseDocumentByKeyRequest *storeRequestRetrying();
private:
    void timeoutCallback();
    StoreCouchbaseDocumentByKeyRequest *m_RequestToRetry;
};

#endif // AUTORETRYINGWITHEXPONENTIALBACKOFFCOUCHBASESTOREREQUEST_H

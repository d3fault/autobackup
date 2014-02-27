#include "autoretryingwithexponentialbackoffcouchbasestorerequest.h"

#include "../anonymousbitcoincomputingcouchbasedb.h"

//TODOoptional: could make both request types have the same interface and a pure virtual "processRequest", which then does the appropriate Get/Store stuff (which is basically done in timeoutCallback here). Polymorphism is addicting as fuck, even when it provides little benefit. CAN becomes SHOULD (because of the addiction (or maybe it really is should always, idfk (my instincts tell me yes, hence the addiction))). the respond/respondWithCas methods won't line up though i don't think... but maybe i'm wrong and they just need a tad of refactoring in order to line up...
void AutoRetryingWithExponentialBackoffCouchbaseStoreRequest::setNewStoreRequestToRetry(StoreCouchbaseDocumentByKeyRequest *newRequestToRetry)
{
    m_RequestToRetry = newRequestToRetry;
    resetTimeval();
}
StoreCouchbaseDocumentByKeyRequest *AutoRetryingWithExponentialBackoffCouchbaseStoreRequest::storeRequestRetrying()
{
    return m_RequestToRetry;
}
void AutoRetryingWithExponentialBackoffCouchbaseStoreRequest::timeoutCallback()
{
    m_AnonymousBitcoinComputingCouchbaseDB->scheduleStoreRequest(this);
}

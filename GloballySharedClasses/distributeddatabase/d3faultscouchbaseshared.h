#ifndef D3FAULTS_COUCHBASE_SHARED_H
#define D3FAULTS_COUCHBASE_SHARED_H

#define D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR "_"

#include <libcouchbase/couchbase.h>

class D3faultsCouchbaseShared
{
public:
    static inline bool lcbErrorTypeIsEligibleForExponentialBackoff(lcb_error_t error)
    {
        return (error == LCB_EBUSY || error == LCB_ETMPFAIL || error == LCB_CLIENT_ETMPFAIL);
    }
};

#endif // D3FAULTS_COUCHBASE_SHARED_H

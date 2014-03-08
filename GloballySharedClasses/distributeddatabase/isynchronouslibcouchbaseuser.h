#ifndef ISYNCHRONOUSLIBCOUCHBASEUSER_H
#define ISYNCHRONOUSLIBCOUCHBASEUSER_H

#include <libcouchbase/couchbase.h>

#include <string>
using namespace std;

class ISynchronousLibCouchbaseUser
{
public:
    ISynchronousLibCouchbaseUser();
protected:
    lcb_t m_Couchbase;
    bool m_Connected;
    lcb_error_t m_LastOpStatus;
    std::string m_LastDocGetted;
    lcb_cas_t m_LastGetCas;

    //be sure to call base implementation if overriding xD, though really i can't even imagine any cases where i would override to begin with...
    virtual void errorCallback(lcb_error_t error, const char *errinfo);
    virtual void configurationCallback(lcb_configuration_t config);
    virtual void getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    virtual void storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);

    bool connectToCouchbase();
    bool couchbaseGetRequest(const string &key, const string &optionalDescriptionOfGet = "a get request");
    bool couchbaseGetRequestWithExponentialBackoff(const string &key, const string &optionalDescriptionOfGet = "a get request");
    bool couchbaseGetRequestWithExponentialBackoffRequiringSuccess(const string &key, const string &optionalDescriptionOfGet = "a get request");
    bool couchbaseStoreRequest(const string &key, const string &value, lcb_storage_t storageType = LCB_ADD, lcb_cas_t cas = 0, const string &optionalDescriptionOfStore = "a store request");
    bool couchbaseStoreRequestWithExponentialBackoff(const string &key, const string &value, lcb_storage_t storageType = LCB_ADD, lcb_cas_t cas = 0, const string &optionalDescriptionOfStore = "a store request");
    bool couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(const string &key, const string &value, lcb_storage_t storageType = LCB_ADD, lcb_cas_t cas = 0, const string &optionalDescriptionOfStore = "a store request");

    virtual void errorOutput(const std::string &errorString);
    const string lastErrorString();
private:
    void resetExponentialSleepTimers();
    void exponentialSleep();
    __useconds_t m_CurrentExponentialBackoffMicrosecondsAmount;
    unsigned int m_CurrentExponentialBackoffSecondsAmount;

    //zzz
    static void errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo);
    static void configurationCallbackStatic(lcb_t instance, lcb_configuration_t config);
    static void getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    static void storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
};

#endif // ISYNCHRONOUSLIBCOUCHBASEUSER_H

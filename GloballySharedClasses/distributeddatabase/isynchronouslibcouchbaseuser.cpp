#include "isynchronouslibcouchbaseuser.h"

#include <unistd.h>

#include "d3faultscouchbaseshared.h"

//"Client" fits better than "User" in the class name
ISynchronousLibCouchbaseUser::ISynchronousLibCouchbaseUser()
    : m_CouchbaseCreated(false)
    , m_Connected(false) //i think my initializer/page-adder forgot to set this to false, so this refactor caught a potentially large bug
{ }
ISynchronousLibCouchbaseUser::~ISynchronousLibCouchbaseUser()
{
    destroyCouchbaseIfCreated();
}
void ISynchronousLibCouchbaseUser::errorCallback(lcb_error_t error, const char *errinfo)
{
    errorOutput("Got an error in our couchbase error callback: " + string(lcb_strerror(m_Couchbase, error)) + " / " + string(errinfo));
}
void ISynchronousLibCouchbaseUser::configurationCallback(lcb_configuration_t config)
{
    if(config == LCB_CONFIGURATION_NEW)
    {
        m_Connected = true;
    }
}
void ISynchronousLibCouchbaseUser::getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    (void)cookie;
    m_LastOpStatus = error;
    if(error == LCB_SUCCESS)
    {
        //Note: these members are undefined(uninitialized or old values) if the error wasn't success, need to make sure i don't try to access them in such cases
        m_LastDocGetted = std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
        m_LastGetCas = resp->v.v0.cas;
    }
}
void ISynchronousLibCouchbaseUser::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)cookie;
    (void)operation;
    (void)resp;
    m_LastOpStatus = error;
    if(error == LCB_SUCCESS)
    {
        m_LastSetCas = resp->v.v0.cas; //I added this way later after the first version of this class was coded. Was tempted to just use m_LastGetCas (psbly changing the variable name), but there MIGHT be source compatibility conflicts (there definitely would be if I changed the variable name) if for example I did a get, a store, and then did something (another store most likely) relying on m_LastGetCas -- if they shared variables, the second store would now be using a different cas and would fuck shit up
    }
}
bool ISynchronousLibCouchbaseUser::connectToCouchbase()
{
    struct lcb_create_st createOptions;
    memset(&createOptions, 0, sizeof(createOptions));
    //createOptions.v.v0.host = "192.168.56.10:8091;192.168.57.10:8091;192.168.58.10:8091"; //TODOreq: supply hosts/username/password through args
    createOptions.v.v0.host = "192.168.56.10:8091";
    lcb_error_t error;
    if((error = lcb_create(&m_Couchbase, &createOptions)) != LCB_SUCCESS)
    {
        errorOutput("Failed to create a libcouchbase instance: " + string(lcb_strerror(NULL, error)));
        return false;
    }
    m_CouchbaseCreated = true;

    lcb_set_cookie(m_Couchbase, this);

    //callbacks
    lcb_set_error_callback(m_Couchbase, ISynchronousLibCouchbaseUser::errorCallbackStatic);
    lcb_set_configuration_callback(m_Couchbase, ISynchronousLibCouchbaseUser::configurationCallbackStatic);
    lcb_set_get_callback(m_Couchbase, ISynchronousLibCouchbaseUser::getCallbackStatic);
    lcb_set_store_callback(m_Couchbase, ISynchronousLibCouchbaseUser::storeCallbackStatic);

    if((error = lcb_connect(m_Couchbase)) != LCB_SUCCESS)
    {
        errorOutput("Failed to start connecting libcouchbase instance: " + string(lcb_strerror(m_Couchbase, error)));
        destroyCouchbaseIfCreated();
        return false;
    }
    if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
    {
        errorOutput("Failed to lcb_wait after lcb_connect:" + string(lcb_strerror(m_Couchbase, error)));
        destroyCouchbaseIfCreated();
        return false;
    }
    if(!m_Connected)
    {
        errorOutput("Failed to connect libcouchbase instance: " + string(lcb_strerror(m_Couchbase, error)));
        destroyCouchbaseIfCreated();
        return false;
    }
    return true;
}
bool ISynchronousLibCouchbaseUser::couchbaseGetRequest(const string &key, const string &optionalDescriptionOfGet)
{
    lcb_error_t error;
    {
        lcb_get_cmd_t cmd;
        const lcb_get_cmd_t *cmds[1];
        cmds[0] = &cmd;
        memset(&cmd, 0, sizeof(cmd));
        cmd.v.v0.key = key.c_str();
        cmd.v.v0.nkey = key.length();
        error = lcb_get(m_Couchbase, NULL, 1, cmds);
        if(error  != LCB_SUCCESS)
        {
            errorOutput("Failed to setup get request for " + optionalDescriptionOfGet + ": " + string(lcb_strerror(m_Couchbase, error)));
            destroyCouchbaseIfCreated();
            return false;
        }
    }
    if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
    {
        errorOutput("Failed to lcb_wait after lcb_get for " + optionalDescriptionOfGet + ": " + string(lcb_strerror(m_Couchbase, error)));
        destroyCouchbaseIfCreated();
        return false;
    }
    return true;
}
//TODOoptional: get with exponential backoff the default? maybe WITHOUT is what can be explicitly called?
bool ISynchronousLibCouchbaseUser::couchbaseGetRequestWithExponentialBackoff(const string &key, const string &optionalDescriptionOfGet)
{
    resetExponentialSleepTimers();
    if(!couchbaseGetRequest(key, optionalDescriptionOfGet))
    {
        return false;
    }
    while(D3faultsCouchbaseShared::lcbErrorTypeIsEligibleForExponentialBackoff(m_LastOpStatus))
    {
        exponentialSleep();
        if(!couchbaseGetRequest(key, optionalDescriptionOfGet))
        {
            return false;
        }
    }
    return true;
}
//better named "requring lcb op success"
bool ISynchronousLibCouchbaseUser::couchbaseGetRequestWithExponentialBackoffRequiringSuccess(const string &key, const string &optionalDescriptionOfGet)
{
    if(!couchbaseGetRequestWithExponentialBackoff(key, optionalDescriptionOfGet))
    {
        return false;
    }
    if(m_LastOpStatus != LCB_SUCCESS)
    {
        errorOutput("Failed to get " + optionalDescriptionOfGet + ": " + string(lcb_strerror(m_Couchbase, m_LastOpStatus)));
        destroyCouchbaseIfCreated();
        return false;
    }
    return true;
}
bool ISynchronousLibCouchbaseUser::couchbaseStoreRequest(const string &key, const string &value, lcb_storage_t storageType, lcb_cas_t cas, const string &optionalDescriptionOfStore)
{
    lcb_error_t error;
    {
        lcb_store_cmd_t cmd;
        const lcb_store_cmd_t *cmds[1];
        cmds[0] = &cmd;
        memset(&cmd, 0, sizeof(cmd));
        cmd.v.v0.key = key.c_str();
        cmd.v.v0.nkey = key.length();
        cmd.v.v0.bytes = value.c_str();
        cmd.v.v0.nbytes = value.length();
        cmd.v.v0.operation = storageType;
        cmd.v.v0.cas = cas;
        error = lcb_store(m_Couchbase, NULL, 1, cmds);
        if(error != LCB_SUCCESS)
        {
            errorOutput("Failed to set up add request for " + optionalDescriptionOfStore + string(lcb_strerror(m_Couchbase, error)));
            destroyCouchbaseIfCreated();
            return false;
        }
    }
    if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
    {
        errorOutput("Failed to lcb_wait after set request for " + optionalDescriptionOfStore + string(lcb_strerror(m_Couchbase, error)));
        destroyCouchbaseIfCreated();
        return false;
    }
    return true;
}
bool ISynchronousLibCouchbaseUser::couchbaseStoreRequestWithExponentialBackoff(const string &key, const string &value, lcb_storage_t storageType, lcb_cas_t cas, const string &optionalDescriptionOfStore)
{
    resetExponentialSleepTimers();
    if(!couchbaseStoreRequest(key, value, storageType, cas, optionalDescriptionOfStore))
    {
        return false;
    }
    while(D3faultsCouchbaseShared::lcbErrorTypeIsEligibleForExponentialBackoff(m_LastOpStatus))
    {
        exponentialSleep();
        if(!couchbaseStoreRequest(key, value, storageType, cas, optionalDescriptionOfStore))
        {
            return false;
        }
    }
    return true;
}
//better named "requring lcb op success"
bool ISynchronousLibCouchbaseUser::couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(const string &key, const string &value, lcb_storage_t storageType, lcb_cas_t cas, const string &optionalDescriptionOfStore)
{
    if(!couchbaseStoreRequestWithExponentialBackoff(key, value, storageType, cas, optionalDescriptionOfStore))
    {
        return false;
    }
    if(m_LastOpStatus != LCB_SUCCESS)
    {
        errorOutput("Failed to store " + optionalDescriptionOfStore + ": " + string(lcb_strerror(m_Couchbase, m_LastOpStatus)));
        destroyCouchbaseIfCreated();
        return false;
    }
    return true;
}
void ISynchronousLibCouchbaseUser::errorOutput(const string &errorString)
{
    (void)errorString;
    //default impl does nothing
}
const string ISynchronousLibCouchbaseUser::lastErrorString()
{
    return string(lcb_strerror(m_Couchbase, m_LastOpStatus));
}
void ISynchronousLibCouchbaseUser::resetExponentialSleepTimers()
{
    m_CurrentExponentialBackoffMicrosecondsAmount = 2500; //5ms is first sleep amount (doubled before first use)
    m_CurrentExponentialBackoffSecondsAmount = 0;
}
void ISynchronousLibCouchbaseUser::exponentialSleep()
{
    if(m_CurrentExponentialBackoffSecondsAmount > 0)
    {
        m_CurrentExponentialBackoffSecondsAmount *= 2;
        sleep(m_CurrentExponentialBackoffSecondsAmount);
    }
    else
    {
        m_CurrentExponentialBackoffMicrosecondsAmount *= 2;
        if(m_CurrentExponentialBackoffMicrosecondsAmount > 1000000)
        {
            m_CurrentExponentialBackoffSecondsAmount = 1;
            sleep(m_CurrentExponentialBackoffSecondsAmount);
        }
        else
        {
            usleep(m_CurrentExponentialBackoffMicrosecondsAmount);
        }
    }
}
//follows is the c++ equivalent of a wrapper to c. isn't strictly necessary if you aren't using OO (but you should)
void ISynchronousLibCouchbaseUser::errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo)
{
    const_cast<ISynchronousLibCouchbaseUser*>(static_cast<const ISynchronousLibCouchbaseUser*>(lcb_get_cookie(instance)))->errorCallback(error, errinfo);
}
void ISynchronousLibCouchbaseUser::configurationCallbackStatic(lcb_t instance, lcb_configuration_t config)
{
    const_cast<ISynchronousLibCouchbaseUser*>(static_cast<const ISynchronousLibCouchbaseUser*>(lcb_get_cookie(instance)))->configurationCallback(config);
}
void ISynchronousLibCouchbaseUser::getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    const_cast<ISynchronousLibCouchbaseUser*>(static_cast<const ISynchronousLibCouchbaseUser*>(lcb_get_cookie(instance)))->getCallback(cookie, error, resp);
}
void ISynchronousLibCouchbaseUser::storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    const_cast<ISynchronousLibCouchbaseUser*>(static_cast<const ISynchronousLibCouchbaseUser*>(lcb_get_cookie(instance)))->storeCallback(cookie, operation, error, resp);
}
void ISynchronousLibCouchbaseUser::destroyCouchbaseIfCreated()
{
    if(m_CouchbaseCreated)
    {
        lcb_destroy(m_Couchbase);
        m_CouchbaseCreated = false;
    }
}

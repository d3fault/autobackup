#ifndef ABC2PESSIMISTICSTATEMONITORANDRECOVERER_H
#define ABC2PESSIMISTICSTATEMONITORANDRECOVERER_H

#include <libcouchbase/couchbase.h>

#include <string>

//TODOreq: exponential backoffs and/or just plain old in the m_LastOpStatus error checking in this macro here, idfk. also for all the other checks of m_LastOpStatus that follow in the core while(true) loop...
#define DO_COUCHBASE_GET_CAMPAIGN_DOC() \
{ \
    lcb_get_cmd_t cmd; \
    const lcb_get_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = campaignDocKey.c_str(); \
    cmd.v.v0.nkey = campaignDocKey.length(); \
    lcb_error_t error = lcb_get(m_Couchbase, NULL, 1, cmds); \
    if(error  != LCB_SUCCESS) \
    { \
        cerr << "Failed to setup get request for campaign doc: " << lcb_strerror(m_Couchbase, error) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
} \
if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS) \
{ \
    cerr << "Failed to lcb_wait after get campaign doc': " << lcb_strerror(m_Couchbase, error) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
} \
if(m_LastOpStatus != LCB_SUCCESS) \
{ \
    cerr << "Failed to get campaign doc: " << lcb_strerror(m_Couchbase, error) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
}

#define DO_COUCHBASE_GET_TRANSACTION_DOC() \
{ \
    lcb_get_cmd_t cmd; \
    const lcb_get_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = transactionDocKey.c_str(); \
    cmd.v.v0.nkey = transactionDocKey.length(); \
    error = lcb_get(m_Couchbase, NULL, 1, cmds); \
    if(error  != LCB_SUCCESS) \
    { \
        cerr << "Failed to setup get request for tx that might exist (" + transactionDocKey + "): " << lcb_strerror(m_Couchbase, error) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
} \
if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS) \
{ \
    cerr << "Failed to lcb_wait after get request for tx that might exist (" + transactionDocKey + "): " << lcb_strerror(m_Couchbase, error) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
}

#define DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_USER_ACCOUNT_DEBIT_AND_UNLOCK() \
{ \
    lcb_store_cmd_t cmd; \
    const lcb_store_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = userAccountKey.c_str(); \
    cmd.v.v0.nkey = userAccountKey.length(); \
    double buyerBalance = boost::lexical_cast<double>(pt6.get<std::string>("balance")); \
    double purchasePrice = boost::lexical_cast<double>(purchasePriceString); \
    buyerBalance -= purchasePrice; \
    pt6.put("balance", boost::lexical_cast<std::string>(buyerBalance)); \
    std::ostringstream userAccountDebittedAndUnlockedJsonBuffer; \
    write_json(userAccountDebittedAndUnlockedJsonBuffer, pt6, false); \
    std::string userAccountDebittedAndUnlockedJson = userAccountDebittedAndUnlockedJsonBuffer.str(); \
    cmd.v.v0.bytes = userAccountDebittedAndUnlockedJson.c_str(); \
    cmd.v.v0.nbytes = userAccountDebittedAndUnlockedJson.length(); \
    cmd.v.v0.operation = LCB_SET; \
    cmd.v.v0.cas = userAccountCas; \
    error = lcb_store(m_Couchbase, NULL, 1, cmds); \
    if(error != LCB_SUCCESS) \
    { \
        cerr << "Failed to set up set request for user account unlocking+debitting: " << lcb_strerror(m_Couchbase, error) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
} \
if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS) \
{ \
    cerr << "Failed to lcb_wait after set request for user account unlocking+debitting: " << lcb_strerror(m_Couchbase, error) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
} \
if(m_LastOpStatus != LCB_SUCCESS) \
{ \
    if(m_LastOpStatus != LCB_KEY_EEXISTS) \
    { \
        cerr << "Failed to cas-swap set user account unlocking+debitting: " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
}

#define DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_CAMPAIGN_DOC_UPDATE() \
{ \
    lcb_store_cmd_t cmd; \
    const lcb_store_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = campaignDocKey.c_str(); \
    cmd.v.v0.nkey = campaignDocKey.length(); \
    ptree pt8; \
    pt8.put("slotIndex", slotIndexForSlotThatShouldntExistButMightString); \
    pt8.put("purchaseTimestamp", pt4.get<std::string>("purchaseTimestamp")); \
    pt8.put("startTimestamp", pt4.get<std::string>("startTimestamp")); \
    pt8.put("purchasePrice", pt4.get<std::string>("purchasePrice")); \
    std::ostringstream updatedCampaignDocJsonBuffer; \
    write_json(updatedCampaignDocJsonBuffer, pt8, false); \
    std::string updatedCampaignDocJson = updatedCampaignDocJsonBuffer.str(); \
    cmd.v.v0.bytes = updatedCampaignDocJson.c_str(); \
    cmd.v.v0.nbytes = updatedCampaignDocJson.length(); \
    cmd.v.v0.operation = LCB_SET; \
    cmd.v.v0.cas = campaignDocToUpdateCAS; \
    error = lcb_store(m_Couchbase, NULL, 1, cmds); \
    if(error != LCB_SUCCESS) \
    { \
        cerr << "Failed to set up set request for updating campaign doc: " << lcb_strerror(m_Couchbase, error) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
} \
if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS) \
{ \
    cerr << "Failed to lcb_wait after set request for updating campaign doc: " << lcb_strerror(m_Couchbase, error) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
} \
if(m_LastOpStatus != LCB_SUCCESS) \
{ \
    if(m_LastOpStatus != LCB_KEY_EEXISTS) \
    { \
        cerr << "Failed to cas-swap update campaign doc: " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
}

class Abc2PessimisticStateMonitorAndRecoverer
{
public:
    Abc2PessimisticStateMonitorAndRecoverer();
    int startPessimisticallyMonitoringAndRecovereringStateUntilToldToStop();
private:
    lcb_t m_Couchbase;
    lcb_error_t m_LastOpStatus;
    bool m_Connected;
    std::string m_LastDocGetted;
    lcb_cas_t m_LastGetCas;

    static void errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo);
    void errorCallback(lcb_error_t error, const char *errinfo);
    static void configurationCallbackStatic(lcb_t instance, lcb_configuration_t config);
    void configurationCallback(lcb_configuration_t config);
    static void getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    void getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    static void storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
    void storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
};

#endif // ABC2PESSIMISTICSTATEMONITORANDRECOVERER_H

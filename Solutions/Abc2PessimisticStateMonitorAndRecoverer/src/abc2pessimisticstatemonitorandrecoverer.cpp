#include "abc2pessimisticstatemonitorandrecoverer.h"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "abc2couchbaseandjsonkeydefines.h"

using namespace std;
using namespace boost::property_tree;

//TODOreq: exponential backoffs and/or just plain old in the m_LastOpStatus error checking in this macro here, idfk. also for all the other checks of m_LastOpStatus that follow in the core while(true) loop...
#define ABCP_DO_RESET_EXPONENTIAL_SLEEP_TIMERS \
m_CurrentExponentialBackoffMicrosecondsAmount = 2500; \
m_CurrentExponentialBackoffSecondsAmount = 0; //5ms is first sleep amount

#define ABCP_DO_ONE_EXPONENTIAL_SLEEP \
if(m_CurrentExponentialBackoffSecondsAmount > 0) \
{ \
    m_CurrentExponentialBackoffSecondsAmount *= 2; \
    sleep(m_CurrentExponentialBackoffSecondsAmount); \
} \
else \
{ \
    m_CurrentExponentialBackoffMicrosecondsAmount *= 2; \
    if(m_CurrentExponentialBackoffMicrosecondsAmount > 1000000) \
    { \
        m_CurrentExponentialBackoffSecondsAmount = 1; \
        sleep(m_CurrentExponentialBackoffSecondsAmount); \
    } \
    else \
    { \
        usleep(m_CurrentExponentialBackoffMicrosecondsAmount); \
    } \
}

#define ABCP_DO_ONE_COUCHBASE_GET_REQUEST(keyStringVar, descriptionOfRequestStringLiteral) \
{ \
    lcb_get_cmd_t cmd; \
    const lcb_get_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = keyStringVar.c_str(); \
    cmd.v.v0.nkey = keyStringVar.length(); \
    lcb_error_t error = lcb_get(m_Couchbase, NULL, 1, cmds); \
    if(error  != LCB_SUCCESS) \
    { \
        cerr << "Failed to setup get request for " << descriptionOfRequestStringLiteral << ": " << lcb_strerror(m_Couchbase, error) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
} \
if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS) \
{ \
    cerr << "Failed to lcb_wait after lcb_get for " << descriptionOfRequestStringLiteral << ": " << lcb_strerror(m_Couchbase, error) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
}

#define ABCP_DO_ONE_COUCHBASE_STORE_REQUEST(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
{ \
    lcb_store_cmd_t cmd; \
    const lcb_store_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = keyStringVar.c_str(); \
    cmd.v.v0.nkey = keyStringVar.length(); \
    cmd.v.v0.bytes = valueStringVar.c_str(); \
    cmd.v.v0.nbytes = valueStringVar.length(); \
    cmd.v.v0.operation = lcbOp; \
    cmd.v.v0.cas = tehCas; \
    error = lcb_store(m_Couchbase, NULL, 1, cmds); \
    if(error != LCB_SUCCESS) \
    { \
        cerr << "Failed to set up add request for " << descriptionOfRequestStringLiteral << lcb_strerror(m_Couchbase, error) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
} \
if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS) \
{ \
    cerr << "Failed to lcb_wait after set request for " << descriptionOfRequestStringLiteral << lcb_strerror(m_Couchbase, error) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
}

#define ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF(keyStringVar, descriptionOfRequestStringLiteral) \
ABCP_DO_RESET_EXPONENTIAL_SLEEP_TIMERS \
ABCP_DO_ONE_COUCHBASE_GET_REQUEST(keyStringVar, descriptionOfRequestStringLiteral) \
while(ABC_COUCHBASE_LCB_ERROR_TYPE_IS_ELIGIBLE_FOR_EXPONENTIAL_BACKOFF(m_LastOpStatus)) \
{ \
    ABCP_DO_ONE_EXPONENTIAL_SLEEP \
    ABCP_DO_ONE_COUCHBASE_GET_REQUEST(keyStringVar, descriptionOfRequestStringLiteral) \
}

#define ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
ABCP_DO_RESET_EXPONENTIAL_SLEEP_TIMERS \
ABCP_DO_ONE_COUCHBASE_STORE_REQUEST(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
while(ABC_COUCHBASE_LCB_ERROR_TYPE_IS_ELIGIBLE_FOR_EXPONENTIAL_BACKOFF(m_LastOpStatus)) \
{ \
    ABCP_DO_ONE_EXPONENTIAL_SLEEP \
    ABCP_DO_ONE_COUCHBASE_STORE_REQUEST(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
}

#define ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(keyStringVar, descriptionOfRequestStringLiteral) \
ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF(keyStringVar, descriptionOfRequestStringLiteral) \
if(m_LastOpStatus != LCB_SUCCESS) \
{ \
    cerr << "Failed to get " << descriptionOfRequestStringLiteral << ": " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl; \
    lcb_destroy(m_Couchbase); \
    return 1; \
}

#define DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_USER_ACCOUNT_DEBIT_AND_UNLOCK \
double buyerBalance = boost::lexical_cast<double>(pt6.get<std::string>(JSON_USER_ACCOUNT_BALANCE)); \
double purchasePrice = boost::lexical_cast<double>(purchasePriceString); \
buyerBalance -= purchasePrice; \
pt6.erase(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL); \
pt6.erase(JSON_USER_ACCOUNT_SLOT_TO_ATTEMPT_TO_FILL_IT_WITH); \
pt6.put(JSON_USER_ACCOUNT_BALANCE, boost::lexical_cast<std::string>(buyerBalance)); \
std::ostringstream userAccountDebittedAndUnlockedJsonBuffer; \
write_json(userAccountDebittedAndUnlockedJsonBuffer, pt6, false); \
std::string userAccountDebittedAndUnlockedJson = userAccountDebittedAndUnlockedJsonBuffer.str(); \
ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF(userAccountKeyString, userAccountDebittedAndUnlockedJson, LCB_SET, userAccountCas, "cas-swap user account unlock+debitting") \
if(m_LastOpStatus != LCB_SUCCESS) \
{ \
    if(m_LastOpStatus != LCB_KEY_EEXISTS) \
    { \
        cerr << "Failed to cas-swap set user account unlocking+debitting: " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
    continue; \
}

#define DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_CAMPAIGN_DOC_UPDATE \
ptree pt8; \
pt8.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_INDEX, slotIndexForSlotThatShouldntExistButMightString); \
pt8.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_TIMESTAMP, pt4.get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_TIMESTAMP)); \
pt8.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_START_TIMESTAMP, pt4.get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_START_TIMESTAMP)); \
pt8.put(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_PRICE, pt4.get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_PRICE)); \
pt2.put_child(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED, pt8); \
std::ostringstream updatedCampaignDocJsonBuffer; \
write_json(updatedCampaignDocJsonBuffer, pt2, false); \
std::string updatedCampaignDocJson = updatedCampaignDocJsonBuffer.str(); \
ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF(campaignDocKey, updatedCampaignDocJson, LCB_SET, campaignDocToUpdateCAS, "cas-swap updating campaign doc") \
if(m_LastOpStatus != LCB_SUCCESS) \
{ \
    if(m_LastOpStatus != LCB_KEY_EEXISTS) \
    { \
        cerr << "Failed to cas-swap update campaign doc: " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl; \
        lcb_destroy(m_Couchbase); \
        return 1; \
    } \
    continue; \
}

//monitor-er? ^^ the things that keep me up at night...
Abc2PessimisticStateMonitorAndRecoverer::Abc2PessimisticStateMonitorAndRecoverer()
    : m_Connected(false)
{ }

//TODOreq: If the driver's durability polling (replication) takes longer than one second, we'll get a lot of false positives here. Additionally, we should do a durability poll of our own for the "slot that just appeared" before we do the "100ms extra time" thing. Lastly, we should do durability polling for all of our recovery storings.
int Abc2PessimisticStateMonitorAndRecoverer::startPessimisticallyMonitoringAndRecovereringStateUntilToldToStop()
{
    struct lcb_create_st createOptions;
    memset(&createOptions, 0, sizeof(createOptions));
    createOptions.v.v0.host = "192.168.56.10:8091"; //TODOreq: supply lots of hosts, either separated by semicolon or comma, I forget..
    lcb_error_t error;
    if((error = lcb_create(&m_Couchbase, &createOptions)) != LCB_SUCCESS)
    {
        cerr << "Failed to create a libcouchbase instance: " << lcb_strerror(NULL, error) << endl;
        return 1;
    }

    lcb_set_cookie(m_Couchbase, this);

    //callbacks
    lcb_set_error_callback(m_Couchbase, Abc2PessimisticStateMonitorAndRecoverer::errorCallbackStatic);
    lcb_set_configuration_callback(m_Couchbase, Abc2PessimisticStateMonitorAndRecoverer::configurationCallbackStatic);
    lcb_set_get_callback(m_Couchbase, Abc2PessimisticStateMonitorAndRecoverer::getCallbackStatic);
    lcb_set_store_callback(m_Couchbase, Abc2PessimisticStateMonitorAndRecoverer::storeCallbackStatic);

    if((error = lcb_connect(m_Couchbase)) != LCB_SUCCESS)
    {
        cerr << "Failed to start connecting libcouchbase instance: " << lcb_strerror(m_Couchbase, error) << endl;
        lcb_destroy(m_Couchbase);
        return 1;
    }
    if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
    {
        cerr << "Failed to lcb_wait after lcb_connect:" << lcb_strerror(m_Couchbase, error) << endl;
        lcb_destroy(m_Couchbase);
        return 1;
    }
    if(!m_Connected)
    {
        cerr << "Failed to connect libcouchbase instance: " << lcb_strerror(m_Couchbase, error) << endl;
        lcb_destroy(m_Couchbase);
        return 1;
    }

    //connected

    boost::random::random_device randomNumberGenerator;
    boost::random::uniform_int_distribution<> randomNumberRange(0, 9);
    while(true)
    {
        //TODOreq: portable way to break out of this, SIGTERM etc
        sleep(1); //fancy boost timer libs? libevent timers? nahh

        if(randomNumberRange(randomNumberGenerator) == 0)
        {
            //selected ourself to do audit (recovery probably not necessary)

            //0 - get campaign doc (note these numbers do NOT match the 'perfected.the.design.here.is.the.flow.aka.pseudocode.txt')
            std::string campaignDocKey = adSpaceCampaignKey("d3fault", "0");
            ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(campaignDocKey, "campaign doc for initial audit")

            //1 - analyze campaign doc, so we know which slot+1 to pessimistically check the existence of
            ptree pt;
            std::istringstream is(m_LastDocGetted);
            read_json(is, pt);
            boost::optional<ptree&> lastSlotFilledAkaPurchased = pt.get_child_optional(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED);
            int slotIndexForSlotThatShouldntExistButMight = 0;
            std::string slotIndexForSlotThatShouldntExistButMightString = "0";
            if(lastSlotFilledAkaPurchased.is_initialized())
            {
                //n + 1
                slotIndexForSlotThatShouldntExistButMight = boost::lexical_cast<int>(lastSlotFilledAkaPurchased.get().get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_INDEX)) + 1;
                slotIndexForSlotThatShouldntExistButMightString = boost::lexical_cast<std::string>(slotIndexForSlotThatShouldntExistButMight);
            }
            //else, no purchases so watch index 0

            //2 - check (via get) for existence of doc that shouldn't exist (but very well might because of a not-so-rare race condition)

            std::string slotThatShouldntExistAkaBeFilledButMightKey = adSpaceCampaignSlotKey("d3fault", "0", slotIndexForSlotThatShouldntExistButMightString);

            ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF(slotThatShouldntExistAkaBeFilledButMightKey, "slot that shouldn't exist (" + slotThatShouldntExistAkaBeFilledButMightKey + ")")

            //3 - we WANT to see LCB_KEY_ENOENT (key doesn't exist), otherwise we need to do recovery on campaign doc/etc
            if(m_LastOpStatus != LCB_KEY_ENOENT)
            {
                //we didn't get the error we wanted, so let's make sure it wasn't some other db error
                if(m_LastOpStatus != LCB_SUCCESS)
                {
                    cerr << "Got an error other than LCB_KEY_ENOENT (not exist) while checking for the existence of the slot that should't exist (adSpaceSlotsd3fault0Slot" + slotIndexForSlotThatShouldntExistButMightString + "): " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl;
                }

                //4 - the slot/key that shouldn't exist, does. we are NOW IN RECOVERY MODE
                ptree pt4;
                std::istringstream is4(m_LastDocGetted);//save this for later: getting username from it when transaction doc doesn't exist, also getting purchase price from it when user account is locked pointing the slot of interest
                read_json(is4, pt4);

                //5a - TODOreq: durability poll on that slot so that we aren't 'ahead of' the driver (because he's doing one too), even after our 100ms waiting in 5b
                //TODOreq

                //5b - wait another 100ms for the state to correct itself (race condition, the driver could still be functioning fine)
                usleep(100000);

                //6 - we've waited 100ms for driver to finish, so now we check campaign doc again to see if driver finished it
                ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(campaignDocKey, "campaign doc after 100ms wait")

                //7 - analyze campaign doc AGAIN
                ptree pt2;
                std::istringstream is2(m_LastDocGetted);
                read_json(is2, pt2);
                boost::optional<ptree&> lastSlotFilledAkaPurchased2 = pt2.get_child_optional(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED);
                bool campaignDocWasFixedIn100msExtraTimeWeGaveIt = false;
                if(lastSlotFilledAkaPurchased2.is_initialized())
                {
                    //if(lastSlotFilledAkaPurchased2.get().get<std::string>("slotIndex") == nextSlotIndexString) //actually, it can be greater than or equal to (incredibly rare race condition, but hey...)
                    int slotIndexInCampaignThisTimeAround = boost::lexical_cast<int>(lastSlotFilledAkaPurchased2.get().get<std::string>(JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_INDEX));
                    if(slotIndexInCampaignThisTimeAround >= slotIndexForSlotThatShouldntExistButMight)
                    {
                        //driver (or neighbor recoverer) updated it -- very common race condition, recovery mode complete
                        campaignDocWasFixedIn100msExtraTimeWeGaveIt = true;
                    }
                }
                //else: STILL no last purchase in campaign doc means we need to do recovery for the [0th] slot we just discovered exists

                if(!campaignDocWasFixedIn100msExtraTimeWeGaveIt)
                {
                    //8 - ok shit just got real because it still isn't up to date, time to try all of the post-slot-fill tasks in order (in order) to fix the state
                    lcb_cas_t campaignDocToUpdateCAS = m_LastGetCas; //save this for later...

                    //9 - check to see if transaction doc exists
                    std::string transactionDocKey = transactionKey("d3fault", "0", slotIndexForSlotThatShouldntExistButMightString);
                    ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF(transactionDocKey, "tx that might exist (" + transactionDocKey + "): ")

                    //10 - transaction 'get' finished, so depending on whether or not it exists we take a certain recovery path (but just use a bool flag since they're mostly the same)
                    bool transactionDocExists = false;
                    if(m_LastOpStatus != LCB_KEY_ENOENT)
                    {
                        //might be some other db error
                        if(m_LastOpStatus != LCB_SUCCESS)
                        {
                            cerr << "Got an error other than LCB_KEY_ENOENT (not exist) while checking for the existence of the transaction that might exist (" + transactionDocKey + "): " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl;
                            lcb_destroy(m_Couchbase);
                            return 1;
                        }
                        //transaction doc exists
                        transactionDocExists = true;
                    }
                    //else, transaction doc does not exist

                    //11a - get the username so we can get the user account of the buyer and do recovery tasks if needed
                    std::string usernameOfBuyer = "";
                    if(transactionDocExists)
                    {
                        //we can just pull the username from the transaction...
                        ptree pt3;
                        std::istringstream is3(m_LastDocGetted);
                        read_json(is3, pt3);
                        usernameOfBuyer = pt3.get<std::string>(JSON_TRANSACTION_BUYER);
                    }
                    else
                    {
                        //we have to look up the slot filler we're doing recovery for in order to get the username of the buyer
                        std::string slotFilledWithAkaSlotFillerKey = pt4.get<std::string>(JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH);

                        ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(slotFilledWithAkaSlotFillerKey, "slot filler to retrieve username of buyer from it: (" + slotFilledWithAkaSlotFillerKey + ")")

                        ptree pt5;
                        std::istringstream is5(m_LastDocGetted);
                        read_json(is5, pt5);
                        usernameOfBuyer = pt5.get<std::string>(JSON_SLOT_FILLER_USERNAME);
                    }

                    //11b - get the user account of the buyer and do recovery tasks if needed
                    std::string userAccountKeyString = userAccountKey(usernameOfBuyer);

                    ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(userAccountKeyString, "user account doc (" + userAccountKeyString + ")")

                    //12 - analyze user doc, do recovery if needed
                    lcb_cas_t userAccountCas = m_LastGetCas;
                    ptree pt6;
                    std::istringstream is6(m_LastDocGetted);
                    read_json(is6, pt6);
                    bool userAccountIsLockedPointingAtSlotJustFilled = (pt6.get<std::string>(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, "n") == slotThatShouldntExistAkaBeFilledButMightKey ? true : false);
                    std::string purchasePriceString = pt4.get<std::string>(JSON_AD_SPACE_CAMPAIGN_SLOT_PURCHASE_PRICE);
                    if(!transactionDocExists)
                    {
                        //12a
                        //if transaction doc doesn't exist, we make sure the user account is locked and pointing at slot just filled (BUT it might not be because of a race condition)
                        if(!userAccountIsLockedPointingAtSlotJustFilled)
                        {
                            //12a.i
                            //there is one last race condition check to prove that total system failure didn't occur (whether transaction doc now exists)
                            ABCP_DO_COUCHBASE_GET_REQUEST_WITH_EXPONENTIAL_BACKOFF(transactionDocKey, "tx that must exist: (" + transactionDocKey + ")")
                            if(m_LastOpStatus != LCB_SUCCESS) //TODOreq: exponential backoffs etc xD
                            {
                                cerr << "TOTAL SYSTEM FAILURE: USER ACCOUNT WAS UNLOCKED WITHOUT TRANSACTION DOC BEING CREATED: (" + transactionDocKey + "): " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl; //TODOreq: email yourself and/or kill abc2 actual, or something?
                                lcb_destroy(m_Couchbase);
                                return 1;
                            }
                            //total system failure averted
                            //the transaction doc exists now, which is proof that the driver or another recoverer is active so we can just stop (even if they crashed, another recoverer would get it). simplifies code and lessens overall system load
                        }
                        else
                        {
                            //12a.ii -- transaction needs create -> user account needs unlocking -> campaign doc update

                            //add-accepting-fail the transaction document itself
                            ptree pt7;
                            pt7.put(JSON_TRANSACTION_BUYER, usernameOfBuyer);
                            pt7.put(JSON_TRANSACTION_SELLER, "d3fault");
                            pt7.put(JSON_TRANSACTION_AMOUNT, purchasePriceString);
                            std::ostringstream transactionJsonBuffer;
                            write_json(transactionJsonBuffer, pt7, false);
                            std::string transactionJson = transactionJsonBuffer.str();

                            ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF(transactionDocKey, transactionJson, LCB_ADD, 0, "transaction add")
                            //TODOreq: durability poll on the tx add (??????)

                            if(m_LastOpStatus != LCB_SUCCESS)
                            {
                                if(m_LastOpStatus != LCB_KEY_EEXISTS)
                                {
                                    cerr << "Failed to add transaction: " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl;
                                    lcb_destroy(m_Couchbase);
                                    return 1;
                                }
                                continue; //bowing out to neighbor/driver already on top of it
                            }
                            //success or add-fail (driver or neighbor responsible) of transaction creation (TODOoptimization: each 'acceptable fail' stage could verify whatever beat them to the punch. It wouldn't apply to all the docs (campaign for example could have changed TWICE), but the ones that were LCB_ADD'd definitely could do it)


                            //cas-swap-accepting-fail the user-account to unlock+debit it

                            DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_USER_ACCOUNT_DEBIT_AND_UNLOCK
                            //success or just cas-swap-fail (driver or neighbor responsible) of unlocking+debitting user-account


                            //cas-swap-accepting-fail updating the campaign doc

                            DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_CAMPAIGN_DOC_UPDATE
                            //success or just cas-swap-fail (driver or neighbor responsible) of updating campaign doc, RECOVERY COMPLETE
                        }
                    }
                    else
                    {
                        //transaction doc exists
                        //12b
                        if(userAccountIsLockedPointingAtSlotJustFilled)
                        {
                            //'unlock+debitting' the user-account
                            DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_USER_ACCOUNT_DEBIT_AND_UNLOCK
                        }
                        //else, update campaign doc only (sharing code path)


                        //update campaign doc
                        DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_CAMPAIGN_DOC_UPDATE
                    }
                }
                //else, typical race condition detected so recovery not needed
            }
            //else, monitor goes back to sleep because the state appears fine :)
        }
    }
}
void Abc2PessimisticStateMonitorAndRecoverer::errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo)
{
    const_cast<Abc2PessimisticStateMonitorAndRecoverer*>(static_cast<const Abc2PessimisticStateMonitorAndRecoverer*>(lcb_get_cookie(instance)))->errorCallback(error, errinfo);
}
void Abc2PessimisticStateMonitorAndRecoverer::errorCallback(lcb_error_t error, const char *errinfo)
{
    cerr << "Got an error in our couchbase error callback: " << lcb_strerror(m_Couchbase, error) << " / " << errinfo; //TODOreq: prolly need to break out of the while(true) loop if we get an error here (?)
}
void Abc2PessimisticStateMonitorAndRecoverer::configurationCallbackStatic(lcb_t instance, lcb_configuration_t config)
{
    const_cast<Abc2PessimisticStateMonitorAndRecoverer*>(static_cast<const Abc2PessimisticStateMonitorAndRecoverer*>(lcb_get_cookie(instance)))->configurationCallback(config);
}
void Abc2PessimisticStateMonitorAndRecoverer::configurationCallback(lcb_configuration_t config)
{
    if(config == LCB_CONFIGURATION_NEW)
    {
        m_Connected = true;
    }
}
void Abc2PessimisticStateMonitorAndRecoverer::getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    const_cast<Abc2PessimisticStateMonitorAndRecoverer*>(static_cast<const Abc2PessimisticStateMonitorAndRecoverer*>(lcb_get_cookie(instance)))->getCallback(cookie, error, resp);
}
void Abc2PessimisticStateMonitorAndRecoverer::getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    (void)cookie;
    m_LastOpStatus = error;
    if(error == LCB_SUCCESS)
    {
        //TODOreq: these members are undefined if the error wasn't success, need to make sure i don't try to access them in such cases
        m_LastDocGetted = std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes); //TODOreq: leaking memory from m_LastDocGetted's previous value?
        m_LastGetCas = resp->v.v0.cas;
    }
}
void Abc2PessimisticStateMonitorAndRecoverer::storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    const_cast<Abc2PessimisticStateMonitorAndRecoverer*>(static_cast<const Abc2PessimisticStateMonitorAndRecoverer*>(lcb_get_cookie(instance)))->storeCallback(cookie, operation, error, resp);
}
void Abc2PessimisticStateMonitorAndRecoverer::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)cookie;
    (void)operation;
    (void)resp;
    m_LastOpStatus = error;
}

#include "abc2pessimisticstatemonitorandrecoverer.h"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost::property_tree;

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

    srand(time(NULL));
    while(true)
    {
        //TODOreq: portable way to break out of this, SIGTERM etc
        sleep(1); //fancy boost timer libs? libevent timers? nahh

        if(rand() % 10 == 0)
        {
            //selected ourself to do monitor (recovery probably not necessary)

            //0 - get campaign doc (note these numbers do NOT match the 'perfected.the.design.here.is.the.flow.aka.pseudocode.txt')
            std::string campaignDocKey = "adSpaceSlotsd3fault0";
            DO_COUCHBASE_GET_CAMPAIGN_DOC()

            //1 - analyze campaign doc, so we know which slot+1 to pessimistically check the existence of
            ptree pt;
            std::istringstream is(m_LastDocGetted);
            read_json(is, pt);
            boost::optional<ptree&> lastSlotFilledAkaPurchased = pt.get_child_optional("lastSlotFilledAkaPurchased");
            int slotIndexForSlotThatShouldntExistButMight = 0;
            std::string slotIndexForSlotThatShouldntExistButMightString = "0";
            if(lastSlotFilledAkaPurchased.is_initialized())
            {
                //n + 1
                slotIndexForSlotThatShouldntExistButMight = boost::lexical_cast<int>(lastSlotFilledAkaPurchased.get().get<std::string>("slotIndex")) + 1;
                slotIndexForSlotThatShouldntExistButMightString = boost::lexical_cast<std::string>(slotIndexForSlotThatShouldntExistButMight);
            }
            //else, no purchases so watch index 0

            //2 - check (via get) for existence of doc that shouldn't exist (but very well might because of a not-so-rare race condition)

            std::string slotThatShouldntExistAkaBeFilledButMightKey = "adSpaceSlotsd3fault0Slot" + slotIndexForSlotThatShouldntExistButMightString;

            {
                lcb_get_cmd_t cmd;
                const lcb_get_cmd_t *cmds[1];
                cmds[0] = &cmd;
                memset(&cmd, 0, sizeof(cmd));
                cmd.v.v0.key = slotThatShouldntExistAkaBeFilledButMightKey.c_str();
                cmd.v.v0.nkey = slotThatShouldntExistAkaBeFilledButMightKey.length();
                error = lcb_get(m_Couchbase, NULL, 1, cmds);
                if(error  != LCB_SUCCESS)
                {
                    cerr << "Failed to setup get request for slot that shouldn't exist (adSpaceSlotsd3fault0Slot" + slotIndexForSlotThatShouldntExistButMightString + "): " << lcb_strerror(m_Couchbase, error) << endl;
                    lcb_destroy(m_Couchbase);
                    return 1;
                }
            }
            if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
            {
                cerr << "Failed to lcb_wait after get request for slot that shouldn't exist (adSpaceSlotsd3fault0Slot" + slotIndexForSlotThatShouldntExistButMightString + "): " << lcb_strerror(m_Couchbase, error) << endl;
                lcb_destroy(m_Couchbase);
                return 1;
            }

            //3 - we WANT to see LCB_KEY_ENOENT (key doesn't exist)
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

                //5 - wait another 100ms for the state to correct itself (race condition, the driver could still be functioning fine)
                usleep(100000);

                //6 - we've waited 100ms for driver to finish, so now we check campaign doc to see whether or not we need to continue
                DO_COUCHBASE_GET_CAMPAIGN_DOC()
                //7 - analyze campaign doc AGAIN
                ptree pt2;
                std::istringstream is2(m_LastDocGetted);
                read_json(is2, pt2);
                boost::optional<ptree&> lastSlotFilledAkaPurchased2 = pt2.get_child_optional("lastSlotFilledAkaPurchased");
                bool campaignDocWasFixedIn100msExtraTimeWeGaveIt = false;
                if(lastSlotFilledAkaPurchased2.is_initialized())
                {
                    //if(lastSlotFilledAkaPurchased2.get().get<std::string>("slotIndex") == nextSlotIndexString) //actually, it can be greater than or equal to (incredibly rare race condition, but hey...)
                    int slotIndexInCampaignThisTimeAround = boost::lexical_cast<int>(lastSlotFilledAkaPurchased2.get().get<std::string>("slotIndex"));
                    if(slotIndexInCampaignThisTimeAround >= slotIndexForSlotThatShouldntExistButMight)
                    {
                        //driver (or neighbor recoverer) updated it -- very common race condition, recovery mode complete
                        campaignDocWasFixedIn100msExtraTimeWeGaveIt = true;
                    }
                }
                //else: STILL no last purchase in campaign doc means we need to do recovery for the slot we just discovered exists

                if(!campaignDocWasFixedIn100msExtraTimeWeGaveIt)
                {
                    //8 - ok shit just got real because it still isn't up to date, time to try all of the post-slot-fill tasks in order (in order) to fix the state
                    lcb_cas_t campaignDocToUpdateCAS = m_LastGetCas; //save this for later...

                    //9 - check to see if transaction doc exists (TODOoptimization: maybe we can/should just LCB_ADD accepting fail the tx at this point?)
                    std::string transactionDocKey = "txd3fault0Slot" + slotIndexForSlotThatShouldntExistButMightString;
                    DO_COUCHBASE_GET_TRANSACTION_DOC()

                    //10 - transaction 'get' finished, so depending on whether or not it exists we take a certain recovery path (but just use a bool flag since they're mostly the same)
                    bool transactionDocExists = false;
                    if(m_LastOpStatus != LCB_KEY_ENOENT)
                    {
                        //might be some other db error
                        if(m_LastOpStatus != LCB_SUCCESS)
                        {
                            cerr << "Got an error other than LCB_KEY_ENOENT (not exist) while checking for the existence of the transaction that might exist (txd3fault0Slot" + slotIndexForSlotThatShouldntExistButMightString + "): " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl;
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
                        usernameOfBuyer = pt3.get<std::string>("buyer");
                    }
                    else
                    {
                        //we have to look up the slot filler we're doing recovery for in order to get the username of the buyer
                        std::string slotFilledWithAkaSlotFillerKey = pt4.get<std::string>("slotFilledWith");

                        {
                            lcb_get_cmd_t cmd;
                            const lcb_get_cmd_t *cmds[1];
                            cmds[0] = &cmd;
                            memset(&cmd, 0, sizeof(cmd));
                            cmd.v.v0.key = slotFilledWithAkaSlotFillerKey.c_str();
                            cmd.v.v0.nkey = slotFilledWithAkaSlotFillerKey.length();
                            error = lcb_get(m_Couchbase, NULL, 1, cmds);
                            if(error  != LCB_SUCCESS)
                            {
                                cerr << "Failed to setup get request for slot filler to retrieve username from it (" + slotFilledWithAkaSlotFillerKey + "): " << lcb_strerror(m_Couchbase, error) << endl;
                                lcb_destroy(m_Couchbase);
                                return 1;
                            }
                        }
                        if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
                        {
                            cerr << "Failed to lcb_wait after get request for slot filler to retrieve username from it (" + slotFilledWithAkaSlotFillerKey + "): " << lcb_strerror(m_Couchbase, error) << endl;
                            lcb_destroy(m_Couchbase);
                            return 1;
                        }
                        if(m_LastOpStatus != LCB_SUCCESS)
                        {
                            cerr << "Failed to get slot filler to retrieve username from it (" + slotFilledWithAkaSlotFillerKey + "): " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl;
                            lcb_destroy(m_Couchbase);
                            return 1;
                        }

                        ptree pt5;
                        std::istringstream is5(m_LastDocGetted);
                        read_json(is5, pt5);
                        usernameOfBuyer = pt5.get<std::string>("username");
                    }

                    //11b - get the user account of the buyer and do recovery tasks if needed
                    std::string userAccountKey = "user" + usernameOfBuyer;

                    {
                        lcb_get_cmd_t cmd;
                        const lcb_get_cmd_t *cmds[1];
                        cmds[0] = &cmd;
                        memset(&cmd, 0, sizeof(cmd));
                        cmd.v.v0.key = userAccountKey.c_str();
                        cmd.v.v0.nkey = userAccountKey.length();
                        error = lcb_get(m_Couchbase, NULL, 1, cmds);
                        if(error  != LCB_SUCCESS)
                        {
                            cerr << "Failed to setup get request for user account (user" + usernameOfBuyer + "): " << lcb_strerror(m_Couchbase, error) << endl;
                            lcb_destroy(m_Couchbase);
                            return 1;
                        }
                    }
                    if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
                    {
                        cerr << "Failed to lcb_wait after get request for user account (user" + usernameOfBuyer + "): " << lcb_strerror(m_Couchbase, error) << endl;
                        lcb_destroy(m_Couchbase);
                        return 1;
                    }
                    if(m_LastOpStatus != LCB_SUCCESS)
                    {
                        cerr << "Failed to get user account (user" + usernameOfBuyer + "): " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl;
                        lcb_destroy(m_Couchbase);
                        return 1;
                    }

                    //12 - analyze user doc, do recovery if needed
                    lcb_cas_t userAccountCas = m_LastGetCas;
                    ptree pt6;
                    std::istringstream is6(m_LastDocGetted);
                    read_json(is6, pt6);
                    bool userAccountIsLockedPointingAtSlotJustFilled = (pt6.get<std::string>("slotToAttemptToFillAkaPurchase", "n") == slotThatShouldntExistAkaBeFilledButMightKey ? true : false);
                    std::string purchasePriceString = pt4.get<std::string>("purchasePrice");
                    if(!transactionDocExists)
                    {
                        //12a
                        //if transaction doc doesn't exist, we make sure the user account is locked and pointing at slot just filled (BUT it might not be because of a race condition)
                        if(!userAccountIsLockedPointingAtSlotJustFilled)
                        {
                            //12a.i
                            //there is one last race condition check to prove that total system failure didn't occur (whether transaction doc now exists)
                            DO_COUCHBASE_GET_TRANSACTION_DOC()
                            if(m_LastOpStatus != LCB_SUCCESS) //TODOreq: exponential backoffs etc xD
                            {
                                cerr << "TOTAL SYSTEM FAILURE: USER ACCOUNT WAS UNLOCKED WITHOUT TRANSACTION DOC BEING CREATED: (txd3fault0Slot" + slotIndexForSlotThatShouldntExistButMightString + "): " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl; //TODOreq: email yourself and/or kill abc2 actual, or something?
                                lcb_destroy(m_Couchbase);
                                return 1;
                            }
                            //total system failure averted
                            //the transaction doc exists now, which is proof that the driver or another recoverer is active so we can just stop (even if they crashed, another recoverer would get it). simplifies code
                        }
                        else
                        {
                            //12a.ii -- transaction needs create -> user account needs unlocking -> campaign doc update

                            //add-accepting-fail the transaction document itself

                            {
                                lcb_store_cmd_t cmd;
                                const lcb_store_cmd_t *cmds[1];
                                cmds[0] = &cmd;
                                memset(&cmd, 0, sizeof(cmd));
                                cmd.v.v0.key = transactionDocKey.c_str();
                                cmd.v.v0.nkey = transactionDocKey.length();

                                ptree pt7;
                                pt7.put("buyer", usernameOfBuyer);
                                pt7.put("seller", "d3fault");
                                pt7.put("amount", purchasePriceString);
                                std::ostringstream transactionJsonBuffer;
                                write_json(transactionJsonBuffer, pt7, false);
                                std::string transactionJson = transactionJsonBuffer.str();

                                cmd.v.v0.bytes = transactionJson.c_str();
                                cmd.v.v0.nbytes = transactionJson.length();
                                cmd.v.v0.operation = LCB_ADD;
                                error = lcb_store(m_Couchbase, NULL, 1, cmds);
                                if(error != LCB_SUCCESS)
                                {
                                    cerr << "Failed to set up set request for transaction add: " << lcb_strerror(m_Couchbase, error) << endl;
                                    lcb_destroy(m_Couchbase);
                                    return 1;
                                }
                            }
                            if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
                            {
                                cerr << "Failed to lcb_wait after set request for transaction add: " << lcb_strerror(m_Couchbase, error) << endl;
                                lcb_destroy(m_Couchbase);
                                return 1;
                            }
                            if(m_LastOpStatus != LCB_SUCCESS)
                            {
                                if(m_LastOpStatus != LCB_KEY_EEXISTS)
                                {
                                    cerr << "Failed to add transaction: " << lcb_strerror(m_Couchbase, m_LastOpStatus) << endl;
                                    lcb_destroy(m_Couchbase);
                                    return 1;
                                }
                            }
                            //success or add-fail (driver or neighbor responsible) of transaction creation (TODOoptimization: each 'acceptable fail' stage could verify whatever beat them to the punch. It wouldn't apply to all the docs (campaign for example could have changed TWICE), but the ones that were LCB_ADD'd definitely could do it)


                            //cas-swap-accepting-fail the user-account to unlock+debit it

                            DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_USER_ACCOUNT_DEBIT_AND_UNLOCK()
                            //success or just cas-swap-fail (driver or neighbor responsible) of unlocking+debitting user-account


                            //cas-swap-accepting-fail updating the campaign doc

                            DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_CAMPAIGN_DOC_UPDATE()
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
                            DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_USER_ACCOUNT_DEBIT_AND_UNLOCK()
                        }
                        //else, update campaign doc only (sharing code path)


                        //update campaign doc
                        DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_CAMPAIGN_DOC_UPDATE()
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

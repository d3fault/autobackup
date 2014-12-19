#include "abc2pessimisticstatemonitorandrecoverer.h"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/filesystem.hpp>

#include "abc2couchbaseandjsonkeydefines.h"

using namespace std;
using namespace boost::property_tree;

//BLOCKER DO NOT RUN THIS APP UNTIL FIXING ALL OCCURANCES OF STRING "d3fault" (and sometimes an associated campaign index "0") hardcoded within. Should probably follow the "event driven monitor/reccovery" window triggering (see below)

#define ABC2_PESSIMISTIC_STATE_MONITOR_AND_RECOVERER_STOP_FILE "/run/shm/stopPessimisticStateMonitorAndRecoverer" //have a better/portable way (my qt way is the shit :-P), i'm open to suggestions. tmpfs at least saves us a hdd hit (but so would hdd cache prolly xD)

#define DO_COUCHBASE_CAS_SWAP_ACCEPTING_FAIL_OF_USER_ACCOUNT_DEBIT_AND_UNLOCK \
SatoshiInt buyerBalance = satoshiStringToSatoshiInt(pt6.get<std::string>(JSON_USER_ACCOUNT_BALANCE)); \
SatoshiInt purchasePrice = satoshiStringToSatoshiInt(purchasePriceString); \
buyerBalance -= purchasePrice; \
pt6.erase(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL); \
pt6.erase(JSON_USER_ACCOUNT_SLOT_TO_ATTEMPT_TO_FILL_IT_WITH); \
pt6.put(JSON_USER_ACCOUNT_BALANCE, satoshiIntToSatoshiString(buyerBalance)); \
std::ostringstream userAccountDebittedAndUnlockedJsonBuffer; \
write_json(userAccountDebittedAndUnlockedJsonBuffer, pt6, false); \
std::string userAccountDebittedAndUnlockedJson = userAccountDebittedAndUnlockedJsonBuffer.str(); \
if(!couchbaseStoreRequestWithExponentialBackoff(userAccountKeyString, userAccountDebittedAndUnlockedJson, LCB_SET, userAccountCas, "cas-swap user account unlock+debitting")) \
    return 1; \
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
if(!couchbaseStoreRequestWithExponentialBackoff(adSpaceCampaignKey("d3fault", "0"), updatedCampaignDocJson, LCB_SET, campaignDocToUpdateCAS, "cas-swap updating campaign doc")) \
    return 1; \
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
Abc2PessimisticStateMonitorAndRecoverer::Abc2PessimisticStateMonitorAndRecoverer(int argc, char *argv[])
    : ISynchronousLibCouchbaseUser()
{
    //TODOreq: parse the target campaign [owner+index] from argv[1] and argv[2]. They are mandatory (but I guess I COULD default to d3fault ;-P). But honestly this monitor and recoverer doesn't scale that well to begin with... :-/... or maybe it does.
}

//TODOreq: If the driver's durability polling (replication) takes longer than one second, we'll get a lot of false positives here. Additionally, we should do a durability poll of our own for the "slot that just appeared" before we do the "100ms extra time" thing. Lastly, we should do durability polling for all of our recovery storings.
//TODOreq: make scalable to multiple ad campaigns by making the 'monitor and recover' an event-triggered (pre ad slot purchase) "window", instead of constantly polling 24/7 like I am now
int Abc2PessimisticStateMonitorAndRecoverer::startPessimisticallyMonitoringAndRecovereringStateUntilToldToStop()
{
    if(boost::filesystem::exists(ABC2_PESSIMISTIC_STATE_MONITOR_AND_RECOVERER_STOP_FILE))
    {
        if(remove(ABC2_PESSIMISTIC_STATE_MONITOR_AND_RECOVERER_STOP_FILE) != 0)
        {
            cerr << "failed to remove stop file: " ABC2_PESSIMISTIC_STATE_MONITOR_AND_RECOVERER_STOP_FILE << endl;
            return 1;
        }
    }

    if(!connectToCouchbase())
        return 1;

    boost::random::random_device randomNumberGenerator;
    boost::random::uniform_int_distribution<> randomNumberRange(0, 9);
    for(;;)
    {
        //TODOreq: portable way to break out of this, SIGTERM etc
        sleep(1); //fancy boost timer libs? libevent timers? nahh

        if(boost::filesystem::exists(ABC2_PESSIMISTIC_STATE_MONITOR_AND_RECOVERER_STOP_FILE))
        {
            cout << "Abc2PessimisticStateMonitorAndRecoverer is stopping because stop file (" ABC2_PESSIMISTIC_STATE_MONITOR_AND_RECOVERER_STOP_FILE ") exists. The stop file will be deleted on next startup" << endl; //would be trivial to make it not startable if the file exists (and to delete on stopping (now))
            return 0;
        }

        if(randomNumberRange(randomNumberGenerator) == 0)
        {
            //selected ourself to do audit (recovery probably not necessary)

            //0 - get campaign doc (note these numbers do NOT match the 'perfected.the.design.here.is.the.flow.aka.pseudocode.txt')
            if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(adSpaceCampaignKey("d3fault", "0"), "campaign doc for initial audit"))
                return 1;

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
            if(!couchbaseGetRequestWithExponentialBackoff(slotThatShouldntExistAkaBeFilledButMightKey, "slot that shouldn't exist (" + slotThatShouldntExistAkaBeFilledButMightKey + ")"))
                return 1;

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
                if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(adSpaceCampaignKey("d3fault", "0"), "campaign doc after 100ms wait"))
                    return 1;

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
                    const std::string transactionDocKey = transactionKey("d3fault" /*TODOreq: this needs to be dynamic*/, "0", slotIndexForSlotThatShouldntExistButMightString);
                    if(!couchbaseGetRequestWithExponentialBackoff(transactionDocKey, "tx that might exist (" + transactionDocKey + "): "))
                        return 1;

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
                        if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(slotFilledWithAkaSlotFillerKey, "slot filler to retrieve username of buyer from it: (" + slotFilledWithAkaSlotFillerKey + ")"))
                            return 1;

                        ptree pt5;
                        std::istringstream is5(m_LastDocGetted);
                        read_json(is5, pt5);
                        usernameOfBuyer = pt5.get<std::string>(JSON_SLOT_FILLER_USERNAME);
                    }

                    //11b - get the user account of the buyer and do recovery tasks if needed
                    const std::string userAccountKeyString = userAccountKey(usernameOfBuyer);
                    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(userAccountKeyString, "user account doc (" + userAccountKeyString + ")"))
                        return 1;

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
                            if(Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(pt6))
                            {
                                //user account is locked towards SOMETHING ELSE
                                //TODOmb: cerr or cout? idfk
                                continue; //we can only handle locks 'pointing at slot just filled' in this app, so just continue as if done (fixing that locked state is the job of abc2 withdrawal request processor (or other if/when I add more lock types))
                            }

                            //12a.i
                            //there is one last race condition check to prove that total system failure didn't occur (whether transaction doc now exists)
                            if(!couchbaseGetRequestWithExponentialBackoff(transactionDocKey, "tx that must exist: (" + transactionDocKey + ")"))
                                return 1;
                            if(m_LastOpStatus != LCB_SUCCESS)
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
                            Abc2CouchbaseAndJsonKeyDefines::createTransactionDoc(pt7, "d3fault" /*TODOreq: this needs to be dynamic*/, usernameOfBuyer, purchasePriceString);
                            std::ostringstream transactionJsonBuffer;
                            write_json(transactionJsonBuffer, pt7, false);
                            std::string transactionJson = transactionJsonBuffer.str();

                            if(!couchbaseStoreRequestWithExponentialBackoff(transactionDocKey, transactionJson, LCB_ADD, 0, "transaction add"))
                                return 1;
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
                        else if(Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(pt6))
                        {
                            //user account is locked towards SOMETHING ELSE
                            //TODOmb: cerr or cout? idfk
                            continue; //we can only handle locks 'pointing at slot just filled' in this app, so just continue as if done (fixing that locked state is the job of abc2 withdrawal request processor (or other if/when I add more lock types))
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
void Abc2PessimisticStateMonitorAndRecoverer::errorOutput(const string &errorString)
{
    cerr << errorString << endl;
}


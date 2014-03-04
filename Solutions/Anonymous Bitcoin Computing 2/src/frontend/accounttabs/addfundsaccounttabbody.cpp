#include "addfundsaccounttabbody.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "../anonymousbitcoincomputingwtgui.h"

using namespace boost::random;

AddFundsAccountTabBody::AddFundsAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp)
    : IAccountTabWidgetTabBody(abcApp),
      m_BitcoinAddressBalancePollerPollingPendingBalance(true),
      m_BitcoinAddressBalancePoller(0),
      m_PendingBitcoinBalanceLabel(0),
      m_ConfirmedBitcoinBalanceLabel(0),
      m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked(0)
{ }
void AddFundsAccountTabBody::populateAndInitialize()
{
    new WText("Fund Your Account via Bitcoin", this);
    new WBreak(this); //gap before any responses

    m_AbcApp->deferRendering();
    //even though we are already logged in, we still need to get the user-account doc because if we kept it in memory instead it might be stale or some shit. could be wrong here idfk, but fuck it

    //we get the bitcoin state to present them either: 1) get key button, 2) check for pending button, or 3) doing HaveKey recovery (which eventually gives them 'check for pending button' like (2))
    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername)); //ACTUALLY: changing back to get with cas because if we see the state is in "GettingKey" mode, we do light recovery from there and in fact need the cas for that. NOPE: was tempted to have this be a 'get with cas', but the user might wait a while and possibly do other things (slot fills namely) before doing any further actions, so the CAS may be hella stale by then.
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::DETERMINEBITCOINSTATEBUTDONTLETTHEMPROCEEDIFLOCKEDATTEMPTINGTOFILLAKAPURCHASESLOT;
}
void AddFundsAccountTabBody::determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot(const string &userAccountJsonDoc, u_int64_t casOnlyUsedWhenBitcoinStateIsInGettingKey_aka_lightRecovery, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //get failed?? should never happen

        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp (we should 'notify' ourselves for any of these total system failures), or i guess hook one up to cerr ;-P
        cerr << "TOTAL SYSTEM FAILURE: user account doc doesn't exist and we're logged in wtfz? in determineBitcoinStateButDontLetThemProceedForwardIfLockedAttemptingToFillAkaPurchaseSlot method" << endl;

        m_AbcApp->resumeRendering();
        return;
    }

    //got user account doc

    //we can tell them the state of add funds (tell them their key if they have one), but we must not allow transitions if "slot attempting to fill" is set (TODOreq: we also check it isn't set after the buttons are clicked, in case they have multiple tabs open). For example we shouldn't proceed with recovery if both "slotToAttemptToFillAkaPurchase" is set and BitcoinState = GettingKey. We must first unlock the slot fill lock, since it is more important.
    ptree pt;
    std::istringstream is(userAccountJsonDoc);
    read_json(is, pt);

    string slotToAttemptToFillAkaPurchase_LOCKED_CHECK = pt.get<std::string>(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, "n");
    string bitcoinState = pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_STATE);
    if(bitcoinState == JSON_USER_ACCOUNT_BITCOIN_STATE_HAVE_KEY)
    {
        presentBitcoinKeyForPaymentsToUser(pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_STATE_DATA));
    }
    else if(slotToAttemptToFillAkaPurchase_LOCKED_CHECK == "n")
    {
        if(bitcoinState == JSON_USER_ACCOUNT_BITCOIN_STATE_NO_KEY)
        {
            WPushButton *getKeyButton = new WPushButton("Get Bitcoin Key", this); //TODOreq: hide after clicking, unhide somewhere(s) (or just delete/recreate idfk)
            getKeyButton->clicked().connect(this, &AddFundsAccountTabBody::handleGetBitcoinKeyButtonClicked);
        }
        else if(bitcoinState == JSON_USER_ACCOUNT_BITCOIN_STATE_GETTING_KEY)
        {
            //light/safe recovery (don't resume rendering [yet] either)
            checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(userAccountJsonDoc, casOnlyUsedWhenBitcoinStateIsInGettingKey_aka_lightRecovery, true, false); //re-parsing json and re-checking slotToAttemptToFillAkaPurchase not set, but saving lines of code and complexity...

            return; //no resume rendering. TO DOnereq(worked-in-testing): since recovery and the button are slightly different paths, my defer/resume counts might not match and i need to hackily fix that. starting to think they match up perfectly. at least the code functions just fine. "get bitcoin key" push button slot defers, whereas we are ALREADY deferred and join up with that code moments later... so yea i think they match...
        }
    }
    else
    {
        new WBreak(this);
        new WText("You are attempting to purchase a slot, so bitcoin key activity is disabled until that finishes", this);
    }
    m_AbcApp->resumeRendering();
}
void AddFundsAccountTabBody::handleGetBitcoinKeyButtonClicked()
{
    m_AbcApp->deferRendering();

    //TODOreq: get user-account for CAS, check AGAIN that slotAttemptingToFill isn't set, then do the hella long pseudo-code for getting a bitcoin key

    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETUSERACCOUNTFORGOINGINTOGETTINGBITCOINKEYMODE;
}
void AddFundsAccountTabBody::checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState(const string &userAccountDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    //TODOreq: we check that we are in fact in "NoKey" state before we transition out of it because if they have multiple tabs open or something they could have already requested one and done the transition (in which case, we'd be transitioning into ourselves (and getting another key?). TODOreq: all such transitions check the transition FROM state is what is expected

    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "TOTAL SYSTEM FAILURE USER ACCOUNT DIDN'T EXIST AND WE ARE LOGGED IN WTFZ: checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState" << endl;

        m_AbcApp->resumeRendering();
        return;
    }

    m_UserAccountJsonForLockingIntoGettingBitcoinKey = userAccountDoc;
    m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking = cas;

    ptree pt;
    std::istringstream is(userAccountDoc);
    read_json(is, pt);

    string slotToAttemptToFillAkaPurchase_LOCKED_CHECK = pt.get<std::string>(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, "n");
    if(slotToAttemptToFillAkaPurchase_LOCKED_CHECK != "n")
    {
        new WBreak(this);
        new WText("It appears you are attempting to purchase a slot. Wait until that finishes and try again", this);
        m_AbcApp->resumeRendering();
        return;
    }

    string bitcoinState = pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_STATE);
    if(bitcoinState != JSON_USER_ACCOUNT_BITCOIN_STATE_NO_KEY)
    {
        if(bitcoinState != JSON_USER_ACCOUNT_BITCOIN_STATE_GETTING_KEY) //TODOreq: this is our second check? why the below message and not just giving them the key?
        {
            new WBreak(this);
            new WText("Please only use one browser tab/window at a time", this); //TODOreq: we should throw them out of add funds mode or something (making them click it again)? idfk
            m_AbcApp->resumeRendering();
            return;
        }

        //GettingKey (recovery)
        m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked = true;

        //Fill in details from previous GettingKey attempt
        m_BitcoinKeySetIndex_aka_setN = pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_STATE_DATA);
        m_BitcoinKeySetPage_aka_PageY = pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_SET_PAGE);
        m_PerGetBitcoinKeyUUID = pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_GET_KEY_UUID);
    }
    else //bitcoinState == "NoKey"
    {
        //begin transitioning to "GettingKey", but first check slotToAttemptToFillAkaPurchase still isn't set
        m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked = false;

        //generate random set number
        mt19937 rng;
        rng.seed(static_cast<int>(WDateTime::currentDateTime().toTime_t()));
        uniform_int_distribution<> zeroThrough999(0,(ABC_NUM_BITCOINKEYSETS-1));
        m_BitcoinKeySetIndex_aka_setN = boost::lexical_cast<std::string>(zeroThrough999(rng));
        //generate uuid-per-request (could be done later, but needs to not overwrite recovery one set above, so best to do it here/now)
        string uuidSeed = m_AbcApp->m_CurrentlyLoggedInUsername + uniqueId() + WDateTime::currentDateTime().toString().toUTF8() + boost::lexical_cast<std::string>(m_BitcoinKeySetIndex_aka_setN);
        m_PerGetBitcoinKeyUUID = base64Encode(sha1(uuidSeed)); //after one look at boost::uuid, looks shit

        //get the page that that set is currently on
        m_BitcoinKeySetPage_aka_PageY = ""; //hack to tell us we aren't in recovery code (recovery code already has page, _AND_ recovery can't use 'most recent' because it needs to check 'old' pages for the UUID).
        //TODOreq: ^not there yet, but i think i need a similar hack for PageZ
    }

    //recovery and normal mode MERGE

    //save cas because way later if we try to fill the next page, we need the cas to lock it
    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(bitcoinKeySetCurrentPageKey(m_BitcoinKeySetIndex_aka_setN)); //this exact get (incl enum) is copy/pasted to a later point when a cas-swap fails
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED;

    //doesn't belong here, but see "SHIT editting this doc" in "bitcoin.nonsense.ramblings.[...].txt", it is a TODOreq that is very important
}
void AddFundsAccountTabBody::analyzeBitcoinKeySetN_CurrentPageDocToSeeWhatPageItIsOnAndIfItIsLocked(const string &bitcoinKeySetCurrentPageDoc, u_int64_t casOnlyForUseInFillingNextPageWayLaterOn, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "analyzeBitcoinKeySetPageCacheDocToSeeWhatPageItIsOnAndIfItIsLocked db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "TOTAL SYSTEM FAILURE: analyzeBitcoinKeySetPageCacheDocToSeeWhatPageItIsOnAndIfItIsLocked lcb op failed" << endl;

        m_AbcApp->resumeRendering();
        return;
    }

    m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater = casOnlyForUseInFillingNextPageWayLaterOn;

    //got the CurrentPage
    ptree pt;
    std::istringstream is(bitcoinKeySetCurrentPageDoc);
    read_json(is, pt);

    std::string fillingNextBitcoinKeySetPage_LOCKED_CHECK = pt.get<std::string>(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE, "n"); //TODOreq(done i think via just making a new "currentPage": "X" doc, not explicitly erasing): pt::erase this and siblings at appropriate places
    if(fillingNextBitcoinKeySetPage_LOCKED_CHECK != "n")
    {
        //light recovery of filling next bitcoin key set page
        //jumps way the fuck deep into recovery code, then pops back to us later if user account isn't locked (flag already set previously).

        //populate the fillingNextBitcoinKeySetPage/uuid/pageZ from the locked doc (we are recovery. normal code path generates the 2nd two)
        m_BitcoinKeySetPage_aka_PageY = fillingNextBitcoinKeySetPage_LOCKED_CHECK;
        m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds = pt.get<std::string>(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_UUID_PER_FILL);
        m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList = pt.get<std::string>(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_START_LOOKING_ON_HUGEBITCOINKEYLIST_PAGE);
        m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING = m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList;

        m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont = false;

        //the jump into recovery
        bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    }
    else //bitcoin key set not locked in fillingNextBitcoinKeySetPage mode, so set PageY using it's contents
    {
        if(m_BitcoinKeySetPage_aka_PageY == "")
        {
            //aren't in get-key recovery code, so we need to get/set PageY
            m_BitcoinKeySetPage_aka_PageY = pt.get<std::string>(JSON_BITCOIN_KEY_SET_CURRENT_PAGE);
        }
        proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked(); //method because we jump "[back]" to it after doing the page-fill also, and some other places
    }
}
void AddFundsAccountTabBody::proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked()
{
    if(!m_UserAccountBitcoinDoingGettingKeyRecoverySoAccountLocked)
    {
        //CAS-Swap-Lock attmempt a user-specific doc into "attempting to get a key from PageY of setN using UUID-per-key-request"

        //TODOreq: verify we already checked slotAttemptingToBeFilled isn't set. pretty sure we did.

        ptree pt;
        std::istringstream is(m_UserAccountJsonForLockingIntoGettingBitcoinKey);
        read_json(is, pt);

        pt.put(JSON_USER_ACCOUNT_BITCOIN_STATE, JSON_USER_ACCOUNT_BITCOIN_STATE_GETTING_KEY);
        pt.put(JSON_USER_ACCOUNT_BITCOIN_STATE_DATA, m_BitcoinKeySetIndex_aka_setN);
        pt.put(JSON_USER_ACCOUNT_BITCOIN_SET_PAGE, m_BitcoinKeySetPage_aka_PageY);
        pt.put(JSON_USER_ACCOUNT_BITCOIN_GET_KEY_UUID, m_PerGetBitcoinKeyUUID);

        std::ostringstream jsonDocBuffer;
        write_json(jsonDocBuffer, pt, false);

        m_AbcApp->store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername), jsonDocBuffer.str(), m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking, StoreCouchbaseDocumentByKeyRequest::SaveOutputCasMode);
        m_AbcApp->m_WhatTheStoreWithInputCasSavingOutputCasWasFor = AnonymousBitcoinComputingWtGUI::USERACCOUNTBITCOINLOCKEDINTOGETTINGKEYMODE;
    }
    else
    {
        //TODOreq (done?): already locked, so jump straight into post-locked code (TODOreq: we use the get'd cas from when we detected it was locked(and accept fail cas-swap obviously))
        userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
    }
}
void AddFundsAccountTabBody::userAccountBitcoinLockedIntoGettingKeyAttemptComplete(u_int64_t casFromLockSoWeCanSafelyUnlockLater, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "userAccountBitcoinLockedIntoGettingKeyAttemptComplete db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText("Please only use one browser tab/window at a time. Try again", this); //TODOoptional: make this error the same in all places i use it via define
        m_AbcApp->resumeRendering();
        return;
    }

    m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking = casFromLockSoWeCanSafelyUnlockLater;

    //using a method here because recovery jumps directly to it
    userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
}
void AddFundsAccountTabBody::userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY()
{
    //get bitcoinKeySet[setN]_PageY
    //TODOoptimization: here, and when getting pageZ (same thing but for RANGES), we should be doing exponential backoff of getting this doc. the subsequent STORE (CAS-swap in both of these cases) should not have any waiting (otherwise that increases the chances that the cas has changed and we have to loop again), the waiting must go before the GET'ing or else i did it wrong. this is different from how the db is already handling exponential backoffs already because what we want to exponentially back off FROM (cas swap failure) is not flagged in the db backend as eligible (and rightly so, since the db has no concept of 'key claiming' and 'key range claiming', which must be done with the GET before the Store/CAS-swap. A random TODOoptimization that will cause more confusion i bet: the db could use a callback in between the get and the store to allow the business layer to do the modifications needed for the store to make any sense, but i'm not sure that optimization fits abc2 specifically (since wt uses a thread pool etc)
    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(bitcoinKeySetPageKey(m_BitcoinKeySetIndex_aka_setN, m_BitcoinKeySetPage_aka_PageY));
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETBITCOINKEYSETNACTUALPAGETOSEEIFUUIDONITENOUGHROOM;
}
void AddFundsAccountTabBody::gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(const string &bitcoinKeySetNpageY, u_int64_t bitcoinKeySetNpageY_CAS, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "TOTAL SYSTEM FAILURE: lcbOpFail: bitcoinKeySetNpageY was supposed to exist but didn't in gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc" << endl;

        m_AbcApp->resumeRendering();
        return;
    }

    //got bitcoinKeySetNpageY
    ptree pt;
    std::istringstream is(bitcoinKeySetNpageY);
    read_json(is, pt);

    bool foundPerKeyRequestUuidOnThisPage = false;
    bool doneInsertingIntoFirstUnclaimed = false; //we insert on first one we find, BUT we might discard the json if we 'detect' the uuid later (unlikely)

    for(int i = 0; i < ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE; ++i)
    {
        const std::string keyPrefix = JSON_BITCOIN_KEY_SET_KEY_PREFIX + boost::lexical_cast<std::string>(i);
        const std::string &bitcoinKey = pt.get<std::string>(keyPrefix); //yes, key is get'd by only the prefix (english teachers are raging)
        const std::string &claimedUuid = pt.get<std::string>(keyPrefix + JSON_BITCOIN_KEY_SET_CLAIMED_UUID_SUFFIX, "n");
        //we don't care about GET'ing the claimedUsername (TO DOnereq: be sure to pt::put it (is just good book keeping imo, since the uuid will vanish later)
        if(claimedUuid != "n") //claimed, check uuid
        {
            if(claimedUuid == m_PerGetBitcoinKeyUUID)
            {
                m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage = bitcoinKey;
                foundPerKeyRequestUuidOnThisPage = true;
                break;
            }
        }
        else if(!doneInsertingIntoFirstUnclaimed) //unclaimed spot found! but we might not need it if the uuid is seen during this iteration
        {
            m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage = bitcoinKey;
            pt.put(keyPrefix + JSON_BITCOIN_KEY_SET_CLAIMED_UUID_SUFFIX, m_PerGetBitcoinKeyUUID);
            pt.put(keyPrefix + JSON_BITCOIN_KEY_SET_CLAIMED_USERNAME_SUFFIX, m_AbcApp->m_CurrentlyLoggedInUsername);
            doneInsertingIntoFirstUnclaimed = true;
            break;
        }
    }

    if(foundPerKeyRequestUuidOnThisPage)
    {
        //woot the uuid is matched up with a bitcoin key, now present it to the user

        unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt();
    }
    else //uuid not found
    {
        if(doneInsertingIntoFirstUnclaimed)
        {
            //space left (and we already added outselves claiming one of the keys, but that doc hasn't yet been cas-swap-stored)

            //jump to unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt if the cas (the m_BitcoinKeySetNpageY_CAS_ForClaimingAkey one below, NOT the cas mentioned in the method name) swap succeeds

            std::ostringstream jsonDocBuffer;
            write_json(jsonDocBuffer, pt, false);

            m_AbcApp->storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(bitcoinKeySetPageKey(m_BitcoinKeySetIndex_aka_setN, m_BitcoinKeySetPage_aka_PageY), jsonDocBuffer.str(), bitcoinKeySetNpageY_CAS, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::CLAIMBITCOINKEYONBITCOINKEYSETVIACASSWAP;
        }
        else
        {
            //no space left
            //get next page (fill if not exist, check uuid/space loop if exist)

            //PageY++
            m_BitcoinKeySetPage_aka_PageY = boost::lexical_cast<std::string>(boost::lexical_cast<int>(m_BitcoinKeySetPage_aka_PageY)+1);
            m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(bitcoinKeySetPageKey(m_BitcoinKeySetIndex_aka_setN, m_BitcoinKeySetPage_aka_PageY));
            m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETBITCOINKEYSETNPAGEYANDIFITEXISTSLOOPAROUNDCHECKINGUUIDBUTIFNOTEXISTMAKEITEXISTBITCH;
        }
    }
}
void AddFundsAccountTabBody::doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneAttemptingToUpdateBitcoinKeySetViaCASswapAkaClaimingAKey db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //try again (but a neighbor might have succeeded, so check UUIDs after re-getting (and of course, make sure enough space))
        userAccountBitcoinGettingKeyLocked_So_GetSavingCASbitcoinKeySetNPageY();
    }
    else //cas swap succeeded, no need to re-get it
    {
        //jump to dat spot that does user account unlocking (GettingKey -> HaveKey)
        unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt();
    }
}
void AddFundsAccountTabBody::unlockUserAccountSafelyFromBitcoinGettingKeyBecauseShitWeGotAfuckingKey_NoSweatIfBeatenToIt()
{
    ptree pt;
    std::istringstream is(m_UserAccountJsonForLockingIntoGettingBitcoinKey);
    read_json(is, pt);

    pt.put(JSON_USER_ACCOUNT_BITCOIN_STATE, JSON_USER_ACCOUNT_BITCOIN_STATE_HAVE_KEY); //TODOreq: this would be a good 'last resort' place to check if "slotToAttemptToFillAkaPurchase" is set and to fail/stop if it is... but it really should be checked earlier. AND besides there is also teh 'account locking into GettingKey' which would be a similar 'last resort' (two of em xD) to check for it. Depending on how confident you are that you checked them in the original GET...
    pt.put(JSON_USER_ACCOUNT_BITCOIN_STATE_DATA, m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage);

    //pt::erase the extra "GettingKey" fields
    pt.erase(JSON_USER_ACCOUNT_BITCOIN_SET_PAGE);
    pt.erase(JSON_USER_ACCOUNT_BITCOIN_GET_KEY_UUID);

    std::ostringstream userAccountUnlockedToHaveKeyModeJsonDocBuffer;
    write_json(userAccountUnlockedToHaveKeyModeJsonDocBuffer, pt, false);

    m_AbcApp->store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername), userAccountUnlockedToHaveKeyModeJsonDocBuffer.str(), m_UserAccountCASforBitcoinGettingKeyLockingAndUnlocking, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::UNLOCKUSERACCOUNTFROMBITCOINGETTINGKEYTOBITCOINHAVEKEY;
}
void AddFundsAccountTabBody::doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneAttemptingToUnlockUserAccountFromBitcoinGettingKeyToBitcoinHaveKey db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //could have been a slot fill (takes presedence) or a recovery neighbor simply beat us
        new WBreak(this);
        new WText("Are you using multiple browser tabs/windows? Don't do that. Retry getting the key", this); //TODOreq: i guess in this case we DON'T want to hide "Get Key" button? Or we re-enable it or idfk...
        m_AbcApp->resumeRendering();
        return;
    }

    //GettingKey -> HaveKey transition completed successfully, present key to user
    presentBitcoinKeyForPaymentsToUser(m_BitcoinKeyToGiveToUserOncePerKeyRequestUuidIsOnABitcoinKeySetPage);
    m_AbcApp->resumeRendering();
}
void AddFundsAccountTabBody::presentBitcoinKeyForPaymentsToUser(const string &bitcoinKey)
{
    m_CurrentBitcoinKeyForPayments = bitcoinKey;
    new WBreak(this);
    new WText("Current Bitcoin Key: " + bitcoinKey, this);

    new WBreak(this);
    WPushButton *checkForPendingButton = new WPushButton("Check For Pending Bitcoins", this); //changes to "Check For Pending Again" after > 0 pending seen. //TODOreq: 2 keys in same session needs to not remake this (and similar) buttons
    checkForPendingButton->clicked().connect(this, &AddFundsAccountTabBody::checkForPendingBitcoinBalanceButtonClicked);

    //check for pending and check for confirmed don't modify the BitcoinState in the user-account, so that means they are safe to perform even when slotToAttemptToFillAkaPurchase is set
}
void AddFundsAccountTabBody::checkForPendingBitcoinBalanceButtonClicked()
{
    m_BitcoinAddressBalancePollerPollingPendingBalance = true;
    sendRpcJsonBalanceRequestToBitcoinD();

    //TODOreq: doesn't belong here, but later in this code path after the "Done With This Key" button is pressed, it is IMPERITIVE that we check that slotToAttemptToFillAkaPurchase isn't set on the user-account before transitioning from BitcoinState:HaveKey to BitcoinState:NoKey (creditting account at same time). The GET for that CAS-swap is where we should make sure slotToAttemptToFillAkaPurchase is not set (if it is, fail saying try again later or stop using multiple tabs noob)
}
void AddFundsAccountTabBody::checkForConfirmedBitcoinBalanceButtonClicked()
{
    m_BitcoinAddressBalancePollerPollingPendingBalance = false;
    sendRpcJsonBalanceRequestToBitcoinD();
}
void AddFundsAccountTabBody::doneSendingBitcoinsToCurrentAddressButtonClicked()
{
    //cas-swap creditting user-account, but TODOreq: fail and say try again in a few moments _IF_ slotToAttemptToFill[...] is present (ie., the account is locked). Set bitcoinState back to NoKey during creditting cas-swap

    //TODOreq: (had:i am unsure if it makes sense to) first check that bitcoinState is even at "HaveKey" and that the key we have in the member variable is accurate. A tiny hunch (probably paranoia) tells me they could maybe credit the amount twice if we don't perform that verification... perhaps using two tabs or fuck I don't know (the cas swap should protect us from this, what am I on about (NO WAIT I AM RIGHT HAHA: two (or 'N' holy shit huuuuuge bug just squashed) sessions/tabs, walk both through pending->confirmed process, 'done' on first session, it completes, 'done' on respective sessions, they complete. We should DEFINITELY verify that bitcoinState is HaveKey and that the key is accurate!!! We should detect the double(N)-credit and laugh at them :-P. It is NOT a cas-swap-fail, but is an inaccurate bitcoinState or bitcoinStateData(key)))

    m_AbcApp->deferRendering();

    //get user-account for cas-swap-credit
    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::CREDITCONFIRMEDBITCOINAMOUNTAFTERANALYZINGUSERACCOUNT;
}
void AddFundsAccountTabBody::sendRpcJsonBalanceRequestToBitcoinD()
{
    //TODOreq: prevent DDOS of checking balance, since I _think_ it's an expensive operation. A simple 1-check-per-X-seconds would suffice, and there is also the design where I poll 'getblockcount' every few seconds and then only even bother doing a minconf=1 balance check when the block count changes
    if(!m_BitcoinAddressBalancePoller)
    {
        m_BitcoinAddressBalancePoller = new Wt::Http::Client(this);
        m_BitcoinAddressBalancePoller->setTimeout(30);
        m_BitcoinAddressBalancePoller->setMaximumResponseSize(2048);
        m_BitcoinAddressBalancePoller->done().connect(boost::bind(&AddFundsAccountTabBody::handleBitcoinAddressBalancePollerReceivedResponse, this, _1, _2));
    }
    Wt::Http::Message bitcoinJsonRpcMessage;
    bitcoinJsonRpcMessage.addHeader("content-type", "text/plain");
    bitcoinJsonRpcMessage.addBodyText("{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getreceivedbyaddress\", \"params\": [\"" + m_CurrentBitcoinKeyForPayments + "\", " + (m_BitcoinAddressBalancePollerPollingPendingBalance ? "0" : "1" ) + "] }");

    if(m_BitcoinAddressBalancePoller->post("http://admin2:123@127.0.0.1:19011/", bitcoinJsonRpcMessage))
    {
        m_AbcApp->deferRendering();
    }
    else
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "(Wt::Http::Client)m_BitcoinAddressBalancePoller->post returned false, malformed url?" << endl;
    }
}
void AddFundsAccountTabBody::handleBitcoinAddressBalancePollerReceivedResponse(boost::system::error_code err, const Http::Message &response)
{
    m_AbcApp->resumeRendering();
    if (!err && response.status() == 200)
    {
        std::istringstream is(response.body());
        ptree pt;
        read_json(is, pt);
        if(pt.get<std::string>("error") != "null")
        {
            new WBreak(this);
            new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
            cerr << "error in json returned handleBitcoinAddressBalancePollerReceivedResponse: " << response.body() << endl;
            return;
        }
        string balanceString = pt.get<std::string>("result");
        SatoshiInt balance = jsonStringToSatoshiInt(balanceString);
        if(balance > 0)
        {
            if(m_BitcoinAddressBalancePollerPollingPendingBalance)
            {
                if(!m_PendingBitcoinBalanceLabel)
                {
                    new WBreak(this);
                    m_PendingBitcoinBalanceLabel = new WText(this);

                    //enable 'check for confirmed'
                    new WBreak(this);
                    WPushButton *checkForConfirmedPushButton = new WPushButton("Check For Confirmed Bitcoins", this);
                    checkForConfirmedPushButton->clicked().connect(this, &AddFundsAccountTabBody::checkForConfirmedBitcoinBalanceButtonClicked);
                }
                //display pending balance
                m_PendingBitcoinBalanceLabel->setText("Pending Bitcoins Received: " + balanceString);
            }
            else
            {
                if(!m_ConfirmedBitcoinBalanceLabel)
                {
                    new WBreak(this);
                    m_ConfirmedBitcoinBalanceLabel = new WText(this);

                    //enable 'done with this key'
                    WPushButton *doneSendingBitcoinsButton = new WPushButton("Done Sending Bitcoins To This Address (Add To My Account)", this); //TODOreq: explain more thoroughly to user that they can not send bitcoins to that address anymore after clicking this. Give a huge warning, like a js 'alert' except not because fuck js
                    doneSendingBitcoinsButton->clicked().connect(this, &AddFundsAccountTabBody::doneSendingBitcoinsToCurrentAddressButtonClicked);
                }
                //display confirmed balance
                m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked = balance;
                m_ConfirmedBitcoinBalanceLabel->setText("Confirmed Bitcoins Received: " + balanceString);
            }
        }
        else
        {
            if(m_BitcoinAddressBalancePollerPollingPendingBalance)
            {
                new WBreak(this);
                new WText("No pending bitcoins received yet. Try again in a minute or so. You did send them, right ;-P?", this);
            }
            else
            {
                new WBreak(this);
                new WText("It takes roughly 10 minutes for pending bitcoins to become confirmed. Try again when the next bitcoin block is mined.", this);
            }
        }
    }
    else
    {
        //TODOreq: handle (retry? what?). lots of reasons it could fail, simple timeout for example
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "bitcoin poller did not return HTTP 200 OK" << endl << "http status: " << response.status() << endl << "http body: " << response.body() << endl;
    }
}
void AddFundsAccountTabBody::creditConfirmedBitcoinAmountAfterAnalyzingUserAccount(const string &userAccountJsonDoc, u_int64_t userAccountCAS, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "creditConfirmedBitcoinAmountAfterAnalyzingUserAccount db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "TOTAL SYSTEM FAILURE: user account didn't exist and must. creditConfirmedBitcoinAmountAfterAnalyzingUserAccount" << endl;

        m_AbcApp->resumeRendering();
        return;
    }

    //got user-account doc

    std::istringstream is(userAccountJsonDoc);
    ptree pt;
    read_json(is, pt);

    string slotToAttemptToFillAkaPurchase_LOCKED_CHECK = pt.get<std::string>(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, "n");
    if(slotToAttemptToFillAkaPurchase_LOCKED_CHECK != "n")
    {
        new WBreak(this);
        new WText("Please try again in a few moments", this);
        m_AbcApp->resumeRendering();
        return;
    }

    //check that they're not trying to credit the amount multiple times
    string bitcoinState = pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_STATE);
    string bitcoinStateData = pt.get<std::string>(JSON_USER_ACCOUNT_BITCOIN_STATE_DATA, "n"); //key ("n" because they could segfault us by doing the hack we're about to check for)
    if(bitcoinState != JSON_USER_ACCOUNT_BITCOIN_STATE_HAVE_KEY || bitcoinStateData != m_CurrentBitcoinKeyForPayments)
    {
        new WBreak(this);
        new WText("Nice try, you already creditted those bitcoins in another tab/window ;-)", this); //TODOreq: no-js allows you to use the same session in multiple windows by copy/pasting sessionId, but what are the implications for that with the state of the web-app. Can they be viewing different pages? Don't they share member variables? I can't exactly put my finger on it, but I think that this hack still might be vulnerable. Something like this: tab1 is at 'done' step, just before clicking (and getting here). tab2 then steals sessionId and (???). Still not sure what they could do (change the key? idfk. don't think they could even do that...), BUT the paranoid thought wonders whether or not tab1 can still click 'done' or not. i also wrote a TODOreq about this up towards the top, relating to when the fuck a slot is not longer activatable (idk lol)

        m_AbcApp->resumeRendering();
        return;
    }

    SatoshiInt userBalance = satoshiStringToSatoshiInt(pt.get<std::string>(JSON_USER_ACCOUNT_BALANCE));
    userBalance += m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked;

    std::string userBalanceInSatoshisString = satoshiIntToSatoshiString(userBalance);
    m_AbcApp->m_CurrentlyLoggedInUsersBalanceStringForDisplayingOnly = userBalanceInSatoshisString;

    pt.put(JSON_USER_ACCOUNT_BALANCE, userBalanceInSatoshisString);
    pt.put(JSON_USER_ACCOUNT_BITCOIN_STATE, JSON_USER_ACCOUNT_BITCOIN_STATE_NO_KEY);
    pt.erase(JSON_USER_ACCOUNT_BITCOIN_STATE_DATA);

    std::ostringstream userAccountWithConfirmedBitcoinAmountCredittedJsonBuffer;
    write_json(userAccountWithConfirmedBitcoinAmountCredittedJsonBuffer, pt, false);

    m_AbcApp->store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername), userAccountWithConfirmedBitcoinAmountCredittedJsonBuffer.str(), userAccountCAS, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::CREDITCONFIRMEDBITCOINBALANCEFORCURRENTPAYMENTKEYCASSWAPUSERACCOUNT;
}
void AddFundsAccountTabBody::attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete(u_int64_t casFromLockSoWeCanSafelyUnlockAfterNextPageFillComplete, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "attemptToLockBitcoinKeySetNintoFillingNextPageModeComplete db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //another neighbor/user (or maybe our own recovery alternate tab) beat us to it, no biggy
        new WBreak(this);
        new WText("A recoverable error occured, please try again in a few moments", this);
        m_AbcApp->resumeRendering();
        return;
    }

    m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater = casFromLockSoWeCanSafelyUnlockAfterNextPageFillComplete;

    //bitcoinKeySet[setN]_CurrentPage now locked, so do the actual next page creation
    bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation(); //method because it is also recovery jump point
}
void AddFundsAccountTabBody::bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation()
{
    //this method expects m_FillingNextBitcoinKeySetPage_ForAfterCASswapLockSucceeds, m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds, and m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList to be accurate before calling

    //Get hugeBitcoinKeyList[pageZ] attempt in order to retrieve it's CAS, check for relevant-UUID, and see if room left
    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(hugeBitcoinKeyListPageKey(m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList));
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETHUGEBITCOINKEYLISTACTUALPAGEFORANALYZINGANDMAYBECLAIMINGKEYRANGE;
}
void AddFundsAccountTabBody::doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneAttemptingCredittingConfirmedBitcoinBalanceForCurrentPaymentKeyCasSwapUserAccount db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //TODOreq: loop back around to the same get/get-cas (with exponential backoff) and store-with-cas (getting back to here), or give error message saying try again in a few moments? Going with safer option of retry momentarily message for now... but I have a hunch it is fine and dandy to just go right back to the getSavingCas (re-using the enum too!) because strict sanitization is done there
        new WBreak(this);
        new WText("Please try again in a few moments", this);
        m_AbcApp->resumeRendering();
        return;
    }

    //successful credit of user-account balance && changing of bitcoinState back to "NoKey" :-D

    new WBreak(this);
    new WText("Your account has been creditted: BTC " + satoshiIntToJsonString(m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked) + ". Do NOT send any more bitcoins to address: " + m_CurrentBitcoinKeyForPayments, this); //TODOreq: enable "get bitcoin key" button again, make check-for-pending/confirmed/done buttons go away (delete?).
    m_AbcApp->m_CurrentlyLoggedInUsersBalanceForDisplayOnlyLabel->setText(satoshiStringToJsonString(m_AbcApp->m_CurrentlyLoggedInUsersBalanceStringForDisplayingOnly));

    m_ConfirmedBitcoinBalanceToBeCredittedWhenDoneButtonClicked = 0; //probably pointless, but will make me sleep better at night
    m_CurrentBitcoinKeyForPayments = ""; //ditto

    m_AbcApp->resumeRendering();
}
void AddFundsAccountTabBody::showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout()
{
    //TO DOnereq(yes): so the set user-account and bitcoin key set both stay locked until the manual filling of more keys? methinks yes, but still seems strange from this point of view... and i was considering saying "no unlock em!" initially (it probably wouldn't matter, but keeping them locked is an optimization (and laziness wins :-P). just need to make sure it's ok that they remain locked is the point

    //TODOreq: automated/machine notification
    new WBreak(this);
    new WText("The server ran out of bitcoin keys and requires a manual refill by the administrator. If you are within shouting distance of him (or her, but lol there are no girls on the internet), say this: \"OOGA BOOGA I AM THE HUGE BITCOIN KEY REFILL NOTIFICATION OUTSOURCED TO ONE OF YOUR SLAVE COCKSUCKERS\". I'll know what it means.", this);
    m_AbcApp->resumeRendering();
}
void AddFundsAccountTabBody::getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont(const string &bitcoinKeySetNpageY_orNot, u_int64_t bitcoinKeySetNpageY_CAS_orNot, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "getBitcoinKeySetNPageYAttemptFinishedSoCheckItIfItExistsAndMakeItIfItDont db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //or not ;-)

        //fill next page
        //Get hugeBitcoinKeyList_CurrentPage to see what page it's on
        m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(hugeBitcoinKeyListCurrentPageKey());
        m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETHUGEBITCOINKEYLISTCURRENTPAGE;
    }
    else
    {
        //race condition
        //loop around to checking UUID/space-left, and possibly even getting here again <3
        gotBitcoinKeySetNpageYSoAnalyzeItForUUIDandEnoughRoomEtc(bitcoinKeySetNpageY_orNot, bitcoinKeySetNpageY_CAS_orNot, true, false);
    }
}
void AddFundsAccountTabBody::getHugeBitcoinKeyListActualPageAttemptCompleted(const string &hugeBitcoinKeyListActualPage, u_int64_t casForUsingInSafelyInsertingOurPerFillUuid, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "getHugeBitcoinKeyListActualPageAttemptCompleted db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //re-lock 'filling from page Z' in the db pointing to this non-existent page, to ensure the OLD/full page isn't pulled like crazy which would likely make the nodes run out of memory
        if(m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING != m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList) //hack but fuck it
        {
            ptree pt2;

            //TODOreq: not sure it's safe to re-make this doc for re-locking it with new Z using these member variables (stale/inaccurate maybe???), but my instincts tell me it is safe and they are accurate. Still, so many fucking recovery paths and shit I'm a tad worried...
            pt2.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE, m_BitcoinKeySetPage_aka_PageY);
            pt2.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_UUID_PER_FILL, m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds);
            pt2.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_START_LOOKING_ON_HUGEBITCOINKEYLIST_PAGE, m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList);

            std::ostringstream reLockedToNewPageZJsonBuffer;
            write_json(reLockedToNewPageZJsonBuffer, pt2, false);

            m_AbcApp->storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(bitcoinKeySetCurrentPageKey(m_BitcoinKeySetIndex_aka_setN), reLockedToNewPageZJsonBuffer.str(), m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::RELOCKBITCOINKEYSETN_CURRENTPAGETONONEXISTENTPAGEZASNECESSARYOPTIMIZATION;

            //displays the error (as in the else) after the op completes
        }
        else
        {
            //just display the error
            showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout();
        }
        return; //huge error to go on
    }

    //got hugeBitcoinKeyListActualPage
    ptree pt;
    std::istringstream is(hugeBitcoinKeyListActualPage);
    read_json(is, pt);

    bool foundPerRefillUuidOnThisPage = false;
    bool doneInsertingIntoFirstUnclaimed = false; //we insert on first one we find, BUT we might discard the json if we 'detect' the uuid later (unlikely)

    for(int i = 0; i < ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE; ++i) //100 key ranges of 100 keys each, so 10k keys per hugeBitcoinKeyList page (~350kb/page)
    {
        const std::string keyPrefix = JSON_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_RANGE_PREFIX + boost::lexical_cast<std::string>(i);
        const std::string &claimedUuid = pt.get<std::string>(keyPrefix + JSON_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_RANGE_CLAIMED_UUID_SUFFIX, "n");
        //we don't care about GET'ing the claimedSetN (or PageY if we decide to utilize it)

        if(claimedUuid != "n") //claimed, check uuid
        {
            if(claimedUuid == m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds)
            {
                //copy-paste'd below
                string commaSeparatedKeyRangeFromHugeBitcoinKeyList = pt.get<std::string>(keyPrefix);
                m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.clear();
                boost::split(m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList,commaSeparatedKeyRangeFromHugeBitcoinKeyList,boost::is_any_of(","));

                foundPerRefillUuidOnThisPage = true;
                break;
            }
        }
        else if(!doneInsertingIntoFirstUnclaimed) //unclaimed spot found! but we might not need it if the uuid is seen during this iteration (but it would have been found already)
        {
            //copy-pasted from above
            string commaSeparatedKeyRangeFromHugeBitcoinKeyList = pt.get<std::string>(keyPrefix);
            m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.clear();
            boost::split(m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList,commaSeparatedKeyRangeFromHugeBitcoinKeyList,boost::is_any_of(","));

            pt.put(keyPrefix + JSON_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_RANGE_CLAIMED_UUID_SUFFIX, m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds);

            doneInsertingIntoFirstUnclaimed = true;
            break;
        }
    }

    if(foundPerRefillUuidOnThisPage)
    {
        //woot the uuid is matched up with a bitcoin key range, now use the key range for the next page fill
        uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill(); //method because we jump directly to it when the cas-swap below succeeds
    }
    else //uuid not found
    {
        if(doneInsertingIntoFirstUnclaimed)
        {
            //space left (and we already added outselves claiming one of the key ranges, but that doc hasn't yet been cas-swap-stored)

            std::ostringstream jsonDocBuffer;
            write_json(jsonDocBuffer, pt, false);

            //CAS-swap attempt hugeBitcoinKeyList[pageZ] claiming a range for the next bitcoinKeySet[setN]_PageY
            m_AbcApp->storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(hugeBitcoinKeyListPageKey(m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList), jsonDocBuffer.str(), casForUsingInSafelyInsertingOurPerFillUuid, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
            m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::HUGEBITCOINKEYLISTKEYRANGECLAIMATTEMPT;
        }
        else
        {
            //no space left, but there might be a next page

            //TODOreq: regarding what i was wondering with whether the hugeBitcoinKeyList_CurrentPage increment should be a cas-swap or just a plane set/store, I am now leaning towards cas swap because how do we know that it isn't now on the page after the one we are setting it to? It probably doesn't matter though because I think it'd just get set'd again after realizing that the page in between is full. I am unsure tbh, but keep this in mind: recovery needs to iterate all the pages between where it was locked to and the current page (might not even be relevant).

            //++PageZ;
            m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList = boost::lexical_cast<std::string>(boost::lexical_cast<int>(m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList)+1);

            if(m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont)
            {
                //set CurrentPage, then go to the method directly called in the else (even if cas swap fails? methinks yes)
                ptree pt2;
                pt2.put(JSON_HUGE_BITCOIN_KEY_LIST_CURRENT_PAGE_KEY, m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList);
                std::ostringstream jsonDocBuffer;
                write_json(jsonDocBuffer, pt2, false);
                m_AbcApp->storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(hugeBitcoinKeyListCurrentPageKey(), jsonDocBuffer.str(), m_CasToUseForSafelyUpdatingHugeBitcoinKeyList_CurrentPageUsingIncrementedPageZ, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
                m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::HUGEBITCOINKEYLISTPAGECHANGE;
            }
            else
            {
                bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
                //old/error: proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
            }
        }
    }
}
void AddFundsAccountTabBody::uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill()
{
    //create bitcoinKeySet[setN] Page Y using the range obtained from hugeBitcoinKeyList[pageZ] via an LCB_ADD-accepting fail
    ptree pt;
    int bitcoinKeysVectorSize = static_cast<int>(m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.size());
    for(int i = 0; i < bitcoinKeysVectorSize; ++i)
    {
        pt.put(JSON_BITCOIN_KEY_SET_KEY_PREFIX + boost::lexical_cast<std::string>(i), m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList[i]); //A stupid but actual TODOoptimization (don't do it) is to claim our key now.. would save us from doing it later. But nah suicide/insanity doesn't sound that fun, I'll pass...
    }

    std::ostringstream bitcoinKeySetPageJsonBuffer;
    write_json(bitcoinKeySetPageJsonBuffer, pt, false);

    m_AbcApp->storeLarge_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(bitcoinKeySetPageKey(m_BitcoinKeySetIndex_aka_setN, m_BitcoinKeySetPage_aka_PageY), bitcoinKeySetPageJsonBuffer.str(), StoreCouchbaseDocumentByKeyRequest::AddMode);
    m_AbcApp->m_WhatTheStoreWithoutInputCasWasFor = AnonymousBitcoinComputingWtGUI::BITCOINKEYSETNPAGEYCREATIONVIALCBADD;

    m_BitcoinKeysVectorToUseForNextPageFillOncePerFillUuidIsSeenOnHugeBitcoinKeyList.clear();
}
void AddFundsAccountTabBody::hugeBitcoinKeyListCurrentPageGetComplete(const string &hugeBitcoinKeyList_CurrentPageJson, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "hugeBitcoinKeyListCurrentPageGetComplete db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "TOTAL SYSTEM FAILURE hugeBitcoinKeyList_CurrentPage didn't exist and must" << endl;

        m_AbcApp->resumeRendering();
        return;
    }

    //got hugeBitcoinKeyList_CurrentPage
    //this code path knows that it isn't already locked, so Attempt: bitcoinKeySet[setN]_CurrentPage CAS-swap-locked into "fillingNextBitcoin with UUID-per-fill on pageZ of hugeBitcoinKeyList" mode

    m_PageZisFromCurrentPageInDbSoCasSwapIncremementCurrentPageWhenIncrementingPageZ_AsOpposedToPageZbeingSeenFromLockedRecoveryInWhichCaseDont = true;
    m_CasToUseForSafelyUpdatingHugeBitcoinKeyList_CurrentPageUsingIncrementedPageZ = cas;

    ptree pt;
    std::istringstream is(hugeBitcoinKeyList_CurrentPageJson);
    read_json(is, pt);

    //remember the values put into the cas-swap, so we have them handy if/when the cas-swap succeeds
    m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds = m_BitcoinKeySetIndex_aka_setN + "_" + m_BitcoinKeySetPage_aka_PageY;
    m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList = pt.get<std::string>(JSON_HUGE_BITCOIN_KEY_LIST_CURRENT_PAGE_KEY);
    m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList_NON_CHANGING = m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList;

    //now put them into json
    ptree pt2;
    pt2.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE, m_BitcoinKeySetPage_aka_PageY);
    pt2.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_UUID_PER_FILL, m_FillingNextBitcoinKeySetPerFillUuid_ForAfterCASswapLockSucceeds);
    pt2.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_START_LOOKING_ON_HUGEBITCOINKEYLIST_PAGE, m_FillingNextBitcoinKeySetStartingFromPageZofHugeBitcoinList); //TODOreq: recovering from this doesn't segfault etc when that pageZ doesn't exist
    std::ostringstream jsonDocBuffer;
    write_json(jsonDocBuffer, pt2, false);

    m_AbcApp->storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(bitcoinKeySetCurrentPageKey(m_BitcoinKeySetIndex_aka_setN), jsonDocBuffer.str(), m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater, StoreCouchbaseDocumentByKeyRequest::SaveOutputCasMode);
    m_AbcApp->m_WhatTheStoreWithInputCasSavingOutputCasWasFor = AnonymousBitcoinComputingWtGUI::LOCKINGBITCOINKEYSETNINTOFILLINGNEXTPAGEMODE;
}
void AddFundsAccountTabBody::doneAttemptingBitcoinKeySetNnextPageYcreation(bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneAttemptingBitcoinKeySetNnextPageYcreation reported db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    /*if(!lcbOpSuccess)
    {
        //dgaf: recovery/neighbor-user would have made the exact same page (enforced by UUIDs)
    }*/

    //bitcoinKeySet[setN]_PageY now created

    //CAS-swap-unlock bitcoinKeySet[setN]_CurrentPage into normal 'current page = Y' mode (fail is tolerated, just like this add failing was)

    //TODOreq: the cas used for unlocking needs to be stored when doing recovery and seeing it's locked, and in the normal case when the lock that we just did completes. two different places/times

    ptree pt;
    pt.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE, m_BitcoinKeySetPage_aka_PageY);

    std::ostringstream unlockedCurrentPageDoc;
    write_json(unlockedCurrentPageDoc, pt, false);

    m_AbcApp->storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(bitcoinKeySetCurrentPageKey(m_BitcoinKeySetIndex_aka_setN), unlockedCurrentPageDoc.str(), m_BitcoinKeySetCurrentPageCASForFillingNextPageWayLater, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::SAFELYUNLOCKBITCOINKEYSETNCURRENTPAGEDOCFAILINGTOLERABLY;
}
void AddFundsAccountTabBody::doneAttemptingHugeBitcoinKeyListKeyRangeClaim(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneAttemptingHugeBitcoinKeyListKeyRangeClaim db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //jump back to the pre-get code path
        bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    }
    else
    {
        //cas swap succeeded, so we know our uuid got in
        uuidPerRefillIsSeenOnHugeBitcoinListSoProceedWithActualNextPageFill();
    }
}
void AddFundsAccountTabBody::doneAttemptingToUnlockBitcoinKeySetN_CurrentPage(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneAttemptingToUnlockBitcoinKeySetN_CurrentPage db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        //since unlock failed, jump to a point that probes the lock status (might be unlocked by neighbor, but might be locked to NEXT one now in a rare race condition)
        m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(bitcoinKeySetCurrentPageKey(m_BitcoinKeySetIndex_aka_setN)); //this 'get' and enum is copy/paste from the end of checkNotAttemptingToFillAkaPurchaseSlotThenTransitionIntoGettingBitcoinKeyState
        m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETBITCOINKEYSETNCURRENTPAGETOSEEWHATPAGEITISONANDIFITISLOCKED;

        //OLD:
        //we COULD have detected that a neighbor (not necessarily recovery) beat us to the punch here, BUT it would have been erroneous to have used such detection as a signal that we can stop. The page fill request could have been filled by any other users/wt-nodes doing get-key requests. Just because we got beat to the page-fill, doesn't mean that we got beat by someone processing the same per-key-request-uuid as us... SO we need to continue on, even if yes in fact it was hte same per-key-request-uuid (we'll detect that later (TODOreq) and then stop
    }
    else
    {
        //jump out of page filling mode and to a point where fillingNextPage is not set, but we don't know if we're in recovery or not so where we land will check that and act accordingly
        proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
    }
}
void AddFundsAccountTabBody::doneChangingHugeBitcoinKeyListCurrentPage(bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneChangingHugeBitcoinKeyListCurrentPage db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    //lcbOpFail very likely, means nothing. in fact we don't even need to wait for this operation to complete, but i have no way of saying 'dont-respond' in my wt->couchbase communication, so fuck it :). there are probably tons of other cases wehre i don't need to wait, but KISS (sanity > *)
    bitcoinKeySetN_currentPage_Locked_soDoKeyRangeClaimAndNextPageCreation();
    //old/error: proceedToBitcoinKeySetNgettingAfterLockingUserAccountInto_GetAkeyFromPageYofSetNusingUuidPerKeyRequest_UnlessUserAccountAlreadyLocked();
}
void AddFundsAccountTabBody::doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue(bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        //temp:
        cerr << "doneReLockingBitcoinKeySetN_CurrentPage_withNewFromPageZvalue db error" << endl;

        m_AbcApp->resumeRendering();
        return;
    }
    //lcbOpFail very likely, means nothing.

    showOutOfBitcoinKeysErrorToUserInAddFundsPlaceholderLayout();
}

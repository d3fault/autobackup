#include "abc2withdrawrequestprocessor.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <libcouchbase/http.h>

#include <QScopedPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


//The TYPICAL (non-error) flow goes like this:
//In-Abc2: click request withdraw button, check balance is enough, get withdraw index, recursively try to lcb_add to index (+1'ing as appropriate), cas swap accepting fail withdraw index
//In-payout-processor: query a view to get all [stale=false] payout requests with state=unprocessed, iterate them. Here's one payout request flow: check balance is enough, if not set state=insufficientfunds and done. else (sufficient funds), cas swap lock profile account requiring success (fail means they are buying an ad slot or adding funds or something similar, so try again later (leave state as is, continue to next request. mb say it in stdout tho)), cas swap requiring success the payout request state to 'processing' (mb with durability poll of 3 as well but guh mb not since it isn't an lcb_add). finding a payout request in state 'processing' that we didn't JUST set to that means there was previously a fail. these are dangerous as fuck because we might pay them out twice (as in, when we find a payout request in state 'processing' that we didn't just set ourselves, it may or may not have already been processed! processing AGAIN (a la recovery) would be a double pay). check bitcoin logs or idfk what and do manual ass shit to see wtf is going on. see if we paid them or what (this is why i want it to be manual). do bitcoin payout and check it's response etc (spit out errors). cas swap set payout request state to "processed, profile needs balance deducting/unlocking". deduct amount from profile balance and unlock it. TODOreq: the profile should lock TO the specific payout request (just like buy ad slots are), which will allow us to recover somehow (can't put my finger on it, but yea. I also think it helps us safeguard against a double payout [during recovery]...). then set payout request state to 'processed and done'.
//^^so we can also query a view making sure there are no payout requests in any 'intermediate' states (indicating failure): intermediate states are processing and processed-profile-needs-blanace-deducting.
//In-payout-processor STATES: unprocessed,insufficientFunds_Done,processing,bitcoindCommandReturnedError_Done,processedButProfileNeedsDeductingAndUnlocking,processed_Done
//^maybe it's a good idea to store the bitcoin key we are attempting to send money to (and the amount, and MAYBE even the datetime) in the withdraw request when the state is "processing" (and maybe processedButProfileNeedsDeductingAndUnlocking+processedDone), so that if we do find a withdraw request in state "processing" (indicating a previous fail), we have at least SOMETHING to go by to check [manually, for now] whether the payout was sent (as in, whether the bitcoin command was issued/succeeded). The key/amount/datetime will be MORE OR LESS differentiable from one to the next, but it is not perfect: carefully crafted withdraw requests (same balance, same key) that are processed 'manually but in an automated fashion' might just have the same datetime and in that case then we'd NOT deduct the amount from their balance (thus 'giving them' more money)!!! TODOreq

/*
The ERROR/previous-fail cases:
 00) Encountering a profile locked towards a withdrawal request (withdrawal request it points to in state "unprocessed") -- currently undetected, but easily resumeable TODOreq (perhaps a run not-so-periodically (1 time per hour or maybe 1 time per day) view query of any profile in locked state, at the very least to print out diagnostics for me to manually solve (but optimally, solving automatically xD). it should build a list of "eh these kinds of locks may fix themselves mementarily" (buying ad slot, among other things), then re-check those in a minute or so)
 0) Encountering a withdrawal request in state 'processing'
 1) Encountering a withdrawal request in state 'processedButProfileNeedsDeductingAndUnlocking' (profile still locked)
 2) Encountering a withdrawal request in state 'processedButProfileNeedsDeductingAndUnlocking' (profile not locked)
 3) Finding a profile locked for withdrawing, where the corresponding withdrawal request is in one of these states: (unprocessed, insufficientFundsDone, processedDone) = TOTAL SYSTEM FAILURE. This is pretty unlikely and is probably just paranoid (overchecking. it's like checking a true value isn't false) programming~

Handling the ERROR/previous-fail cases:
 0) Encountering a withdrawal request in state 'processing'
  a) If profile isn't locked towards the withdrawal request: TOTAL SYSTEM FAILURE
  b) Profile is locked towards the withdrawal request: (NOPE:"Process" and continue like normal). You should have seen this very app fail during a previous run, so you should follow your intuition AT THAT POINT (based on errors spat out) to solve the problem. In some cases (we definitely don't issue bitcoind command, or it definitely failed), we might be able to just 'process and continue like normal' (I was going to say "manually unlock the profile and set withdrawal request state back to unprocessed before re-running this app", but that's just dumb)
 1) Encountering a withdrawal request in state 'processedButProfileNeedsDeductingAndUnlocking' (profile still locked)
  a) Continue like normal at the 'debit+unlock' stage (careful to make sure all m_Members are valid before 'jumping' TODOreq, guh)
 2) Encountering a withdrawal request in state 'processedButProfileNeedsDeductingAndUnlocking' (profile not locked)
  a) Bump state to processingDone like normal
 3) Go fuck yourself

 NOTES:
 - Error case (0) Is most dangerous because it means the bitcoind command to pay out funds was PROBABLY issued. It may not have been issued, it may have been issued and [also] failed, or in the worst case (yet still very probable) scenario it may have been issued and succeeded (but we ourselves failed before we could bump the state to 'processedButProfileNeedsDeductingAndUnlocking').
 - I'm leaning towards beefing up the current view/map to also gather the error conditions, instead of having a separate view/map for them. The beefed up view/map could either 'exclude processedInsufficientFundsDone and processedDone', or 'include unprocessed, processing, and processedButProfileNeedsDeductingAndUnlocking' -- hardly a difference except future-proofing if I add more states
*/

//TO DOneoptional: if the bitcoind command responds with an error set, we could ask via the UI whether to "put withdrawal state back to unprocessed and unlock profile [without debitting]", or to "put withdrawal state in bitcoindCommandReturnedError_Done and unlock profile [without debitting]". I could then decide based on the errors on screen. It would also be a good idea to ask whether or not I should continue iterating the withdrawal requests (so 2 variants of those options just described, giving me 4 options total)

//TO DOnereq: 3% fee. Implementing it is pretty easy, but I need to understand and decide on what to do with rounding errors. My "minimum withdrawal amount" might influence rounding (unsure). Basically I need to make sure that when I calculate how much I've earned by "3% of the offline wallet's total balance", that it REALLY IS EQUAL TO the 3% fee applied in this here app (so I'm not accidentally spending money that isn't mine). If I have to decide whether to round "toward me" or "toward the customers", I should round toward me.... because Superman 3. I shouldn't/won't lie about it, but still those satoshis (had:pennies) really do add up (and the customers don't miss them because they are insignificant when viewed individually)
//TO DOnereq: user profile balance deduction includes 3% fee, amount sent to bitcoind does not include 3% fee obviously (ALL MINE MUAHAHAHA). It almost sounds backwards written that way, so put it differently: the 3% fee is taken out (via math) before the bitcoind call
//TODOreq: I'm first going to code the "query view for all unprocessed" typical non-error case, BUT in order of program execution, the checking for previously failed "processing" (and possibly 'processedStillNeedToDeduct') states needs to come before that. I should definitely spit out that there are previosuly failed withdraw requests, and NOT continue onto querying view for all unprocessed states (until those fails are handled or ignored or whatever). I think even before THAT, I should check for profiles 'locked in withdraw' mode and do error recovery -- still that should never happen since I'd be watching the app run and see it's errors be spit out (and then manually fix)
//TODOreq: for now I'm just going to "process" the requests, but unless I have some kind of "this is how much I'm about to deduct: XXX. Does this make sense?" (and then I manually make sure that that much in bitcoins have been received)... followed by "press Y or N to continue or not"... then this app may as well be completely automated...
//TODOreq: abc2 and this app both need to account for the new locking mode, AND SO DOES the pessimistic recovery app. I have no idea what the implications of that are. OT: I'm scared as fuck to modify this locking code, and yet it's pretty much the only thing standing in the way of completion (aside from some polish etc)
//TODOoptional: another way to use this app without really changing any of the code, juse using it differently than how I originally plan, is to run it hourly/etc on a cron-job and to have it send me periodic emails telling me how much more funds the payout wallet needs. Basically, automating payouts completely will likely amount to tying together this app with cron, and having a trusted/openbsd/local 'amount verifier' (it receives those 'emails' saying how much is needed, and checks that the offline receive wallet actually received that much). With math I could even extrapolate how much is predicted to be needed on a daily basis, so I can just fund "one day in advance" (assuming I like the numbers. they aren't too big and i'm leik omgwtfz) and we'd have more or less (less, by est. 1 day) the same amount of security but now fully automated/instant payouts. -- TO START OFF, i will be running this app manually because there might be errors etc that require manual intervention
//TODOreq: at app startup we should ask the bitcoin payout wallet how much it has, and keep track of how much has been paid out. even if calculate-only mode is run before execute mode is run, we still might not have enough funds in the payout wallet by the time execute mode engages (more withdrawal requests filed since then). Whenever we encounter a withdrawal request that our payout wallet cannot process, we should skip that withdrawal request, and add that withdrawal request's amount to a rolling "adjusted calculation during execution", the grand total of which is spit out at the end of execution (and then you fund the payout wallet with that amount and run execute mode again). it's worth putting in it's own sentence: not having sufficient funds in the payout wallet (not to be confused with the user profile not having sufficient funds ofc) is not a fatal error -- still might be smarter (optimization-wise) to stop then and there, but eh since we'd then run calculate-only mode, it's the same (and perhaps MORE efficient) if we just continued and tallied up how much we didn't have enough to pay out
//TODOreq: I might need another state "invalid key", but that depends on how much sanitizing I do in the front-end. Really though that state would be ANY bitcoind error. Although actually maybe not, because I want to spit out the errors, exit the app, and leave the withdrawal request in INVALID state. Still as of now I am not sanitizing the bitcoin keys nearly enough to be confident that bitcoind SHOULDN'T give me errors.
//TODOreq: bitcoindCommandReturnedError_Done, I should base64+store the error in the withdrawal request... for later analyzing
//TODOoptimization: might require raw transaction handling/crafting (and bitcoin does provide json rpc commands to help with crafting them): it might be cheaper for me to combine all the payouts into a single "transaction" with a single transaction fee, instead of many small ones as I'm currently doing. maybe that sendmany command is what i want (but what if one of the keys out of the 'many' is invalid?)??
Abc2WithdrawRequestProcessor::Abc2WithdrawRequestProcessor(QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
    , m_NetworkAccessManager(new QNetworkAccessManager(this))
{
    qRegisterMetaType<Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum>("Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum");
    connect(m_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkReplyFinished(QNetworkReply*)));
}
bool Abc2WithdrawRequestProcessor::debitCurrentWithdrawalRequestFromUsersCalculatedRollingBalance_AndReturnTrueIfWeHaveCalculatedTheyHaveSufficientFundsForThisWithdrawalRequest()
{
    SatoshiInt rollingUserBalance = m_CurrentUserBalanceInSatoshis; //if it's our first time seeing this user, we use the value from their profile as our starting point
    QString userRequestingWithdrawal = QString::fromStdString(m_CurrentUserRequestingWithdrawal);
    if(m_UserBalancesInSatoshisDuringCalculationMode.contains(userRequestingWithdrawal))
    {
        //we've seen this user before, so we already have a balance that has been debitted some, so debit THAT one some more
        rollingUserBalance = m_UserBalancesInSatoshisDuringCalculationMode.value(userRequestingWithdrawal);
    }
    //rollingUserBalance -= m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis;
    rollingUserBalance -= m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis;
    if(rollingUserBalance < 0) //our "calculate mode" insufficient funds checking
    {
        //for 'calculate mode' insufficient funds, we don't debit the amount from the user's calculated balance, nor do we add the amount to our running total. we just move onto the next withdrawal request
        //processNextWithdrawalRequestOrEmitFinishedIfNoMore
        return false;
    }
    m_UserBalancesInSatoshisDuringCalculationMode.insert(userRequestingWithdrawal, rollingUserBalance); //inserts new or overwrites/updates old
    return true;
}
void Abc2WithdrawRequestProcessor::errorOutput(const string &errorString)
{
    QString errorQString = QString::fromStdString(errorString);
    emit e(errorQString);
}
void Abc2WithdrawRequestProcessor::viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp)
{
    Q_UNUSED(request)
    Q_UNUSED(instance)
    (const_cast<Abc2WithdrawRequestProcessor*>(static_cast<const Abc2WithdrawRequestProcessor*>(cookie)))->viewQueryCompleteCallback(error, resp);
}
void Abc2WithdrawRequestProcessor::viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp)
{
    emit o("viewQueryCompleteCallback called (good)");
    if(error != LCB_SUCCESS)
    {
        emit e("httpCompleteCallback has error");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //TO DOnereq: don't trust the view query. we still must get the doc (and we have to anyways to get the CAS in order to fuck with it. so that being said, i now know i can emit(null,null) (which gives us only the key) in the view that's a WIP-in-mah-brain) and check AGAIN then that the state is 'unprocessed'. hence the name of the variable:
    //QList<QString> listOfKeysWhereViewQuerySaidTheStateIsUnprocessedButHeyMaybeThatWasStaleInfo;

    QByteArray jsonResultByteArray(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString jsonResultString(jsonResultByteArray);

    m_ViewQueryPropertyTree.clear();
    std::string jsonResultStdString = jsonResultString.toStdString();
    std::istringstream is(jsonResultStdString);
    read_json(is, m_ViewQueryPropertyTree);
    boost::optional<std::string> errorMaybe = m_ViewQueryPropertyTree.get_optional<std::string>("error");
    if(errorMaybe.is_initialized())
    {
        emit e("json had error set");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    m_ViewQueryHadError = false;
}
void Abc2WithdrawRequestProcessor::processNextWithdrawalRequestOrEmitFinishedIfNoMore()
{
    ++m_CurrentIndexIntoViewQueryResultsStringList;
    if(m_CurrentIndexIntoViewQueryResultsStringList < m_ViewQueryResultsStringListSize)
    {
        processWithdrawalRequest(m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate.at(m_CurrentIndexIntoViewQueryResultsStringList));
    }
    else
    {
        if(m_Mode == CalculateMode) //calculate iteration just ended
        {
            //Pause for user input (TO DOnereq: also allow them to quit, TODOoptional: allow them to write the buffered withdrawal requests list out to file, then quit)
            std::string runningCalculatedTotalJsonString = satoshiIntToJsonString(m_RunningTotalOfAllWithdrawalRequestsInSatoshis);
            QString runningCalculatedTotalJsonQString = QString::fromStdString(runningCalculatedTotalJsonString);
            emit calculationIterationComplete_SoWaitForUserInputBeforeContinuingOntoExecutionIteration(runningCalculatedTotalJsonQString);
        }
        else //execution iteration just ended
        {
            emit o("Withdrawal Request Processing Finished Successfully");
            emit withdrawalRequestProcessingFinished(true);
        }
    }
}
void Abc2WithdrawRequestProcessor::processWithdrawalRequest(const QString &currentKeyToNotDoneWithdrawalRequest)
{
    //"maybe" unprocessed because we don't trust the view query. it may have given us stale data

    //check state is really not done
    m_CurrentKeyToNotDoneWithdrawalRequest = currentKeyToNotDoneWithdrawalRequest.toStdString();
    std::string getDescription = "the withdrawal request for verifying in state not done for the first step of processing: " + m_CurrentKeyToNotDoneWithdrawalRequest;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToNotDoneWithdrawalRequest, getDescription))
    {
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_cas_t withdrawalRequestCas = m_LastGetCas;
    m_CurrentWithdrawalRequestPropertyTree.clear();
    std::istringstream withdrawalRequestIs(m_LastDocGetted);
    read_json(withdrawalRequestIs, m_CurrentWithdrawalRequestPropertyTree);
    const std::string &withdrawFundsRequestState = m_CurrentWithdrawalRequestPropertyTree.get<std::string>(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY);
    QString withdrawFundsRequestStateQString = QString::fromStdString(withdrawFundsRequestState);
    //if(withdrawFundsRequestState != JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_UNPROCESSED)
    if(withdrawFundsRequestStateQString.endsWith("_Done"))
    {
        //the view query was stale (despite stale=false) and the state was not in fact not done... so we just continue onto the next one
        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return;
    }

    //state is "not done"

    //check profile not locked attempting to fill slot (or locked withdrawing)
    QStringList keyParts = currentKeyToNotDoneWithdrawalRequest.split(D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR);
    const QString &userRequestingWithdrawal = keyParts.at(1);
    if(m_UsersWithProfilesFoundLocked.contains(userRequestingWithdrawal))
    {
        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return; //we already checked them and they have a locked profile, so we don't check any of their other withdrawal requests
    }
    m_CurrentUserRequestingWithdrawal = userRequestingWithdrawal.toStdString();
    getDescription = "the profile request to see if it's locked and then to lock it for user: " + m_CurrentUserRequestingWithdrawal;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(userAccountKey(m_CurrentUserRequestingWithdrawal), getDescription))
    {
        //error getting profile doc
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_cas_t userProfileCas = m_LastGetCas;
    m_CurrentUserProfilePropertyTree.clear();
    std::istringstream userProfileIs(m_LastDocGetted);
    read_json(userProfileIs, m_CurrentUserProfilePropertyTree);


    if(withdrawFundsRequestState == JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSING)
    {
        //error recovery from state 'processing'

        //If profile isn't locked towards the withdrawal request: TOTAL SYSTEM FAILURE
        const std::string &withdrawalRequestLockedToHopefully = m_CurrentUserProfilePropertyTree.get<std::string>(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS, "n");
        if(withdrawalRequestLockedToHopefully != m_CurrentKeyToNotDoneWithdrawalRequest)
        {
            emit e("TOTAL SYSTEM FAILURE: found a withdrawal request in state 'processing' (" + currentKeyToNotDoneWithdrawalRequest + "), but the associated user profile (" + userRequestingWithdrawal + ") was not locked towards the withdrawal request!");
            emit withdrawalRequestProcessingFinished(false);
            return;
        }

        //profile is locked towards the withdrawal request

        emit e("Encountered a withdrawal request in state 'processing' (it will not be processed, nor will any other withdrawal requests for that user): " + currentKeyToNotDoneWithdrawalRequest);
        //for now just going to do nothing (the users account will be seen as 'locked' and none of the rest of the user's withdrawal requests will even be attempted), but once I add SOMETHING here TODOreq (such as asking whether to continue or whatever, but honestly idfk what to do. One thing I could try is to check my wallet balance to see if I'm "missing" the amount specified in the withdrawal request, indicating I did pay it out (but "many" of such failed withdrawal requests would make the balanace missing higher/not-match-exactly)), I should uncomment the below two statements:

        //processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        //return;
    }
    if(withdrawFundsRequestState == JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSEDBUTPROFILENEEDSDEDUCTINGANDUNLOCKING)
    {
        //error recovery from state 'processedButProfileNeedsDeductingAndUnlocking'

        const std::string &withdrawalRequestProfileIsMaybeLockedTo = m_CurrentUserProfilePropertyTree.get<std::string>(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS, "n");
        m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas = withdrawalRequestCas; //both code paths below need to make this member valid before jumping
        if(withdrawalRequestProfileIsMaybeLockedTo != m_CurrentKeyToNotDoneWithdrawalRequest)
        {
            //profile is locked towards the withdrawal request

            //Continue like normal at the 'debit+unlock' stage, BUT

            //FIRST, need to make valid m_CurrentUserBalanceInSatoshis, m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis, and m_CurrentUserProfileInLockedWithdrawingStateCas

            //m_CurrentUserBalanceInSatoshis and m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis
            readInUserBalanceAndCalculateWithdrawalFeesEtc();

            //m_CurrentUserProfileInLockedWithdrawingStateCas
            //m_CurrentUserProfileInLockedWithdrawingStateCas = userProfileCas;

            //SECOND, we need to make valid all the members accessed in the 'Bump state to processingDone like normal' parts, since that is done after the deduct+unlock. we're "jumping", which admittedly is dangerous xD

            //m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas
            //m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas = withdrawalRequestCas; //optimized to above

            continueAt_deductAmountFromCurrentUserProfileBalanceAndCasSwapUnlockIt_StageOfWithdrawRequestProcessor();
        }
        else
        {
            //profile is NOT locked towards the withdrawal request

            //Bump state to processingDone like normal

            //FIRST, need to make valid m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas
            //m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas = userProfileCas; //optimized to above

            continueAt_setWithdrawalRequestStateToProcessedAndDone_StageOfWithdrawRequestProcessor();
        }

        return; //don't continue on as if the withdrawal request state was 'unprocessed'
    }

    //state is 'unprocessed'

    if(Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(m_CurrentUserProfilePropertyTree))
    {
        m_UsersWithProfilesFoundLocked.insert(userRequestingWithdrawal);
        emit o("user profile '" + userRequestingWithdrawal + "' is locked, continuing onto next. no more of this user's withdrawal requests will be processed and you must run this app again after fixing the locked account problem manually (or maybe it will fix itself)"); //TO DOne req: append to list of locked profiles, so we don't check again later for all their other withdrawal requests (especially since that would break ordering for processing of withdrawal requests. now changing from optimization to req)
        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return; //just continue onto next for now. the above output is kind of like a "warning". it's not fatal, but we do need to intervene...
    }

    readInUserBalanceAndCalculateWithdrawalFeesEtc();

    if(m_Mode == ExecuteMode)
    {
        //if(m_CurrentUserBalanceInSatoshis < m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis)
        if(m_CurrentUserBalanceInSatoshis < m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis)
        {
            //insufficient funds. not fatal error, they might have simply spent the funds on an ad slot filler after filing the withdrawal request. set the withdrawal request state to insufficient funds done and return true;
            m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_INSUFFICIENTFUNDS);
            std::string insufficientFundsWithdrawalRequest = Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree);
            if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToNotDoneWithdrawalRequest, insufficientFundsWithdrawalRequest, LCB_SET, withdrawalRequestCas, "setting withdrawal request '" + m_CurrentKeyToNotDoneWithdrawalRequest + "' to insufficient funds"))
            {
                emit withdrawalRequestProcessingFinished(false);
                return; //on the other hand, failing to set the withdrawal funds request to insufficient funds IS fatal error
            }
            processNextWithdrawalRequestOrEmitFinishedIfNoMore();
            return;
        }
    }

    //profile not locked and there are sufficient funds

    if(m_Mode == CalculateMode)
    {
        //TO DOnereq: individual user balances need to rolling debit -- the "set insufficient funds" code (just above) must be changed, because it affects it. perhaps i should just not do it during calculate mode? idfk

        //we intercept the code path here because we don't need to do any of the rest of it. we just add up what WOULD HAVE been done and proceed to next

        //update the 'calculated balance'
        if(!debitCurrentWithdrawalRequestFromUsersCalculatedRollingBalance_AndReturnTrueIfWeHaveCalculatedTheyHaveSufficientFundsForThisWithdrawalRequest())
        {
            processNextWithdrawalRequestOrEmitFinishedIfNoMore(); //'insufficient funds' (see the method body of debitCurrentWithdrawalRequestFromUsersCalculatedRollingBalance_AndReturnTrueIfWeHaveCalculatedTheyHaveSufficientFundsForThisWithdrawalRequest for details)
            return;
        }

        //nope: m_RunningTotalOfAllWithdrawalRequests += m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis;
        m_RunningTotalOfAllWithdrawalRequestsInSatoshis += m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis;

        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return;
    }

    //lock profile towards the withdrawal request
    m_CurrentUserProfilePropertyTree.put(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS, m_CurrentKeyToNotDoneWithdrawalRequest);
    std::ostringstream userProfileLockedForWithdrawingOs;
    write_json(userProfileLockedForWithdrawingOs, m_CurrentUserProfilePropertyTree, false);
    if(!couchbaseStoreRequestWithExponentialBackoff(userAccountKey(m_CurrentUserRequestingWithdrawal), userProfileLockedForWithdrawingOs.str(), LCB_SET, userProfileCas, "locking user profile '" + m_CurrentUserRequestingWithdrawal + "' for withdrawal of funds request '" +  m_CurrentKeyToNotDoneWithdrawalRequest + "'"))
    {
        //db error locking profile doc. TO DOnereq: it may be a non-fatal error in that the cas swap simply failed (they are attempting to purchase a slot and the profile was locked? (or even they are adding funds or shit changing their password. tons of reasons this could fail) nothing wrong with that). right now we have 'return false', which indicates a fatal error. m_LastOpStatus is your friend here, and maybe i shouldn't use the "requiring success" method, idfk
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    if(m_LastOpStatus != LCB_SUCCESS)
    {
        if(m_LastOpStatus == LCB_KEY_EEXISTS) //cas-swap fail
        {
            //cas swap fail means their account is active, purchasing a slot, or other shit. it's not a fatal error, but we don't attempt any of their other withdrawal requests
            m_UsersWithProfilesFoundLocked.insert(userRequestingWithdrawal);
            processNextWithdrawalRequestOrEmitFinishedIfNoMore();
            return;
        }

        //db error
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    m_CurrentUserProfileInLockedWithdrawingStateCas = m_LastSetCas;

    //cas swap set withdrawal request state to 'processing'
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSING);
    std::string withdrawalRequestInStateProcessingJson = Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToNotDoneWithdrawalRequest, withdrawalRequestInStateProcessingJson, LCB_SET, withdrawalRequestCas, "setting withdrawal request '" + m_CurrentKeyToNotDoneWithdrawalRequest + "' to processing"))
    {
        //error setting withdrawal request state to processing
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    m_CurrentWithdrawalRequestInProcessingStateCas = m_LastSetCas;

    //withdrawal request state is 'processing', so PROCESS it by issuing the payout command to bitcoin!

    QNetworkRequest request(QUrl("http://admin2:123@127.0.0.1:19011/")); //TODOreq: maybe qsettings 'profiles', but obviously some way to specify user/pw/ip/port (I lean against cli args [solely] because if I'm 'launched', I want to be able to show what's on my screen (but eh not a necessity initially, fuggit))
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    const std::string &keyToWithdrawTo = m_CurrentWithdrawalRequestPropertyTree.get<std::string>(JSON_WITHDRAW_FUNDS_BITCOIN_PAYOUT_KEY);
    QString keyToWIthdrawToQString = QString::fromStdString(keyToWithdrawTo);
    std::string desiredWithdrawAmountAsJsonString = satoshiIntToJsonString(m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis);
    QString desiredWithdrawAmountAsJsonQString = QString::fromStdString(desiredWithdrawAmountAsJsonString);
    m_CurrentBitcoinCommand = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"sendtoaddress\", \"params\": [\"" + keyToWIthdrawToQString + "\", " + desiredWithdrawAmountAsJsonQString + "] }";
    QByteArray payload = m_CurrentBitcoinCommand.toUtf8();
    m_NetworkAccessManager->post(request, payload); //TODOreq: separate bitcoin daemons on separate ports for payout/receive? or maybe use the "accounts" feature? i know for sure they need to be different wallets, so perhaps separate processes. (nope:the payout wallet daemon doesn't need to be running 24/7. in fact, this app can/should start/stop it on demand) since we want to stay in sync with the block chain, it does need to be running 24/7. there might be a way to make them share the blocks they receive to lessen bandwidth requirements, or maybe there is a way to do multiple wallets for one process, idfk
}
void Abc2WithdrawRequestProcessor::readInUserBalanceAndCalculateWithdrawalFeesEtc()
{
    const std::string &userBalanceInSatoshisStr = m_CurrentUserProfilePropertyTree.get<std::string>(JSON_USER_ACCOUNT_BALANCE);
    m_CurrentUserBalanceInSatoshis = boost::lexical_cast<SatoshiInt>(userBalanceInSatoshisStr);

    std::string amountToWithdrawInSatoshisStr = m_CurrentWithdrawalRequestPropertyTree.get<std::string>(JSON_WITHDRAW_FUNDS_REQUESTED_AMOUNT);
    m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis = boost::lexical_cast<SatoshiInt>(amountToWithdrawInSatoshisStr);
#if 0 //before moving fee from withdrawals to transactions
    double desiredWithdrawRequestAmountAsJsonDouble = satoshiIntToJsonDouble(m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis);
    double withdrawalFeeJsonDouble = withdrawalFeeForWithdrawalAmount(desiredWithdrawRequestAmountAsJsonDouble);
    SatoshiInt withdrawalFeeInSatoshis = jsonDoubleToSatoshiIntIncludingRounding(withdrawalFeeJsonDouble); //the rounding, it does nothing (we already 'rounded up') ;-P!
    m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis = m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis + withdrawalFeeInSatoshis;
#endif
}
bool Abc2WithdrawRequestProcessor::revertCurrentWithdrawalRequestStateToUnprocessed__AndUnlockWithoutDebittingUserProfile()
{
    //revertCurrentWithdrawalRequestStateToUnprocessed
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_UNPROCESSED);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToNotDoneWithdrawalRequest, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree), LCB_SET, m_CurrentWithdrawalRequestInProcessingStateCas, "reverting withdrawal request to state 'unprocessed'"))
    {
        emit withdrawalRequestProcessingFinished(false); //yea fuck it, emitting twice doesn't hurt =o
        return false;
    }

    return unlockUserProfileWithoutDebitting();
}
bool Abc2WithdrawRequestProcessor::setCurrentWithdrawalRequestToStateBitcoindReturnedError_Done__AndUnlockWithoutDebittingUserProfile(const QString &bitcoinDcommunicationsErrorToStoreInDb)
{
    QByteArray bitcoinDcommunicationsErrorToStoreInDbBA = bitcoinDcommunicationsErrorToStoreInDb.toUtf8();
    QByteArray bitcoinDcommunicationsErrorToStoreInDbBase64BA = bitcoinDcommunicationsErrorToStoreInDbBA.toBase64();
    QString bitcoinDcommunicationsErrorToStoreInDbBase64QString(bitcoinDcommunicationsErrorToStoreInDbBase64BA);
    std::string bitcoinDcommunicationsErrorToStoreInDbBase64StdString = bitcoinDcommunicationsErrorToStoreInDbBase64QString.toStdString();
    std::string bitcoinDcommunicationsErrorToStoreInDbStdString = bitcoinDcommunicationsErrorToStoreInDb.toStdString();

    //setCurrentWithdrawalRequestToStateBitcoindReturnedError_Done
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_BITCOINDCOMMANDRETURNEDERROR);
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_BITCOINDERRORIFAPPLICABLE, bitcoinDcommunicationsErrorToStoreInDbBase64StdString);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToNotDoneWithdrawalRequest, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree), LCB_SET, m_CurrentWithdrawalRequestInProcessingStateCas, "setting withdrawal request to state 'bitcoindCommandReturnedError_Done': " + bitcoinDcommunicationsErrorToStoreInDbStdString))
    {
        //as of writing, we're already emitting 'withdrawalRequestProcessingFinished(false)' after the call to this method returns, but maybe in the future i would want to emit it here <- no idea if this still holds true xd
        emit withdrawalRequestProcessingFinished(false); //yea fuck it, emitting twice doesn't hurt =o
        return false;
    }

    //UnlockWithoutDebittingUserProfile
    return unlockUserProfileWithoutDebitting();
}
bool Abc2WithdrawRequestProcessor::unlockUserProfileWithoutDebitting()
{
    m_CurrentUserProfilePropertyTree.erase(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(userAccountKey(m_CurrentUserRequestingWithdrawal), Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentUserProfilePropertyTree), LCB_SET, m_CurrentUserProfileInLockedWithdrawingStateCas, "unlocking without debitting user profile"))
    {
        emit withdrawalRequestProcessingFinished(false); //yea fuck it, emitting twice doesn't hurt =o
        return false;
    }
    return true;
}
void Abc2WithdrawRequestProcessor::continueAt_deductAmountFromCurrentUserProfileBalanceAndCasSwapUnlockIt_StageOfWithdrawRequestProcessor()
{
    if(m_Mode == CalculateMode)
    {
        //special hack for the error condition that can jump here, we need to deduct the amount from the user's calculated rolling balance
        //TO DOnereq: our error case processing can throw off the rolling calculated balance of a user, if the error case where we debit+unlock is seen. it would make the rolling debit amount we have in memory be invalid. On the other hand, if we don't process that error case then their balance will appear to be higher than what it will be during execution mode... which means some of their withdrawal requests that should have been considered insufficient funds might not have been, which may cause me to send more than needed to the payout wallet (not a HUGE deal, since the execution mode will still handle it fine, but I prefer accuracy)
        if(!debitCurrentWithdrawalRequestFromUsersCalculatedRollingBalance_AndReturnTrueIfWeHaveCalculatedTheyHaveSufficientFundsForThisWithdrawalRequest()) //we can ignore the return value of this because it's more or less guaranteed to have sufficient funds, SINCE THE PAYOUT WAS ALREADY ISSUED IT HAD BETTER!! but eh let's handle it and call it total system failure
        {
            emit e("TOTAL SYSTEM FAILURE: it's hard to explain so search for this in the source sdfoasdfsfldjdfkjsdflkjdfsodfusdfoui");
            emit withdrawalRequestProcessingFinished(false);
            return; //the 'return' here is actually doing something, because otherwise we'd be calling processNextWithdrawalRequestOrEmitFinishedIfNoMore twice when we should only be calling it once. but shit actually since we want to stop, it shouldn't even be calling it once. uhh, shit TO DOneoptional rare ass critical bug
        }
    }

    SatoshiInt originalBalance = m_CurrentUserBalanceInSatoshis;
    //m_CurrentUserBalanceInSatoshis -= m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis;
    m_CurrentUserBalanceInSatoshis -= m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis;
    m_CurrentUserProfilePropertyTree.put(JSON_USER_ACCOUNT_BALANCE, m_CurrentUserBalanceInSatoshis);
    m_CurrentUserProfilePropertyTree.erase(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(userAccountKey(m_CurrentUserRequestingWithdrawal), Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentUserProfilePropertyTree), LCB_SET, m_CurrentUserProfileInLockedWithdrawingStateCas, "debitting+unlocking user profile '" + m_CurrentUserRequestingWithdrawal + "' with amount: (OLD: " + satoshiIntToJsonString(originalBalance) + ", NEW: " + satoshiIntToJsonString(m_CurrentUserBalanceInSatoshis) + ", DIFF: -" + satoshiIntToJsonString(m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis) + ")"))
    {
        //error debitting+unlocking user profile
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    continueAt_setWithdrawalRequestStateToProcessedAndDone_StageOfWithdrawRequestProcessor();
}
void Abc2WithdrawRequestProcessor::continueAt_setWithdrawalRequestStateToProcessedAndDone_StageOfWithdrawRequestProcessor()
{
    //set withdrawal request state to "processed and done"
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSEDDONE);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToNotDoneWithdrawalRequest, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree), LCB_SET, m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas, "setting withdrawal request '" + m_CurrentKeyToNotDoneWithdrawalRequest + "' to processedDone"))
    {
        //error setting withdrawal request state to processedDone
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //onto the next withdrawal request. this used to be a while loop (easier to read/understand), but QNetworkAccessManager requires async guh
    processNextWithdrawalRequestOrEmitFinishedIfNoMore();
}
void Abc2WithdrawRequestProcessor::processWithdrawalRequests()
{
    if(!connectToCouchbase())
        return;
    emit o("Connected...");

    lcb_set_http_complete_callback(m_Couchbase, Abc2WithdrawRequestProcessor::viewQueryCompleteCallbackStatic);

    std::string viewPath = "_design/dev_AllWithdrawalRequestsWithStateOfNotDone/_view/AllWithdrawalRequestsWithStateOfNotDone?stale=ok"; //TODOmb: perhaps if this app is run on the webserver, we should use some form of pagination so that all the requests don't have to be in ram. for now KISS. Another way to solve it is to run this app on a server other than the webserver
    lcb_http_cmd_t viewQueryCmd;
    viewQueryCmd.version = 0;
    viewQueryCmd.v.v0.path = viewPath.c_str();
    viewQueryCmd.v.v0.npath = viewPath.length();
    viewQueryCmd.v.v0.body = NULL;
    viewQueryCmd.v.v0.nbody = 0;
    viewQueryCmd.v.v0.method = LCB_HTTP_METHOD_GET;
    viewQueryCmd.v.v0.chunked = 0;
    viewQueryCmd.v.v0.content_type = "application/json";
    m_ViewQueryHadError = true; //prove to me that there wasn't an error ;-P
    lcb_error_t error = lcb_make_http_request(m_Couchbase, this, LCB_HTTP_TYPE_VIEW, &viewQueryCmd, NULL);
    if (error != LCB_SUCCESS)
    {
        emit e("Failed to query view " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_wait(m_Couchbase);
    if(m_ViewQueryHadError)
    {
        emit e("View query had error");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //view successfully queried

    const ptree &rowsPt = m_ViewQueryPropertyTree.get_child("rows");
    m_UsersWithProfilesFoundLocked.clear();
    BOOST_FOREACH(const ptree::value_type &row, rowsPt)
    {
        //convert to QStringList, because i suck at using boost iterators and don't care to look it up. TODOoptimization: use an iterator or index to async iterate m_ViewQueryPropertyTree's rows DIRECTLY (instead of copying to qstringlist for same purpose)
        m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate.append(QString::fromStdString(row.second.get<std::string>("id")));
    }
    m_ViewQueryPropertyTree.clear(); //just an optimization, because we now have two copies of the list in memory lol

    m_CurrentIndexIntoViewQueryResultsStringList = -1;
    m_ViewQueryResultsStringListSize = m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate.size();
    m_Mode = CalculateMode;
    m_RunningTotalOfAllWithdrawalRequestsInSatoshis = 0;
    m_UserBalancesInSatoshisDuringCalculationMode.clear();
    processNextWithdrawalRequestOrEmitFinishedIfNoMore();
}
void Abc2WithdrawRequestProcessor::proceedOntoExecutionIteration()
{
    //reset iteration, change mode to execute, and start over :)
    m_CurrentIndexIntoViewQueryResultsStringList = -1;
    m_Mode = ExecuteMode;
    processNextWithdrawalRequestOrEmitFinishedIfNoMore();
}
void Abc2WithdrawRequestProcessor::userWantsUsToHandleTheBitcoindCommunicationsErrorThisWay(Abc2WithdrawRequestProcessor::WayToHandleBitcoindCommunicationsErrorEnum userSelectedWayToHandleBitcoindCommunicationsError, const QString &errorStringToStoreInDb)
{
    //the action itself
    switch(userSelectedWayToHandleBitcoindCommunicationsError)
    {
        case RevertWithdrawalRequestStateToUnprocessedAndContinueProcessingWithdrawalRequests:
        case RevertWithdrawalRequestStateToUnprocessedAndStopProcessingWithdrawalRequests:
        {
            //revert
            if(!revertCurrentWithdrawalRequestStateToUnprocessed__AndUnlockWithoutDebittingUserProfile())
            {
                //db error, so don't continue no matter what the user requested
                return;
            }
        }
            break;
        case SetWithdrawalRequestStateToBitcoindReturnedErrorAndContinueProcessingWithdrawalRequests:
        case SetWithdrawalRequestStateToBitcoindReturnedErrorAndStopProcessingWithdrawalRequests:
        {
            //set error state to bitcoind returned error
            if(!setCurrentWithdrawalRequestToStateBitcoindReturnedError_Done__AndUnlockWithoutDebittingUserProfile(errorStringToStoreInDb))
            {
                //db error, so don't continue no matter what the user requested
                return;
            }
        }
            break;
    }

    //whether to continue processing withdrawal requests or to stop
    switch(userSelectedWayToHandleBitcoindCommunicationsError)
    {
        case RevertWithdrawalRequestStateToUnprocessedAndContinueProcessingWithdrawalRequests:
        case SetWithdrawalRequestStateToBitcoindReturnedErrorAndContinueProcessingWithdrawalRequests:
        {
            //continue
            processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        }
            break;
        case RevertWithdrawalRequestStateToUnprocessedAndStopProcessingWithdrawalRequests:
        case SetWithdrawalRequestStateToBitcoindReturnedErrorAndStopProcessingWithdrawalRequests:
        {
            //stop
            emit withdrawalRequestProcessingFinished(true); //they asked to stop, but it was on account of seeing an error. does that count as success? blah~
        }
            break;
    }
}
void Abc2WithdrawRequestProcessor::handleNetworkReplyFinished(QNetworkReply *reply)
{
    QScopedPointer<QNetworkReply, QScopedPointerObjectDeleteLater<QNetworkReply> > replyDeleter(reply);
    Q_UNUSED(replyDeleter)
    if(reply->error() != QNetworkReply::NoError)
    {
        QString networkReplyError = "QNetworkReply has error: " + reply->errorString();
        QString bitcoinCommandCausingErrorString = "the bitcoin request causing the error: " + m_CurrentBitcoinCommand;
        emit bitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(bitcoinCommandCausingErrorString, networkReplyError);
        return;
    }
    QVariant responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if((!responseCode.isValid()) || (responseCode.toInt() != 200))
    {
        QString statusCodeError = "QNetworkReply did not return status code of 200. status code: " + QString::number(responseCode.toInt());
        QString bitcoinCommandCausingErrorString = "the bitcoin request causing the error: " + m_CurrentBitcoinCommand;
        emit bitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(bitcoinCommandCausingErrorString, statusCodeError);
        return;
    }
    //check for json error
    QByteArray replyByteArray = reply->readAll();
    QString replyQString(replyByteArray);
    std::string replyStdString = replyQString.toStdString();
    std::istringstream is(replyStdString);
    ptree pt;
    read_json(is, pt);
    std::string jsonErrorMaybe = pt.get<std::string>("error");
    if(jsonErrorMaybe != "null") //certain kinds of errors, such as "invalid bitcoin key", don't require user interaction to ask what should happen next. we should just set the withdrawal request state to bitcoindCommandReturnedError_Done and continue without stopping
    {
        QString bitcoindJsonError = "bitcoind reply had json error set: " + QString::fromStdString(jsonErrorMaybe);
        QString bitcoinCommandCausingErrorString = "the bitcoin request causing the error: " + m_CurrentBitcoinCommand;
        emit bitcoindCommunicationsErrorDetectedSoWeNeedToAskTheUserHowToProceed(bitcoinCommandCausingErrorString, bitcoindJsonError);
        return;
    }

    //the bitcoin sendtoaddress command response had no errors

    //TO DOne optional: the ptree in scope contains a txid that we could store in the withdrawal request (when putting in the 'done' state) for book keeping and such. hell, i might [legally] have to! TODOreq: figure out if it's a legal requirement (i'm still curious even though i already implemented it). It might also help in error recovery, but I'm unsure
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_TXID, pt.get<std::string>("result"));

    //cas swap set withdrawal request state to "processed, profile needs balance deducting/unlocking"
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSEDBUTPROFILENEEDSDEDUCTINGANDUNLOCKING);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToNotDoneWithdrawalRequest, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree), LCB_SET, m_CurrentWithdrawalRequestInProcessingStateCas, "setting withdrawal request '" + m_CurrentKeyToNotDoneWithdrawalRequest + "' to processedButProfileNeedsDeductingAndUnlocking"))
    {
        //error setting withdrawal request state to processedButProfileNeedsDeductingAndUnlocking
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    m_WithdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas = m_LastSetCas;


    //NOTE: IF CHANGING ANY CODE BENEATH THIS POINT (INCLUDING ALL [SUB-]FUNCTIONS] IN A WAY THAT MAKES THE CODE UTILIZE MORE (or different) MEMBER (m_) VARIABLES, THOSE MEMBER VARIABLES NEED TO BE POPULATED CORRECTLY BEFORE THE ERROR CONDITIONS 'PROCESSED' AND 'PROCESSEDNEEDTODEBITANDUNLOCKPROFILE' _ALSO_ CONTINUE TO THE BELOW STAGES


    //deduct amount from profile balance and cas swap unlock it
    continueAt_deductAmountFromCurrentUserProfileBalanceAndCasSwapUnlockIt_StageOfWithdrawRequestProcessor();
#if 0
    if(!deductAmountFromCurrentUserProfileBalanceAndCasSwapUnlockIt(m_CurrentUserBalanceInSatoshis, m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis, m_CurrentUserProfileInLockedWithdrawingStateCas))
    {
        return;
    }
#endif
}

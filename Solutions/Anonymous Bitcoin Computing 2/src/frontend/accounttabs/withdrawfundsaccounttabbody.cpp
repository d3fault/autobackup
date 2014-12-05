#include "withdrawfundsaccounttabbody.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WDoubleValidator>

#include "../anonymousbitcoincomputingwtgui.h"
#include "../validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.h"
#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

//TODOreq:whatever minimum bitcoin transaction is (either in Abc2 or on bitcoin network)
#define WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT 0.00000001 /*if changing this, change below string too*/
#define WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT_STR "0.00000001"

//The flow goes like this:
//In-Abc2: click request withdraw button, check balance is enough, get withdraw index, recursively try to lcb_add to index (+1'ing as appropriate), cas swap accepting fail withdraw index
//In-payout-processor: query a view to get all [stale=false] payout requests with state=unprocessed, iterate them. Here's one payout request flow: check balance is enough, if not set state=insufficientfunds and done. else (sufficient funds), cas swap lock profile account requiring success (fail means they are buying an ad slot or adding funds or something similar, so try again later (leave state as is, continue to next request. mb say it in stdout tho)), cas swap requiring success the payout request state to 'processing' (mb with durability poll of 3 as well but guh mb not since it isn't an lcb_add). finding a payout request in state 'processing' that we didn't JUST set to that means there was previously a fail. these are dangerous as fuck because we might pay them out twice (as in, when we find a payout request in state 'processing' that we didn't just set ourselves, it may or may not have already been processed! processing AGAIN (a la recovery) would be a double pay). check bitcoin logs or idfk what and do manual ass shit to see wtf is going on. see if we paid them or what (this is why i want it to be manual). do bitcoin payout and check it's response etc (spit out errors). cas swap set payout request state to 'processed, profile needs balance deducting/unlocking". deduct amount from profile balance and unlock it. TODOreq: the profile should lock TO the specific payout request (just like buy ad slots are), which will allow us to recover somehow (can't put my finger on it, but yea. I also think it helps us safeguard against a double payout [during recovery]...). then set payout request state to 'processed and done'.
//^^so we can also query a view making sure there are no payout requests in any 'intermediate' states (indicating failure): intermediate states are processing and processed-profile-needs-blanace-deducting.
//In-payout-processor STATES: unprocessed,insufficientFundsDone,processing,processedButProfileNeedsDeductingAndUnlocking,processedDone
//^maybe it's a good idea to store the bitcoin key we are attempting to send money to (and the amount, and MAYBE even the datetime) in the withdraw request when the state is "processing" (and maybe processedButProfileNeedsDeductingAndUnlocking+processedDone), so that if we do find a withdraw request in state "processing" (indicating a previous fail), we have at least SOMETHING to go by to check [manually, for now] whether the payout was sent (as in, whether the bitcoin command was issued/succeeded). The key/amount/datetime will be MORE OR LESS differentiable from one to the next, but it is not perfect: carefully crafted withdraw requests (same balance, same key) that are processed 'manually but in an automated fashion' might just have the same datetime and in that case then we'd NOT deduct the amount from their balance (thus 'giving them' more money)!!! TODOreq

//TODOoptional: some kind of "notify me via email when the funds are sent" functionality...
//TODOreq: OT as fuck, but: I myself have no need to use the "withdraw" functionality. What this does mean however is that my "abc2" account will have assloads of unused funds, and will not accurately reflect how much I really have (since I will have spent them). If I ever want to buy an ad slot, I need to somehow actually make sure I can afford it (otherwise there will be trouble when that person requests a payout). One way to solve this is to use a different account for buying, and to never use the 'receive' funds account (dog food, etc) for buying ads
WithdrawFundsAccountTabBody::WithdrawFundsAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp)
    : IAccountTabWidgetTabBody(abcApp)
{ }
void WithdrawFundsAccountTabBody::populateAndInitialize()
{
    new WText("<b>Withdraw Funds to Bitcoin</b>", this);

    new WBreak(this);
    new WBreak(this);

    new WText("Use this page to request a withdrawal of funds. Requests are processed manually, and usually within 24 hours. Automated withdrawals might come eventually, but as you might imagine it's dangerous functionality to implement.", this);

    new WBreak(this);
    new WBreak(this);

    new WText("<i>There is a 3% fee to withdraw funds.</i>", this); //TODOoptional: if js is enabled, show them as they type in the line edit just how much they will get after the 3% fee is applied

    new WBreak(this);
    new WBreak(this);

    new WText("Amount to Withdraw:", this);
    m_AmountToWithdrawLineEdit = new WLineEdit(WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT_STR, this);
    WDoubleValidator *bitcoinAmountValidator = new WDoubleValidator(WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT, 21000000 /* TODOoptional: use their balance instead of max bitcoins lol */, m_AmountToWithdrawLineEdit);
    bitcoinAmountValidator->setMandatory(true);
    m_AmountToWithdrawLineEdit->setValidator(bitcoinAmountValidator);

    new WBreak(this);

    new WText("Bitcoin Key To Withdraw Funds To:", this);
    m_BitcoinPayoutKeyLineEdit = new WLineEdit(this);
    LettersNumbersOnlyRegExpValidatorAndInputFilter *bitcoinKeyValidator = new LettersNumbersOnlyRegExpValidatorAndInputFilter("35", m_BitcoinPayoutKeyLineEdit); //TODOreq: proper bitcoin key validity (had:base58 check encoding) filter instead, including minimum length (right now I only have max len)
    bitcoinAmountValidator->setMandatory(true);
    m_BitcoinPayoutKeyLineEdit->setValidator(bitcoinKeyValidator);

    new WBreak(this);

    WPushButton *requestWithdrawalButton = new WPushButton("Request Withdrawal", this);

    requestWithdrawalButton->clicked().connect(this, &WithdrawFundsAccountTabBody::handleRequestWithdrawalButtonClicked); //TODOoptional: maybe disable button after clicking as well, which means I need to enable it whenever we resume rendering etc
}
void WithdrawFundsAccountTabBody::handleRequestWithdrawalButtonClicked()
{
    //TODOreq: sanitize fields (double for amount, bitcoin key could even rely on the bitcoin exe call that checks if valid (BUT DO PRE-SANTIZING TOO OFC BECAUSE SENDING A STRING TO A PROCESS IS DANGEROUS LOL)
    //TODOreq: verify funds amount when? a validator on the line edit would be best, but implies a db hit before showing the page. sanitizing here after the button is clicked certainly makes sense (since we do db stuff anyways). And of course sanitizing in the "withdraw request processer" app is paramount.
    //TODOreq: for now they can schedule multiple requests for the full amount of their balance. the "withdraw request processor" will simply reject any/all requests that (at the time of processing) don't have enough funds available. Ideally we would reject the scheduling of their second request (total up all previous requests, subtract from balance, see if new request is more than the difference -- reject that request if it is -- but request processing atomicity poses a problem: if I am running request app processor simultaneously == ???? fml)

    if(m_AmountToWithdrawLineEdit->validate() != WValidator::Valid)
    {
        new WBreak(this);
        new WText("Invalid Amount", this);
        return;
    }
    if(m_BitcoinPayoutKeyLineEdit->validate() != WValidator::Valid)
    {
        new WBreak(this);
        WText("Invalid Bitcoin Key", this);
        return;
    }

    //get the user profile doc to verify sufficient funds
    m_AbcApp->getCouchbaseDocumentByKeyBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::GETPROFILEBEFORESCHEDULINGWITHDRAWFUNDSREQUESTTOVERIFYTHEYHAVEENOUGHFUNDS;

    m_AbcApp->deferRendering(); //TODOreq: resume rendering at all error paths etc
}
void WithdrawFundsAccountTabBody::verifyBalanceIsGreaterThanOrEqualToTheirRequestedWithdrawAmountAndThenContinueSchedulingWithdrawRequest(const string &userProfileDoc, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "verifyBalanceIsGreaterThanOrEqualToTheirRequestedWithdrawAmountAndThenContinueSchedulingWithdrawRequest db error" << endl;
        m_AbcApp->resumeRendering();
        return;
    }
    if(!lcbOpSuccess)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "TOTAL SYSTEM FAILURE: user profile did not exist for a logged in user, in: verifyBalanceIsGreaterThanOrEqualToTheirRequestedWithdrawAmountAndThenContinueSchedulingWithdrawRequest" << endl;
        m_AbcApp->resumeRendering();
        return;
    }

    //get here, user profile doc exists and was gotten
    ptree pt;
    std::istringstream is(userProfileDoc);
    read_json(is, pt);

    const std::string &userBalanceInSatoshisStr = pt.get<std::string>(JSON_USER_ACCOUNT_BALANCE);
    SatoshiInt userBalanceInSatoshis = boost::lexical_cast<SatoshiInt>(userBalanceInSatoshisStr);

    std::string amountToWithdrawJsonString = m_AmountToWithdrawLineEdit->text().toUTF8();
    SatoshiInt amountToWithdrawInSatoshis = jsonStringToSatoshiInt(amountToWithdrawJsonString);

    if(userBalanceInSatoshis < amountToWithdrawInSatoshis)
    {
        new WBreak(this);
        new WText("Insufficient Funds", this);
        m_AbcApp->resumeRendering();
        return;
    }

    //withdraw amount is >= their balance, but the final check for that is in the payout request processor app anyways. all other checks are just good UI

    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(withdrawFundsRequestsNextAvailableIndexKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETWITHDRAWREQUESTINDEXBECAUSEWEWANTTOSCHEDULEAWITHDRAWDUH;
}
void WithdrawFundsAccountTabBody::useNextAvailableWithdrawRequestIndexToScheduleTheWithdrawRequest(const string &withdrawRequestNextAvailalbeIndexDoc, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "useNextAvailableWithdrawRequestIndexToScheduleTheWithdrawRequest db error" << endl;
        m_AbcApp->resumeRendering();
        return;
    }

    m_WithdrawRequestIndexToTryLcbAddingAt = "0";
    if(lcbOpSuccess)
    {
        //parse what the cache indicates is the next available index, but DO NOT RELY ON IT
        ptree pt;
        std::istringstream is(withdrawRequestNextAvailalbeIndexDoc);
        read_json(is, pt);

        m_WithdrawRequestIndexToTryLcbAddingAt = pt.get<std::string>(JSON_WITHDRAW_FUNDS_REQUESTS_NEXT_AVAILABLE_INDEX);
        m_CasOfWithdrawRequestIndexDoc_OrZeroIfDocDoesNotExist = cas;
    }
    else
    {
        //it should always exist, but might not if there was a power fail right during/after creating a user. if it doesn't exist, we simply walk up the indexes until a spot is found
        m_CasOfWithdrawRequestIndexDoc_OrZeroIfDocDoesNotExist = 0;
    }

    //create withdraw request doc
    ptree pt;

    pt.put(JSON_WITHDRAW_FUNDS_REQUESTED_AMOUNT, boost::lexical_cast<std::string>(jsonStringToSatoshiInt(m_AmountToWithdrawLineEdit->text().toUTF8())));
    pt.put(JSON_WITHDRAW_FUNDS_BITCOIN_PAYOUT_KEY, m_BitcoinPayoutKeyLineEdit->text().toUTF8());
    pt.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_UNPROCESSED);

    std::ostringstream withdrawRequestDocBuffer;
    write_json(withdrawRequestDocBuffer, pt, false);

    m_WithdrawRequestDocBeingCreated = withdrawRequestDocBuffer.str();

    attemptToLcbAddWithdrawRequest();
}
void WithdrawFundsAccountTabBody::attemptToLcbAddWithdrawRequest()
{
    int rangeCheck = boost::lexical_cast<int>(m_WithdrawRequestIndexToTryLcbAddingAt);
    if(rangeCheck == INT_MAX)
    {
        new WBreak(this);
        new WText("You've reached the maximum amount of withdraw requests. Please contact the administrator.", this); //TODOreq: ulong ulong?
        m_AbcApp->resumeRendering();
        return;
    }
    m_AbcApp->store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(withdrawFundsRequestKey(m_AbcApp->m_CurrentlyLoggedInUsername, m_WithdrawRequestIndexToTryLcbAddingAt), m_WithdrawRequestDocBeingCreated);
    m_AbcApp->m_WhatTheStoreWithoutInputCasWasFor = AnonymousBitcoinComputingWtGUI::ATTEMPTTOADDWITHDRAWREQUESTATINDEX;
}
void WithdrawFundsAccountTabBody::handleAttemptToAddWithdrawRequestAtIndexFinished(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "handleAttemptToAddWithdrawRequestAtIndexFinished db error" << endl;
        m_AbcApp->resumeRendering();
        return;
    }

    if(!lcbOpSuccess)
    {
        //+1 the index and try again
        m_WithdrawRequestIndexToTryLcbAddingAt = boost::lexical_cast<std::string>((boost::lexical_cast<int>(m_WithdrawRequestIndexToTryLcbAddingAt)+1));
        attemptToLcbAddWithdrawRequest();
        return;
    }

    //getting here means the withdraw request was created/lcb-added, so now we update the next available withdraw request index to point to +1 whatever we just got
    //TODOreq: if cas is 0, do lcb_add [ignoring fail]. else, do lcb_set with cas [ignoring fail]
    std::string nextAvailableIndexToPutInNextAvailableIndex = boost::lexical_cast<std::string>((boost::lexical_cast<int>(m_WithdrawRequestIndexToTryLcbAddingAt)+1));

    ptree pt;
    pt.put(JSON_WITHDRAW_FUNDS_REQUESTS_NEXT_AVAILABLE_INDEX, nextAvailableIndexToPutInNextAvailableIndex);

    std::ostringstream nextAvailableWithdrawRequestIndexDocBuffer;
    write_json(nextAvailableWithdrawRequestIndexDocBuffer, pt, false);

    const std::string &nextAvailableWithdrawFundsRequestIndexKey = withdrawFundsRequestsNextAvailableIndexKey(m_AbcApp->m_CurrentlyLoggedInUsername);
    if(m_CasOfWithdrawRequestIndexDoc_OrZeroIfDocDoesNotExist == 0)
    {
        m_AbcApp->store_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(nextAvailableWithdrawFundsRequestIndexKey, nextAvailableWithdrawRequestIndexDocBuffer.str());
        m_AbcApp->m_WhatTheStoreWithoutInputCasWasFor = AnonymousBitcoinComputingWtGUI::ADDWITHDRAWREQUESTINDEXIGNORINGRESPONSE;
    }
    else
    {
        m_AbcApp->store_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(nextAvailableWithdrawFundsRequestIndexKey, nextAvailableWithdrawRequestIndexDocBuffer.str(), m_CasOfWithdrawRequestIndexDoc_OrZeroIfDocDoesNotExist, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
        m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::CASSWAPWITHDRAWREQUESTINDEXIGNORINGRESPONSE;
    }

    new WBreak(this);
    new WText("Your withdraw request #" + m_WithdrawRequestIndexToTryLcbAddingAt + " has been scheduled. If you schedule any additional withdraw requests, it is up to you to ensure that the total amount for all pending withdraw requests does not exceed your account's balance. Withdraw requests are processed in the order they are receieved, and any withdraw request for an amount greater than your balance will simply be rejected.", this);
    m_AmountToWithdrawLineEdit->setText(WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT_STR);
    m_BitcoinPayoutKeyLineEdit->setText(""); //NOPE: could also clear the bitcoin key, but chances are DECENT that they'll use it again for the next request (WHY? nvm clearing fuckit)
}

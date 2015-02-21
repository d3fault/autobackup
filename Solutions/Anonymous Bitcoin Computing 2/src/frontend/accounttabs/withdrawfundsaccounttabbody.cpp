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
#include "../../abc2common.h"

using namespace boost::property_tree;


//TODOoptional: some kind of "notify me via email when the funds are sent" functionality... TODOoptimization: combine all such notifications for a single user into a single email
//TODOreq: OT as fuck, but: I myself have no need to use the "withdraw" functionality. What this does mean however is that my "abc2" account will have assloads of unused funds, and will not accurately reflect how much I really have (since I will have spent them [without interacting with Abc2 in any way]). If I ever want to buy an ad slot, I need to somehow actually make sure I can afford it (otherwise there will be trouble when that person requests a payout). One way to solve this is to use a different account for buying, and to never use the 'receive' funds account (dog food, etc) for buying ads
//TODOreq: given their balance, pre-calculate in a read-only line edit (for copy/pasting easier (TODOoptional: 'use this amount' button -- or perhaps it is the default value in the withdraw amount line edit (but that might cause a tad bit of confusion...))) how much the maximum they can withdraw is -- after the 3% fee is applied
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

    new WText("<i>There is a " ABC2_WITHDRAWAL_FEE_PERCENT_STR "% fee to withdraw funds.</i>", this);
    new WBreak(this);

    std::string maxWithdrawalAmountJsonDoubleIncorporatingFee = formatDoubleAs8decimalPlacesString(maximumWithdrawalAmountIncorporatingFee());
    new WText("Given your balance of: " + satoshiStringToJsonString(m_AbcApp->m_CurrentlyLoggedInUsersBalanceSatoshiStringForDisplayingOnly) + ", the maximum amount (after the " ABC2_WITHDRAWAL_FEE_PERCENT_STR "% fee has been applied) available for withdrawing is: " + maxWithdrawalAmountJsonDoubleIncorporatingFee + " ", this);
    WPushButton *useThisAmountButton = new WPushButton("Use This Amount", this);
    useThisAmountButton->clicked().connect(this, &WithdrawFundsAccountTabBody::handleUseMaxAmountButtonClicked);

    new WBreak(this);
    new WBreak(this);

    //TODOoptional: grid or table layout (idfk the difference)
    new WText("Desired Amount to Withdraw: ", this);
    m_DesiredAmountToWithdrawLineEdit = new WLineEdit(ABC2_MIN_WITHDRAW_AMOUNT_STR, this);
    WDoubleValidator *bitcoinAmountValidator = new WDoubleValidator(ABC2_MIN_WITHDRAW_AMOUNT, 21000000 /* TODOoptional: use their balance instead of max bitcoins lol */, m_DesiredAmountToWithdrawLineEdit);
    bitcoinAmountValidator->setMandatory(true);
    m_DesiredAmountToWithdrawLineEdit->setValidator(bitcoinAmountValidator);
    m_DesiredAmountToWithdrawLineEdit->setTextSize(ABC2_BITCOIN_AMOUNT_VISUAL_INPUT_FORM_WIDTH); //visual only

    new WBreak(this);

    new WText("+ Withdrawal Fee (" ABC2_WITHDRAWAL_FEE_PERCENT_STR "%): ", this);
    m_WithdrawalFeeLineEdit = new WLineEdit(formatDoubleAs8decimalPlacesString(withdrawalFeeForWithdrawalAmount(ABC2_MIN_WITHDRAW_AMOUNT)), this);
    m_WithdrawalFeeLineEdit->setDisabled(true); //visual only derp
    m_WithdrawalFeeLineEdit->setTextSize(ABC2_BITCOIN_AMOUNT_VISUAL_INPUT_FORM_WIDTH); //visual only

    new WBreak(this);

    new WText("= Total Amount Withdrawn (after " ABC2_WITHDRAWAL_FEE_PERCENT_STR "% fee): ", this);
    m_TotalWithdrawAmountVisualOnlyLineEdit = new WLineEdit(this); //TODOoptional: typing in this one populates "Amount to Withdraw" (with 3% taken out (how much they'll GET)), typing in "Amount To Withdraw" populates this one (with 3% fee factored in (how much their account's balance will be REDUCED). For now just read-only...
    m_TotalWithdrawAmountVisualOnlyLineEdit->setDisabled(true);
    m_TotalWithdrawAmountVisualOnlyLineEdit->setTextSize(ABC2_BITCOIN_AMOUNT_VISUAL_INPUT_FORM_WIDTH); //visual only

    if(!m_AbcApp->environment().ajax())
    {
        //no-js requires 'calculate' button
        new WText(" ", this);
        WPushButton *calculateTotalAmountPushButton = new WPushButton("Calculate Total Amount Withdrawn (after " ABC2_WITHDRAWAL_FEE_PERCENT_STR "% fee)", this);
        calculateTotalAmountPushButton->clicked().connect(this, &WithdrawFundsAccountTabBody::calculateTotalAmountWithdrawnAfterFeeForVisual);
    }
    else
    {
        //js calculates on key presses (TODOoptimization: make client side only, perhaps those stateless slot things would do it but idfk. custom js wouldn't be too hard either)
        m_DesiredAmountToWithdrawLineEdit->keyWentUp().connect(this, &WithdrawFundsAccountTabBody::calculateTotalAmountWithdrawnAfterFeeForVisual); //keypressed() signal is too soon
    }

    new WBreak(this);
    new WBreak(this);

    new WText("Bitcoin Key To Withdraw Funds To:", this);
    m_BitcoinPayoutKeyLineEdit = new WLineEdit(this);
    LettersNumbersOnlyRegExpValidatorAndInputFilter *bitcoinKeyValidator = new LettersNumbersOnlyRegExpValidatorAndInputFilter("35", m_BitcoinPayoutKeyLineEdit); //TODOreq: proper bitcoin key validity (had:base58 check encoding) filter instead, including minimum length (right now I only have max len)
    bitcoinAmountValidator->setMandatory(true);
    m_BitcoinPayoutKeyLineEdit->setValidator(bitcoinKeyValidator);
    m_BitcoinPayoutKeyLineEdit->setTextSize(35 /*bitcoin addresses are 35 chars max*/); //visual only

    new WBreak(this);
    new WBreak(this);

    WPushButton *requestWithdrawalButton = new WPushButton("Request Withdrawal", this);

    requestWithdrawalButton->clicked().connect(this, &WithdrawFundsAccountTabBody::handleRequestWithdrawalButtonClicked); //TODOoptional: maybe disable button after clicking as well, which means I need to enable it whenever we resume rendering etc
}
double WithdrawFundsAccountTabBody::maximumWithdrawalAmountIncorporatingFee()
{
    //TODOreq: ceil() -- but maybe floor() instead since we're depreciating idfk (and my more accurate check comes later anyways, this would just be a UI bug invloving the "use maximum amount" button. it might give values slightly more than what we really accept). Elaborting: I think in the 'more final' code (that uses .03 instead of .97), we're rounding up ONLY the fee... whereas here/now we're rounding up "the balance after it's multiplied by .97" -- so yea, maybe there will be cases where the max value we provide isn't then accepted when they try to use it xD. Also just now came to mind, they might not even be able to withdrawal their entire balance since there might not be an even divide OR SOMETHING (TODOreq). It would only apply to small balances anyways, most large balances should be ok, but idfk
    double userBalanceJsonDouble = satoshiStringToJsonDouble(m_AbcApp->m_CurrentlyLoggedInUsersBalanceSatoshiStringForDisplayingOnly);
    //double backwardsFeeMultipler = (static_cast<double>(1.0) - (static_cast<double>(ABC2_WITHDRAWAL_FEE_PERCENT)/static_cast<double>(100.0))); //1-(3/100) gives me 0.97, the multipler to use (i think). i call it 'backwards' multipler because the 'fowards' one would be either 1.03 or .03 -- idfk the proper terms
    //double maximumWithdrawalAmountExact = userBalanceJsonDouble * backwardsFeeMultipler;
    double forwardsFeeMultipler = (static_cast<double>(1.0) + ((static_cast<double>(ABC2_WITHDRAWAL_FEE_PERCENT)/static_cast<double>(100.0)))); //gives me 1.03
    double maximumWithdrawalAmountExact = userBalanceJsonDouble / forwardsFeeMultipler; //I am confused on what the difference of dividing by 1.03 vs. timesing by .97 even is. I grasped it momentarily and am just relying on test results xD
    //now i want to round it up to nearest satoshi i think :-/. maybe down instead (since i round UP later?)
    return maximumWithdrawalAmountExact; //i'm pretty confident that ceil()'ing this is incorrect, and i wonder if i should floor() it. BUT, seeing as "format as 8 digit double" thingo chops off the fucking shit anyways (effectively flooring it), i don't need to. Also, I checked with the second highest possible bitcoin value and it appears to have enough precision or whatever...
    //double maximumWithdrawalAmountRoundedUp = roundUpJsonDoubleToNearestSatoshi(maximumWithdrawalAmountExact);
    //return maximumWithdrawalAmountRoundedUp;
}
void WithdrawFundsAccountTabBody::handleUseMaxAmountButtonClicked()
{
    m_DesiredAmountToWithdrawLineEdit->setText(formatDoubleAs8decimalPlacesString(maximumWithdrawalAmountIncorporatingFee()));
    calculateTotalAmountWithdrawnAfterFeeForVisual();
}
void WithdrawFundsAccountTabBody::calculateTotalAmountWithdrawnAfterFeeForVisual()
{
    if(m_DesiredAmountToWithdrawLineEdit->validate() != WValidator::Valid)
    {
        m_TotalWithdrawAmountVisualOnlyLineEdit->setText("");
        m_WithdrawalFeeLineEdit->setText("");
        return;
    }
    double desiredAmountToWithdraw = boost::lexical_cast<double>(m_DesiredAmountToWithdrawLineEdit->text().toUTF8());
    double withdrawalFee = withdrawalFeeForWithdrawalAmount(desiredAmountToWithdraw);
    double totalAmountToWithdrawAfterFeeApplied = desiredAmountToWithdraw + withdrawalFee;

    m_WithdrawalFeeLineEdit->setText(formatDoubleAs8decimalPlacesString(withdrawalFee));
    m_TotalWithdrawAmountVisualOnlyLineEdit->setText(formatDoubleAs8decimalPlacesString(totalAmountToWithdrawAfterFeeApplied)); //TODOreq: even though this is visual only, it still is the amount we need to verify they have in their balance before accepting the withdraw request. Whether or not we store this amount in the withdrawal request is irrelevant and depends on what withdraw request processor expects (can simply reapply the fee). TODOopitonal: the desired amount + withdrawal fee shouldn't be able to be > 21 million bitcoins, because there can't be > 21 million bitcoins. still, the chances of that happening are lol...
    //TODOreq: should i convert to satoshis before applying the fee?
    //TODOreq: the validator should not validate if they put in 9 decimal places
}
void WithdrawFundsAccountTabBody::handleRequestWithdrawalButtonClicked()
{
    //TODOreq: sanitize fields (double for amount, bitcoin key could even rely on the bitcoin exe call that checks if valid (BUT DO PRE-SANTIZING TOO OFC BECAUSE SENDING A STRING TO A PROCESS IS DANGEROUS LOL)
    //TODOreq: verify funds amount when? a validator on the line edit would be best, but implies a db hit before showing the page. sanitizing here after the button is clicked certainly makes sense (since we do db stuff anyways). And of course sanitizing in the "withdraw request processer" app is paramount.
    //TODOreq: for now they can schedule multiple requests for the full amount of their balance. the "withdraw request processor" will simply reject any/all requests that (at the time of processing) don't have enough funds available. Ideally we would reject the scheduling of their second request (total up all previous requests, subtract from balance, see if new request is more than the difference -- reject that request if it is -- but request processing atomicity poses a problem: if I am running request app processor simultaneously == ???? fml)

    if(m_DesiredAmountToWithdrawLineEdit->validate() != WValidator::Valid)
    {
        new WBreak(this);
        new WText("Invalid Amount", this);
        return;
    }
    if(m_BitcoinPayoutKeyLineEdit->validate() != WValidator::Valid)
    {
        new WBreak(this);
        new WText("Invalid Bitcoin Key", this);
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

    //std::string desiredAmountToWithdrawJsonString = m_DesiredAmountToWithdrawLineEdit->text().toUTF8();
    //SatoshiInt desiredAmountToWithdrawInSatoshis = jsonStringToSatoshiInt(desiredAmountToWithdrawJsonString);
    double desiredAmountToWithdraw = boost::lexical_cast<double>(m_DesiredAmountToWithdrawLineEdit->text().toUTF8());
    double withdrawalFee = withdrawalFeeForWithdrawalAmount(desiredAmountToWithdraw);
    double totalAmountToWithdrawAfterFeeApplied = desiredAmountToWithdraw + withdrawalFee;
    SatoshiInt totalAmountInSatoshisToWithdrawAfterFeeApplied = jsonDoubleToSatoshiIntIncludingRounding(totalAmountToWithdrawAfterFeeApplied);

    if(userBalanceInSatoshis < totalAmountInSatoshisToWithdrawAfterFeeApplied)
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

    pt.put(JSON_WITHDRAW_FUNDS_REQUESTED_AMOUNT, boost::lexical_cast<std::string>(jsonStringToSatoshiInt(m_DesiredAmountToWithdrawLineEdit->text().toUTF8())));
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
    new WText("Withdrawal request #" + m_WithdrawRequestIndexToTryLcbAddingAt + " has been scheduled. If you schedule any additional withdrawal requests, it is up to you to ensure that the total amount for all pending withdrawal requests does not exceed your account's balance. Withdrawal requests are processed in the order they are receieved, and any withdrawal request for an amount greater than your balance will be rejected/skipped.", this);
    m_DesiredAmountToWithdrawLineEdit->setText(ABC2_MIN_WITHDRAW_AMOUNT_STR);
    m_BitcoinPayoutKeyLineEdit->setText(""); //NOPE: could also clear the bitcoin key, but chances are DECENT that they'll use it again for the next request (WHY? wouldn't they have just added that amount to the withdrawal request we just accepted? nvm clearing fuckit)
}

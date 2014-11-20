#include "withdrawfundsaccounttabbody.h"

#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WDoubleValidator>

#include "../anonymousbitcoincomputingwtgui.h"
#include "../validatorsandinputfilters/lettersnumbersonlyregexpvalidatorandinputfilter.h"

#define WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT 0.00000001 /* TODOreq:whatever minimum bitcoin transaction is (either in Abc2 or on bitcoin network) */

//The flow goes like this:
//In-Abc2: click request withdraw button, check balance is enough, get withdraw index, recursively try to lcb_add to index (+1'ing as appropriate), cas swap accepting fail withdraw index
//In-payout-processor: query a view to get all [stale=false] payout requests with state=unprocessed, iterate them. Here's one payout request flow: check balance is enough, if not set state=insufficientfunds and done. else (sufficient funds), cas swap lock profile account requiring success (fail means they are buying an ad slot or adding funds or something similar, so try again later (leave state as is, continue to next request. mb say it in stdout tho)), cas swap requiring success the payout request state to 'processing' (mb with durability poll of 3 as well but guh mb not since it isn't an lcb_add). finding a payout request in state 'processing' that we didn't JUST set to that means there was previously a fail. these are dangerous as fuck because we might pay them out twice. check bitcoin logs or idfk what and do manual ass shit to see wtf is going on. see if we paid them or what (this is why i want it to be manual). do bitcoin payout and check it's response etc (spit out errors). cas swap set payout request state to 'processed, profile needs balance deducting/unlocking". deduct amount from profile balance and unlock it. TODOreq: the profile should lock TO the specific payout request (just like buy ad slots are), which will allow us to recover somehow (can't put my finger on it, but yea. I also think it helps us safeguard against a double payout [during recovery]...). then set payout request state to 'processed and done'.
//^^so we can also query a view making sure there are no payout requests in any 'intermediate' states (indicating failure): intermediate states are processing and processed-profile-needs-blanace-deducting.

//TODOoptional: some kind of "notify me via email when the funds are sent" functionality...
//TODOreq: OT as fuck, but: I myself have no need to use the "withdraw" functionality. What this does mean however is that my "abc2" account will have assloads of unused funds, and will not accurately reflect how much I really have (since I will have spent them). If I ever want to buy an ad slot, I need to somehow actually make sure I can afford it (otherwise there will be trouble when that person requests a payout). One way to solve this is to use a different account for buying, and to never use the 'receive' funds account (dog food, etc) for buying ads
WithdrawFundsAccountTabBody::WithdrawFundsAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp)
    : IAccountTabWidgetTabBody(abcApp)
{ }
void WithdrawFundsAccountTabBody::populateAndInitialize()
{
    new WText("Withdraw Funds to Bitcoin", this);

    new WBreak(this);
    new WBreak(this);

    new WText("Use this page to request a withdrawal of funds. Requests are be processed manually, and usually within 24 hours. Automated withdrawals might come eventually, but as you might imagine it is dangerous functionality to implement.", this);

    new WBreak(this);
    new WBreak(this);

    new WText("There is a 3% fee to withdraw funds", this); //TODOoptional: if js is enabled, show them as they type in the line edit just how much they will get after the 3% fee is applied

    new WBreak(this);
    new WBreak(this);

    new WText("Amount to Withdraw:", this);
    m_AmountToWithdrawLineEdit = new WLineEdit(WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT, this);
    WDoubleValidator *bitcoinAmountValidator = new WDoubleValidator(WithdrawFundsAccountTabBody_MIN_WITHDRAW_AMOUNT, 21000000 /* TODOoptional: use their balance instead of max bitcoins lol */, m_AmountToWithdrawLineEdit);
    bitcoinAmountValidator->setMandatory(true);
    m_AmountToWithdrawLineEdit->setValidator(bitcoinAmountValidator);

    new WBreak(this);

    new WText("Bitcoin Key To Withdraw Funds To:", this);
    m_BitcoinPayoutKeyLineEdit = new WLineEdit(this);
    LettersNumbersOnlyRegExpValidatorAndInputFilter *bitcoinKeyValidator = new LettersNumbersOnlyRegExpValidatorAndInputFilter("35", m_BitcoinPayoutKeyLineEdit); //TODOreq: base58 check encoding filter instead, including minimum length (right now I only have max len)
    bitcoinAmountValidator->setMandatory(true);
    m_BitcoinPayoutKeyLineEdit->setValidator(bitcoinKeyValidator);

    new WBreak(this);

    WPushButton *requestWithdrawalButton = new WPushButton("Request Withdrawal", this);

    requestWithdrawalButton->clicked().connect(this, &WithdrawFundsAccountTabBody::handleRequestWithdrawalButtonClicked);
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

    std::string amountToWithdrawString = m_AmountToWithdrawLineEdit->text().toUTF8(); //TODOreq: still dunno if it's >= their balance, but the final check for that is in the payout request processor app anyways. all other checks are just good UI
    std::string payoutKey = m_BitcoinPayoutKeyLineEdit->text().toUTF8();


    m_AbcApp->deferRendering(); //TODOreq: resume rendering at all error paths etc
}

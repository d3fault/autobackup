#include "createbankaccountapplogicrequest.h"

#include "../applogicrequest.h"

CreateBankAccountAppLogicRequest::CreateBankAccountAppLogicRequest(WtFrontEndToAppDbMessage *networkRequestMessage)
    : AppLogicRequest(networkRequestMessage)
{
}
bool CreateBankAccountAppLogicRequest::raceCondition1detected()
{
    if(this->m_AppLogic->getListOfUsersWithBankAccount().contains(this->getNetworkRequestMessage()->m_ExtraString0))
    {
        return true;
    }
    return false;
}
bool CreateBankAccountAppLogicRequest::raceCondition2detected()
{
    if(this->getPendingBankRequests().contains(PseudoBankRequestThatEQUATEStoAnExistingOne(CreateBankAccount,user))) //user is m_ExtraString0
    {
        return true;
    }
    return false;
}
void CreateBankAccountAppLogicRequest::performAppLogicRequest()
{
    //so this performing of the app logic request will NEGATE both of the race conditions above (except we do have to wait for the response before (a) takes effect/is-negated
    //that is, what we do in here will trigger the opposites of what they detect. this way, any OF THE SAME MESSAGES that come later will detect them as race conditions
    //what this boils down to (in this case), is:
    //a) ListOfUsersWithBankAccounts will now contain the user to be created (although, this is only done AFTER we get the request back from the bank request)
    //b) PendingBankRequests will have our request to the bank server in it

    //note that for simple 'GET' requests, they both always return false. and if there is no dispatch to the bank server, (a) is negated from within this method... instead of when the bank server respondes

    //TODO: perform the request. TODO: the 'creating (and recycling) + sending' of bank requests could be done in an AppLogicRequest method... with us passing in a) the request type and b) any parameters. for now and simplity's sake... i'm going to just code it all right here in this 1 use case

    //BankServerActionRequest *bankServerActionRequest = new BankServerActionRequest();
    //TODO: maybe an inherited bank server action request just like we do for app logic requests???? idfk.



    //this->setBankServerRequest(bankServerActionRequest);
    //this->setBankServerRequest(BankServerActionRequest::getBankServerActionRequestByTheMessage(BankServerActionRequest::CreateBankAccount));

    this->setBankServerRequestTriggered(); //basically, we defer the actual doing of ANYTHING to the server (although we still do respond to it). all we do is check for the 2 race conditions that our specific to this type of request.. and then tell app logic to forward the request to the bank server. this is sort-of bad because it couples BankServerActionRequests with WtFrontEndToAppDbMessages .... gross... but also efficient(??)

    //basicallyy we just fill out a bank server request, attach it to ourselves as a child (it will have a pointer to us too), then set m_TriggeredRequestToBankServer to true. then just return; done. when we get back to AppLogic, it sees that bool is true (AND FROM THAT ALONE, KNOWS THAT OUR CHILD POINTER TO BANK SERVER REQUEST IS _NOT_ NULL (or it could be pointing to an old bank server request... which doesn't matter so long as we don't touch it)). when it sees that bool is true, it then emits the bank server request... and bam, we're  then in BankServerHelper object/thread and that's where we should do the matching up of [network replies] with our BankServerRequests (which have a child pointing to everything we need to know... our AppLogicRequest)
}

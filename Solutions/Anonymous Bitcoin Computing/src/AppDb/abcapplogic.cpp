#include "abcapplogic.h"

AbcAppLogic::AbcAppLogic(QObject *parent) :
    QObject(parent)
{
}
void AbcAppLogic::init()
{
    //TODO: i guess like init the database n shit
    emit d("starting logic init");
}
void AbcAppLogic::handleRequestFromWtFrontEnd(AppLogicRequest *request)
{
    request->processAppLogicRequest();

    //the following is pseudo-code
    if(!request->bankServerRequestTriggered()) //if we didn't dispatch a request to the bank server, that means we already have a response and should send it back to the wt front-end
    {
        emit responseToWtFrontEndReady(request->response());
    }
    else
    {
        m_ListOfPendingBankRequests.append(request->bankServerActionRequest());
        emit appLogicRequestRequiresBankServerAction(request->bankServerActionRequest());
    }
}
void AbcAppLogic::handleResponseFromBankServer(BankServerActionRequestResponse *bankResponse)
{
    int index = m_ListOfPendingBankRequests.indexOf(bankResponse->parentRequest());
    if(index > -1)
    {
        /*BankServerActionRequest *request = */m_ListOfPendingBankRequests.removeAt(index); //we don't really need this request (it's in response->parentRequest), we could just call takeAt (or maybe removeAt (assuming that doesn't delete!!!!))... but we need to take it off the pending list, because DURR it's no longer pending.
    }
    else
    {
        //TODOreq: handle error. we got a response that did not have a pending request. hack attempt?
    }

    //re: the TODOTODOTODOreq in our header... something like bankResponse->processBankServerResponse() ;-)
    //i don't think there will be any race condition detection done in there... so maybe processBankServerResponse() will be pure virtual

    //psbly do (unsure atm... though PROBABLY):
    emit responseToWtFrontEndReady(bankResponse);
}
#if 0
void AbcAppLogic::createBankAccountForUser(const QString &user, AppLogicRequestResponse *responseThatWeFillOutAndEmitBack)
{
    //TODOreq: handle the race condition that 2 (or more) wt front-ends made this request at the same time. one of them (whoever gets here second) should return an error in the response... and perhaps even a human readable error for the human (to be pushed/notified to the wt front-end). so we need to check our AppDb (the database!!).... AND we need to make sure (or maybe not? maybe the bitcoin server can return the same error in it's response) that we don't pass along a second request to the bitcoin server after the first is made [and has not yet responded].
    //^^ it's like we're handling 2 race conditions... it's just sloppy/ugly/hacky/theres-gotta-be-a-better-way. the 'or maybe not' sentence might hold true, but i need to think the fuck on it. ----- to catch the "us -> bitcoin" race conditions HERE saves bandwidth i guess? ... it requires a 'pending bitcoin server requests' list that we gotta check against before dispatching another request (to make sure it isn't a dupe)

    if(m_ListOfUsersWithABankAccount.contains(user))
    {
        //race condition where our wt front-ends haven't yet gotten the update saying the user now has a bank account (otherwise we wouldn't have gotten this message TODOreq: make sure that code is in the wt front-end. ACTUALLY, it would be wise to use the exact same error for when the wt front-end detects it and when we detect it here. error forwarding or something idk... so i do NOT fill in the human readable error here)

        //TODOreq: mark the response as unsuccessful, maybe even saying why in plain english ("bank account for user 'x' already created")?? and the emit the result and return so we don't go any further in this method
    }
    if(m_PendingBankRequests.contains(PseudoBankRequestThatEQUATEStoAnExistingOne(CreateBankAccount,user)))
    {
        //race condition where we've already made the request to create the bank account, but we haven't yet gotten the result back yet... so we got got passed the above if(m_ListOfUsersWithABankAccount.contains(user)) race condition

        //TODOreq: mark the response as unsuccessful, maybe even saying why in plain english ("duplicate bank account creation attempt detected. please wait"). the wt front-end that gets that error should still be updated/notified when the actual/first bank account creation... as should the one that made the request (TODOreq: basically just keep watching for updates even after an error... not sure if that applies to all cases or just this one... but that code won't be written for a while anyways)
        //and emit the result and return so we don't go any further
    }

    //as you can see, 2 race conditions for 1 request. if my assumptions are correct, those 2 race conditions exist for EVERY request (although just 1 for anything that doesn't need to access the bank server... and none for 'simple GETs' (reading values only)).
    //fml. is there a way to generically handle them?


    //FINALLY, if we haven't detected the two above race conditions (fml), we actually perform the operation. we need to send a request to the bank server, make note of it in pending... then handle the bank server's response... make note of it as finalized... then respond to the *APPROPRIATE* WtFrontEnd, which is accessible by just emitting responseToWtFrontEndReady. this implies we need to store the AppLogicRequestResponse somewhere in this method... and then retrieve it when we handle the bank server's response


    //tbh, i'm glad my brain can detect those race conditions. ignorance is NOT bliss. newbie programmers wouldn't see them and they would cause HELL later on (and very intermittently... i'm so glad i am smart enough to see them... even though i dread them)


    //eventually, though perhaps not necessarily in this method (or maybe both... like if we DON'T need to do anything/wait-for-a-request-from-the-server).... and i don't just mean _ONLY_ this class... but all the the "App Logic" methods..... emit the response back:
    emit responseToWtFrontEndReady(responseThatWeFillOutAndEmitBack); //i'm tempted to say i should pass back the user that the account was created for, but it's already in: responseThatWeFillOutAndEmitBack->parentRequest()->m_WtFrontEndToAppDbMessage->m_ExtraString0 .... we'd just have to know that it's there... (which we can figure out by re-examining 'theMessage' if we need to (this brings up another point... how the fuck am i going to know how to populate the AppDbToWtFrontEndMessage? i guess i'll cross that bridge [...]))
}
#endif

//there's also going to be a handleBankAccountCreated(....????...) <-idk what params yet. it will be triggered LIKE a signal.. but actually in our slot handleResponseFromBankServer.... in the response->processBankServerResponse() method.
//what i'm getting at is that like at some point we need to getAppLogicRequestResponse_BY_BankServerActionRequestResponse(bankResponse)
//should those also be associated with AppLogicRequest? not every app logic request even requires a request to the bank server, so it is slightly wasteful (because we'll have a bank server request/response allocated for _EVERY_ app logic request/response)
//perhaps the 'pending bitcoin server requests' would be something like a QHash with the AppLogicRequest(Response) as either key/value and the BankServerActionRequest(Response) as the other half (key or value... depending what makes more sense)
//i'm not sure this design is good at all. i need to zoom out and look at the big picture. i need to design it in dia. didn't i say i was going to do that a little bit ago?

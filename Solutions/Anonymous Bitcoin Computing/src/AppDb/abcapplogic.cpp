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
}
void AbcAppLogic::handleResponseFromBankServer(BankServerActionRequestResponse *bankResponse)
{
    //re: the TODOTODOTODOreq in our header... something like response->processBankServerResponse() ;-)
}
void AbcAppLogic::createBankAccountForUser(const QString &user, AppLogicRequestResponse *responseThatWeFillOutAndEmitBack)
{
    //TODOreq: handle the race condition that 2 (or more) wt front-ends made this request at the same time. one of them (whoever gets here second) should return an error in the response... and perhaps even a human readable error for the human (to be pushed/notified to the wt front-end). so we need to check our AppDb (the database!!).... AND we need to make sure (or maybe not? maybe the bitcoin server can return the same error in it's response) that we don't pass along a second request to the bitcoin server after the first is made [and has not yet responded].
    //^^ it's like we're handling 2 race conditions... it's just sloppy/ugly/hacky/theres-gotta-be-a-better-way. the 'or maybe not' sentence might hold true, but i need to think the fuck on it. ----- to catch the "us -> bitcoin" race conditions HERE saves bandwidth i guess? ... it requires a 'pending bitcoin server requests' list that we gotta check against before dispatching another request (to make sure it isn't a dupe)
}
//there's also going to be a handleBankAccountCreated(....????...) <-idk what params yet. it will be triggered LIKE a signal.. but actually in our slot handleResponseFromBankServer.... in the response->processBankServerResponse() method.
//what i'm getting at is that like at some point we need to getAppLogicRequestResponse_BY_BankServerActionRequestResponse(bankResponse)
//should those also be associated with AppLogicRequest? not every app logic request even requires a request to the bank server, so it is slightly wasteful (because we'll have a bank server request/response allocated for _EVERY_ app logic request/response)
//perhaps the 'pending bitcoin server requests' would be something like a QHash with the AppLogicRequest(Response) as either key/value and the BankServerActionRequest(Response) as the other half (key or value... depending what makes more sense)
//i'm not sure this design is good at all. i need to zoom out and look at the big picture. i need to design it in dia. didn't i say i was going to do that a little bit ago?

#include "bankserverhelper.h"

BankServerHelper::BankServerHelper(QObject *parent) :
    QObject(parent)
{
}
void BankServerHelper::connectToBankServer()
{
    //TODO
    emit connectedToBankServer();
}
void BankServerHelper::handleBankServerActionRequest(BankServerActionRequest *request)
{
    //TODO: synchronously send the request over our ssltcpclient (to be coded)
    //i'm thinking in _THIS_ class is where the 'pending requests' list should go...
    //oh fuck... wait a minute... doesn't our 2nd race condition need to check against that list??? oh fuck. maybe uhmm store it in a list in the AppLogic side JUST BEFORE emitting it over to us??

    //now that i think of it, this class too will need to keep track of the requests... and match them up with responses.
    //the same applies to the SSL Client (AppDbHelper) code i have not yet written on the Wt Front End. it will send  WtFrontEndToAppDbMessages, and then has to match up the AppDbToWtFrontEndMessage that it receives in return.
    //so what we keep track of in here is not the same as (but is similar to!!!) the list that AppLogic keeps track of (for race condition #2 purposes).
    //i'm unsure how to keep track of the specific messages. a message identifier code (int, double, string, whatever) would work.. but is slightly wasteful.
    //also sort of tangent (but i guess could apply to if/when AppDb ever scales (*gasp*... that sounds so unfeasible atm))........ but definitely relates to current config of multiple wt front-ends: the BROADCAST/UPDATE/NOTIFY messages have no use for a message identifier... as no request initiated them. in those instances they are very wasteful and completely unnecessary.

    //i could use the pointer to the request object... the address of the request object itself... as the identifier. this makes me lawl and gag and choke and yet... i think it would work. rofl. the only potential pitfall is 32/64-bit compatibility
    //ok fuck thinking about that anyways

    //my main question: is there any way to implicitly deduce who/what object the request came from. you get a response, can you be all like 'oh lol that OBVIOUSLY goes with this request'?






    //for debugging purposes, just emit a response lol...
    BankServerActionRequestResponse *response = new BankServerActionRequestResponse(); //this will probably be a child of and/or allocated/RECYCLED with the request. since every bank request merits a response. (tangent: not every app logic request merits a bank server request... hence the bool m_TriggeredBankResponse)
    emit responseToAppLogicReady(response);
}

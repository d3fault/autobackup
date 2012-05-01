#include "applogicrequest.h"

#include "AppLogicRequests/createbankaccountapplogicrequest.h"

AppLogicRequest::AppLogicRequest()
    : m_BankServerRequestTriggered(false), m_BankServerActionRequest(0)
{
    m_NetworkRequestMessage = new WtFrontEndToAppDbMessage();
    m_AppLogicRequestResponse = new AppLogicRequestResponse(this);
    //TODO: setDefaultValues should be called both here and before returning the takeLast() below. it sets things like somethingChanged (etc) to false;
}
AbcAppLogic *AppLogicRequest::m_AppLogic = NULL;
QList<AppLogicRequest*> AppLogicRequest::m_RecycledAppLogicRequests = QList<AppLogicRequest*>(); //extra copy but fuck it, only once per app
#if 0
AppLogicRequest *AppLogicRequest::fromWtFrontEndToAppDbMessage(WtFrontEndToAppDbMessage message)
{
    //TODOopt: from here we can manage a recycled queue of already new'd objects or something if we want
    AppLogicRequest *request = new AppLogicRequest();

    //ok so blah this method is pointless and actually it seems as if having a network protocol and an inter-object message protocol is a waste of time
    //no matter what i'm going to have to switch(message) somewhere and call individual methods... where i do it is all that really matters. no point in changing the type
    //ALTHOUGH it might be good for decoupling. for example i may want to process a request reply before sending it over the network. i mentioned earlier that it will have a boolean somethingChanged in it... and if it's true then the reply is broadcast to every connected wt front-end. so that somethingChanged has no place in the network protocol... only in the inter-object protocol. that's the only example i can think of but there could be tons more once i write more code...
    //i DO like the idea of having the conversion code in these static methods... but i DON'T like that i'm going to have to parse the shit twice

    //ok i came up with this while sleeping and i liked it: basically AppLogicRequest is allocated here (***OR RECYCLED***)... as is the AppLogicRequestResponse (which is also recycled). AppLogicRequestResponse and WtFrontEndToAppDbMessage are merely contained as pointers inside the AppLogicRequest. they can additionally contain additional members. for example, the AppLogicRequestResponse will contain a bool somethingChanged (for deciding whether to reply only or if to broadcast)... and AppLogicRequest can have a WtFrontEndIdentifier (QSslCertificate.serialNumber().toUInt() atm)
    //this solution is nice because of it's efficiency only. even though we are emitting AppLogicRequest accross threads, we know that when the AppLogicRequestResponse is emitted back (they can both contain pointers to each other i guess)... we will still be on this thread. and we can also easier deal with the recycling/delete'ing of the objects
    //the caching will be rather simply actually: i check if a queue is nonzero. zero = allocate new. nonzero = deQueue it. BOTH CASES = use the same initialize (somethingChanged = false, for example) method........... and later on after we get the response emitted back to us (and probably after we've responded over the network too), we simply put the AppLogicRequest in the queue.
    //this does mean i need to get the WtFrontEndToAppDbMessage FROM THIS CLASS (either new or cached) before i can even read it off the network

    //this works well for efficiency (although i may just be prematurely optimizing -_-)... but it does NOTHING to solve the coupling issues
    //i _could_ have extraction methods in AppLogicRequest... so only AppLogicRequest has to know about WtFrontEndToAppDbMessage. this sounds fine in theory when extracting doubles, strings, ints, etc.... but when extracting 'theMessage'... or what the request actually is... i'm still going to need a switch statement somewhere. AppLogic might need the Wt <--> AppDb protocol header so it knows the enums and can switch() accordingly. I don't like this. defeats the purpose of the separating. i might as well just pass AppLogic the WtFrontEndToAppDbMessage and let it process that directly. the only difference is the additional variables... which i guess KINDA help with decoupling/organizing
    //if only i could make AppLogicRequest be the one to do the switch()... then it would be perfect. this i probably could. AppLogic gets the AppLogicRequest and then calls processRequest(this); on it (with 'this' being AppLogic itself... or AppLogic could be a singleton idgaf). Or i could set a static AppLogic pointer during the init phase... (just increasing efficiency here... something a compiler/profiler does way better (idk though i mean it's not like a compiler optimizes memory usage/allocating. you have to analyze/redesign based on the output of a profiler (or a simple memory usage view) to do that. and the optimizations i'm making MAKE SENSE to me. kinda like how when i did video i made it recycle the video frames. no fucking profiler/compiler told me to do it... it just made sense. this being a high bandwidth/performance server app i definitely think it should be optimized))
    //perfect. right after i 'new' AppLogic, i'll set AppLogicRequest::AppLogicPtr...
    //then i just call AppLogicRequest.processRequest(); ... and AppLogicRequest (which CAN/SHOULD know AppLogic methods) does the switch on WtFrontEndToAppDbMessage (which it also definitely knows... hell i'm writing in it right now) and bam, that's IT.
}
#endif
void AppLogicRequest::setAppLogic(AbcAppLogic *appLogic)
{
    m_AppLogic = appLogic;
}
void AppLogicRequest::processAppLogicRequest()
{
    //instead of doing switch()'ing here, each app logic request could inherit from this type and override a pure virtual method performAppLogicRequest(AppLogicRequestResponse *response)=0. we could also have a regular virtual methods (with default implementation always returning false) raceCondition1detected() and raceCondition2detected(). race condition 1 = same request sent from wt server to appdb server because appdb response hasn't yet been received by wt server. if it had been, the race condition request would never have left the wt server (TODO: verify this is in the wt code). race condition 2 = an appdb request that triggered a request to the bank server has yet to hear back from the bank server.. and the same request came into the appdb before the bank server responded
    //esssentially, the contents of createBankAccountForUser would be abstracted (and put RIGHT HERE i guess)... with the if, elseif, and actually performing of action into virtual methods that we call. this does fuck up my recycling design though... BAH. FUCK. AN HERO TIEM. god i'm so dumb. KISS MOTHER FUCKER. DESIGN MOTHER FUCKER. YOU ARE MAKING THIS UP AS YOU GO NO WONDER YOU ARE GETTING FURTHER AND FURTHER LOST. at least the ssl server code i wrote is good. haven't written much else. may have to start over is what i'm getting at. i mean not entirely (main + thread init'ing is fine so far)... just with this OurServerForWtFrontEnds --(AppLogicRequest)--> AppLogic code/design [and included recycling] bullshit. that's what it is: bullshit.

    //we need to make sure that this is _NEVER_ called from anywhere but the AppLogic object/thread... since it operates directly on our AppLogic object. AS OPPOSED TO the giveMeAnAppLogicRequest() and returnAnAppLogicRequest()... which should _ONLY_ be called from the OurServerForWtFrontEnds object/thread (so we don't need mutex's to protect the list)
#if 0
    switch(m_NetworkRequestMessage->m_TheMessage)
    {
    //TODOopt: re-arrange these cases based on usage statistics. we probably won't be creating bank accounts nearly as much as we'll be GET'ing some random piece of data...
    case WtFrontEndToAppDbMessage::CreateBankAccountForUserX:
    {
        m_AppLogic->createBankAccountForUser(m_NetworkRequestMessage->m_ExtraString0, m_AppLogicRequestResponse); //we send the response so they can fill it out and tell OurServerForWtFrontEnds the response (by emitting it back). the response has a pointer to the request, and vice versa
            break;
    }
    case WtFrontEndToAppDbMessage::InvalidWtFrontEndToAppDbMessageType:
    default:
        //TODO: ERROR
        break;
    }
#endif


    //TODOreq: going into this, m_TriggeredRequestToBankServer should always be false (though right here is not the place to do it. somewhere in/near the recycling code or something). only in performAppLogicRequest can it be set to true. after this method returns, AppLogic checks it. if true, does nothing. if false, emits the response back to OurServerForWtFrontEnds

    if(this->raceCondition1deteced())
    {
        //handle it, perhaps with a PURE VIRTUAL response string?? idfk.
    }
    else if(this->raceCondition2deteced())
    {
        //handle it
    }
    else //neither race condition detected
    {
        this->performAppLogicRequest();
    }
}
uint AppLogicRequest::getRequestorId()
{
    return m_RequestorId;
}
AppLogicRequest * AppLogicRequest::giveMeAnInheritedAppLogicRequestBasedOnTheMessage(WtFrontEndToAppDbMessage *message)
{
    //TODOreq: make sure it isn't an invalid theMessage. if it is, just return NULL; TODOreq: make sure accessing calls check for null ;-)
    if(m_RecycledAppLogicRequestsByTheMessage.contains(message->m_TheMessage))
    {
        //we have previously used this 'theMessage' type before... but that doesn't necessarily mean we have any currently in our QList value ready to be recycled
        QList<AppLogicRequest*> *hashValue = m_RecycledAppLogicRequestsByTheMessage.value(message->m_TheMessage);
        if(!hashValue->isEmpty())
        {
            AppLogicRequest *requestRecycled = hashValue->takeLast();
            requestRecycled->setNetworkRequestMessage(message);
            return requestRecycled;
        }
    }

    switch(message->m_TheMessage)
    {
        //the only thing i don't like about this new design is that i have to know each of my child/derived types in order to switch()/return new them.....  a parent should never know it's children (lol)
    case WtFrontEndToAppDbMessage::CreateBankAccountForUserX:
        return new CreateBankAccountAppLogicRequest(message);
        break;
    case WtFrontEndToAppDbMessage::InvalidWtFrontEndToAppDbMessageType: //TODO: this really isn't necessary if we already check it above at the top of this method... but i mean i guess it's sort of extra safety?? is also isn't worth writing this comment.
    default:
        return NULL;
        break;
    }
    return NULL;
}
void AppLogicRequest::setNetworkRequestMessage(WtFrontEndToAppDbMessage *networkRequestMessage)
{
    m_NetworkRequestMessage = networkRequestMessage;
}
void AppLogicRequest::returnAnInheritedAppLogicRequest(AppLogicRequest *appLogicRequest)
{
    //TODOmb: setDefaults(appLogicRequest)
    appLogicRequest->unSetBankServerRequestTriggered(false); //setting this to false in effect disassociates (lol) the app logic request with the bank server request (even though we are still pointing to it!!! un-pointing to it is not a necessity so long as we make sure that we point to a new [albeit recycled] one every time we set triggered to true)

    if(m_RecycledAppLogicRequestsByTheMessage.contains(appLogicRequest->m_NetworkRequestMessage->m_TheMessage))
    {
        //already in the hash, so the list is already instantiated. just append ourselves to it
        m_RecycledAppLogicRequestsByTheMessage.value(appLogicRequest->m_NetworkRequestMessage->m_TheMessage)->append(appLogicRequest);
    }
    else
    {
        //else, not in the hash, so therefore no list has been instantiated either
        //instantiate it, append ourselves to it, then insert it into the hash
        QList<AppLogicRequest*> *newHashValue = new QList<AppLogicRequest*>();
        newHashValue->append(appLogicRequest);
        m_RecycledAppLogicRequestsByTheMessage.insert(newHashValue);
    }

    //THEN, return/recycle the underlying network request. since the app logic request is being recycled, we know that the underling network request that triggered the app logic request can also be recycled
    WtFrontEndToAppDbMessage::returnAWtFrontEndToAppDbMessage(appLogicRequest->getNetworkRequestMessage());
}
WtFrontEndToAppDbMessage *AppLogicRequest::getNetworkRequestMessage()
{
    return m_NetworkRequestMessage;
}
void AppLogicRequest::unSetBankServerRequestTriggered()
{
    m_BankServerRequestTriggered = false;
}
bool AppLogicRequest::bankServerRequestTriggered()
{
    return m_BankServerRequestTriggered;
}
void AppLogicRequest::setBankServerRequest(BankServerActionRequest *bankServerRequest)
{
    m_BankServerActionRequest = bankServerRequest;
}
BankServerActionRequest * AppLogicRequest::bankServerActionRequest()
{
    return m_BankServerActionRequest;
}
void AppLogicRequest::setBankServerRequestTriggered()
{
    m_BankServerRequestTriggered = true;
}

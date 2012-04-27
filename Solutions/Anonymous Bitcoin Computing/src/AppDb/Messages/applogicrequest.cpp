#include "applogicrequest.h"

AppLogicRequest::AppLogicRequest()
{
    m_WtFrontEndToAppDbMessage = new WtFrontEndToAppDbMessage();
    m_AppLogicRequestResponse = new AppLogicRequestResponse(this);
    //TODO: setDefaultValues should be called both here and before returning the takeLast() below. it sets things like somethingChanged (etc) to false;
}
AbcAppLogic *AppLogicRequest::m_AppLogic = NULL;
QList<AppLogicRequest*> AppLogicRequest::m_RecycledAppLogicRequests = QList<AppLogicRequest*>(); //extra copy but fuck it, only once per app
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
AppLogicRequest * AppLogicRequest::giveMeAnAppLogicRequest()
{
    if(!m_RecycledAppLogicRequests.isEmpty())
    {
        return m_RecycledAppLogicRequests.takeLast(); //why last? JUST IN CASE taking first would trigger a re-order. it DOES NOT MATTER which one i take. i originally thought this should be a queue but it really doesn't matter at all
    }
    return new AppLogicRequest();
}
void AppLogicRequest::setAppLogic(AbcAppLogic *appLogic)
{
    m_AppLogic = appLogic;
}
void AppLogicRequest::returnAnAppLogicRequest(AppLogicRequest *appLogicRequest)
{
    m_RecycledAppLogicRequests.append(appLogicRequest);
}
void AppLogicRequest::processAppLogicRequest()
{
    //we need to make sure that this is _NEVER_ called from anywhere but the AppLogic object/thread... since it operates directly on our AppLogic object
    switch(m_WtFrontEndToAppDbMessage->m_TheMessage)
    {
    //TODOopt: re-arrange these cases based on usage statistics. we probably won't be creating bank accounts nearly as much as we'll be GET'ing some random piece of data...
    case WtFrontEndToAppDbMessage::CreateBankAccountForUserX:
    {
        m_AppLogic->createBankAccountForUser(m_WtFrontEndToAppDbMessage->m_ExtraString0, m_AppLogicRequestResponse); //we send the response so they can fill it out and tell OurServerForWtFrontEnds the response (by emitting it back). the response has a pointer to the request, and vice versa
            break;
    }
    case WtFrontEndToAppDbMessage::InvalidWtFrontEndToAppDbMessageType:
    default:
        //TODO: ERROR
        break;
    }
}

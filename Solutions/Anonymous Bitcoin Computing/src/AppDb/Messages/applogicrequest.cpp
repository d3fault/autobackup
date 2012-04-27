#include "applogicrequest.h"

AppLogicRequest::AppLogicRequest()
{
}
AppLogicRequest *AppLogicRequest::fromWtFrontEndToAppDbMessage(WtFrontEndToAppDbMessage message)
{
    //TODOopt: from here we can manage a recycled queue of already new'd objects or something if we want
    AppLogicRequest *request = new AppLogicRequest();

    //ok so blah this method is pointless and actually it seems as if having a network protocol and an inter-object message protocol is a waste of time
    //no matter what i'm going to have to switch(message) somewhere and call individual methods... where i do it is all that really matters. no point in changing the type
    //ALTHOUGH it might be good for decoupling. for example i may want to process a request reply before sending it over the network. i mentioned earlier that it will have a boolean somethingChanged in it... and if it's true then the reply is broadcast to every connected wt front-end. so that somethingChanged has no place in the network protocol... only in the inter-object protocol. that's the only example i can think of but there could be tons more once i write more code...
    //i DO like the idea of having the conversion code in these static methods... but i DON'T like that i'm going to have to parse the shit twice
}

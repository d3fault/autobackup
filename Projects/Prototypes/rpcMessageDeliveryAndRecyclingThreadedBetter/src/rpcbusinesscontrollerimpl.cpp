#include "rpcbusinesscontrollerimpl.h"

RpcBusinessControllerImpl::RpcBusinessControllerImpl(IRpcBusiness *business)
    : IRpcBusinessController(business)
{
    //I'm wondering if in here is a better place to make the connections between IRpcBusinessController and IProtocolKnower (the interface that ServerAndProtocolknower implements. it then hasA 'SslTcpServer' and uses it extensively in the pure virtual IProtocolKnower method overrides). Here rather than in the IRpcBusinessController constructor. I did say (in the header) that only this impl would be the one to hasA ServerAndProtocolKnower

    //see, i can put the IProtocolKnower as a member in IRpcBusinessController... then instantiate ServerAndProtocolKnower in this impl...
    //...but there's no reason that the IRpcBusinessController constructor couldn't then make the connections. except.. well... for the fact that ServerAndProtocolKnower won't exist (null pointer until i new it in this constructor, which happens AFTER the IRpcBusinessController constructor).

    //so i can put the 'new ServerAndProtocolKnower' in here and then call a method on IRpcBusinessController to do the connections...
    //...or i can just do the connections right here

    //does it even really matter?

    //does anything matter?

    //what is the meaning of all this?

    //to make a buck

    //for what?

    //to fund projects that do not have business models but which are necessary to improve communication throughout the world and hopefully solve corruption, world hunger, and err... two personal objectives

    //so, does it matter where i put the connect() calls?

    //nope

    //i'd say just put them in the interface. why? just because. beecause IRpcBusinessController provides the signals/slots that will be connected... and because i can. it potentially saves time later if i re-use the class (but seeing as this is all going to be auto-generated, i doubt it)

    //and i can easily move it to this impl if i ever need to

    //what is the meaning of life?

    //why am i going so slow?


    m_Transport = new ServerAndProtocolKnower();

    setupConnections();

    //m_ProtocolKnower = new ServerAndProtocolKnower();

    //connectActionsAndBroadcastsToFromBusinessAndProtocolKnower();


    //i just have one question left: isn't IRpcBusinessController the same thing as IProtocolKnower?












    //ITransmit
    //we ARE IProtocolKnower
    //so our base needs to know of an ITransmitter (the rpc mechanism, network to start)
    //but we new the ITransmitter pointer in this impl constructor as a .... I DON'T KNOW ROFL. SslTcpServer ?
    //it would reallyb e ITransporter
    //since it both sends and receives

    //should the ITransporter be the layer that remembers 'who to respond to'
    //or should my IRpcBusinessController be the one?
}

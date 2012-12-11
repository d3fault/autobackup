#ifndef IRPCBANKSERVERBUSINESS_H
#define IRPCBANKSERVERBUSINESS_H

#include <QObject>

#define WE_ARE_RPC_BANK_SERVER_CODE

//TODOreq: add our messages etc to the meta object system. register() or something i forget. I might be able to do it in a file that's shared between client/server... but meh who cares for now. This is our entry point atm so I'll just leave this note here.
//Come to think of it how the fuck did the code work BEFORE? Is it because I'm emitting pointers, which are basically Ints?

#include "iemitrpcbankserveractionrequestsignalswithmessageasparamandiacceptalldeliveries.h"
#include "MessagesAndDispensers/Dispensers/rpcbankserveractiondispensers.h"
#include "MessagesAndDispensers/Dispensers/rpcbankserverbroadcastdispensers.h"

class IRpcBankServerBusiness : public QObject
{
    Q_OBJECT
public:
    //the reason for the two following virtuals is so our rpc generated code will call them (it still assumes (well, not enetirely) they are used correctly) in the correct order. because if we move the backend business objects, they can't "pull" the thread ownership (or something. idfk. taking safest route possible)
    virtual void instructBackendObjectsToClaimRelevantBroadcastDispensers()=0; //broadcasts for rpc server impl
    virtual void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()=0;
    virtual void connectRpcBankServerClientActionRequestSignalsToBankServerImplSlots(IEmitRpcBankServerActionRequestSignalsWithMessageAsParamAndIAcceptAllDeliveries *actionRequestSignalEmitter)=0;

    void setBroadcastDispensers(RpcBankServerBroadcastDispensers *broadcastDispensers);
protected:
    RpcBankServerBroadcastDispensers *m_BroadcastDispensers;
public slots:
    virtual void init()=0;
    virtual void start()=0;
    virtual void stop()=0;
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();

    //i'm tempted to put the broadcasts here, because it almost makes sense for the rpc client to listen to signals. but instead i'm going to pass in the IRpcBankClient AppDb to the RpcBankServerHelper and let RpcBankServerHelper's constructor connect to my impl's pure virtual slots for receiving
    //so does that mean i should have IRpcBankClient use the exact functionality of IRpcBankServerMessageTransporter? I think yes. but no the Transporter is also what i have doing the signals for the emit. is that incorrect?
    //i guess it depends on how i _CHOOSE_ the rpc client to function. do i want to emit a signal to initiate a request? maybe i just use the dispensers and then call .deliver() on them... and on the client side they are rigged to go to the right place that sends them to the server

    //so it's really about my .deliver() being able to be sent to a name-friendly client/server shared interface
    //rpc client: createBankAccount.deliver() goes to IRpcBankServer <ActionName> slot in the RpcBankServerHelper impl
    //rpc server: createBankAccount.deliver() goes to IRpcBankServerMessageTransporter <ActionName>Completed slot in the SslTcpServerAndProtocolKnower impl


    //almost like i need a...
    //IAcceptMessageDeliveriesGoingToRpcBankServer
    //and
    //IAcceptMessageDeliveriesGoingToRpcBankClient
    //and they are opposite interfaces


    //by my random guessing, there should be 2 impls of each IAccept* above
    //they both might derive froma  base IAcceptMessageDeliveries -- which is just a common place to include all the custom message types (maybe?)

    //"slot" / "request" "createBankAccount"
    //impl 1 client -> server: RpcBankServerHelperImpl (on rpc client) -- the IRpcBankClient impl calls .deliver() on dispensed messages
    //impl 2 client -> server: IRpcBankServer (on rpc server) (our impl inherits IRpcBankServer which inherits IAcceptMessageDeliveriesGoingToRpcBankServer. in the bank server impl, we aren't going to use the deliver() functionality because we are auto-generated code. but we still happen to use the same interface for it)

    //"signal" / "response" (or broadcast) "createBankAccountCompleted"
    //impl 1 server -> client: IRpcBankServerMessageTransporter (on rpc server) -- the IRpcBankServer impl calls .deliver on pending requests (responds to them) or broadcasts (broadcasts them)
    //impl 2 server -> client: IRpcBankClient (on rpc client) -- same scenario, the impl doesn't know about IAcceptMessageDeliveriesGoingToRpcBankClient and doesn't use deliver (though it could -- TODOoptimization: make it so they can't. kind of security risk? deliver() would at this point just re-send the just-received request. we should not allow this)

    //fuck yea was a random guess but i was right...

    //the server -> client shit above is actually better understood by thinking of it as request -> requestHandling and responseHandling <- response
    //confusing a bit but makes sense

    //so my IAccept* classes are on both the rpc client and server, they are shared


    //rpc server impl actions have message passed as arg and is IAcceptMessageDeliveriesGoingToRpcBankServer
    //rpcserverhelper actions are .deliver()'d with no message and also inherit IAcceptMessageDeliveriesGoingToRpcBankServer

    //rpc client impl broadcasts and action responses have message passed as arg and is IAcceptMessageDeliveriesGoingToRpcBankClient
    //rpc server impl broadcasts and action responses are .deliver()'d with no message and also inherit IAcceptMessageDeliveriesGoingToRpcBankClient

    //so basically, i can't use IAccept bullshit. fml.
    //yes, the interfaces are equivalent
    //but they differ only in that there is no message as a parameter...

    //essentially, whenver auto-generated code gives us a message, we just do .doneWithMessage() on it after we're done processing it and it recycles
    //and whenever we use a dispenser, we just do .deliver()

    //auto-generated code gives us a message as a parameter (it doesn't have to, i could do sender() casting, but making the user do it is ugly)
    //user code just uses .deliver(), so we never emit action(message); like i was originally thinking a while ago. not for action requests, responses,  nor broadcasts

    //god damn prematurely optimizationing gets me again !!!!!!!!!

    //so i can't use any inheritence among server/client then?
    //i _COULD_, but don't want to. because i want to use .deliver()

    //RpcServerHelper in rpc client 'could' inherit ibankserver.h ... which is what the rpc server impl inherits. they are exactly the same, EXCEPT FOR the lack of a message in the parameter. rpc server impl has one, RpcServerHelper does not. hell, it doesn't even have public slots. it just rigs connections etc between an internal class as the passed in IRpcClientImpl (just like on server side, to set up Action Dispensers properly)

    //============================ACTION SIGNAL/SLOTS SIGNATURES============(note none matching)

    //on rpc client
    //--deliver();
    //slot RpcBankServerHelper::createBankAccount(); //deliver() gets us here and we call it from anywhere in rpc client impl. the initial request.
    //--then we wait
    //signal RpcBankServerHelper::createBankAccountCompleted(CreateBankAccountMessage*); //emit signal when reading response from network
    //slot IRpcBankServerClientImpl::handleCreateBankAccountCompleted(CreateBankAccount*); //RpcBankServerHelper triggers this slot when it receives a response from network

    //on rpc server
    //signal RpcClientsHelper::createBankAccount(CreateBankAccountMessage*); //emit this signal when network message received
    //slot IRpcBankServer::createBankAccount(CreateBankAccountMessage*); //rpc clients helper emits these to us
    //--deliver();
    //slot RpcClientsHelper::createBankAccountCompleted(); //deliver() gets us here. the response/initial broadcast

    //notice that, although being very fucking similar, none of the above 6 have matching signatures??? i rest my case. IAccept* sucks.

    //============================BROADCAST SIGNAL/SLOTS SIGNATURES============
    //on rpc server in some business impl object/thread
    //--deliver();
    //slot RpcBankServerClientsHelper::pendingBalanceDetected(); //this is initial request triggered by .deliver(). we do sender() to get access
    //write to network in above slot
    //signal RpcBankServerHelper::pendingBalanceDetected(PendingBalanceDetectedMessage*);
    //slot IRpcBankServerClientImpl::handlePendingBalanceDetected(PendingBalanceDetectedMessage*); //we just call .doneWithMessage() afterwards and it recycles itself

    //again, same with broadcasts... no situation where the signatures are matching

    //ok so that means i get to fucking tear down


    //hmm so broadcasts definitely need a different interface on each side
    //but do actions?
    //technically, no... but the naming of the signals/slots is not consistent. one is a completed() signal and the other is just the request still going to the impl but it is a signal. how can i merge the two's names? is it worth it?

    //i say fuck it. it isn't worth fighting logic / my brain just to use polymorphism. man i'm so stupid sometimes.
};


#endif // IRPCBANKSERVERBUSINESS_H

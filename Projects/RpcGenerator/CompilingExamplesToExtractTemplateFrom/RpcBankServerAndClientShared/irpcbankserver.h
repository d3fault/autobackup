#ifndef IRPCBANKSERVER_H
#define IRPCBANKSERVER_H

#include <QObject>

#include "iacceptmessagedeliveriesgoingtorpcbankserver.h"

class IRpcBankServer : public IAcceptMessageDeliveriesGoingToRpcBankServer
{
    Q_OBJECT
public:
    //the reason for the two following virtuals is so our rpc generated code will call them (it still assumes (well, not enetirely) they are used correctly) in the correct order. because if we move the backend business objects, they can't "pull" the thread ownership (or something. idfk. taking safest route possible)
    virtual void instructBackendObjectsToClaimRelevantDispensers()=0; //broadcasts for rpc server impl, actions for rpc client impl
    virtual void moveBackendBusinessObjectsToTheirOwnThreadsAndStartTheThreads()=0;

    void setActionDispensers(ActionDispensers *actionDispensers);
    ActionDispensers *actionDispensers();

    void setBroadcastDispensers(BroadcastDispensers *broadcastDispensers);
    BroadcastDispensers *broadcastDispensers();
protected:
    ActionDispensers *m_ActionDispensers;
    BroadcastDispensers *m_BroadcastDispensers;
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
};


#endif // IRPCBANKSERVER_H

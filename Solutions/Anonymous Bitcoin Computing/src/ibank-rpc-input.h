#ifndef IBANKRPCINPUT_H
#define IBANKRPCINPUT_H

#include <QObject>

class IBank : public QObject
{
    Q_OBJECT
public slots:
    virtual void createBankAccount(const QString &username)=0; //this far in I already wonder what the parameters should be... for example, if a first parameter needs to be a request identifier (or a request object... but then the RPC auto-gen'd impl will depend on object code that the RPC server impl will use directly). username makes sense... but how will the auto-gen'd rpc impl know which signal/response goes with which slot/request??

    //ok this really boils down to 2 problems. getting the reply matching back up with the request.... AND getting the right appId. it all depends on what design I CHOOSE TO TAKE. there is no right/wrong way (there is only better and worse)
    //to help me understand/future-proof, i'm going to put the equivalent slot/request that i will be putting in my IAppDb (TODO: IAppDb only has createBankAccount etc and IAbcAppDb has all the ABC specific slots. AbcAppDb rpc server impl inherits both):

    //virtual void createBankAccount(const QString &username); //see, even on AbcAppDb, i have the same problem. but on appdb, it only boils down to one issue: getting the reply to match back up with the request. oh and ACTUALLY, we'd be broadcasting the information to everyone [else?] after/during? the reply. there is no appId in that context... but there is a wtId. HOWEVER, the wtId is not used to determine a sub-appdb like the appId is used to determine a sub-bank-account

    //so i want a future proof/decouple-able design that encompasses both use cases

    //-----BANK------
    //for bank rpc server impl, the AppDbClientsHelper could have multiple instances... once per appId. as well as multiple ibank impelementaton INSTANCES, one per appId. this way the signals/slots don't need to care about what appId they are associated with... as that's done in an earlier stage before it reaches it's AppDbClientsHelper instance
    //this does not help with the request/reply... but i don't think it should matter, so long as i pass whatever parameters back in the resulting signal

    //------APPDB------
    //for appdb rpc server impl, the WtFrontEndsClientsHelper could also use the same multiple instances~~~~~~~
    //........ACTUALLY NO IT COULDN'T, as there is only ever 1 AbcAppDb object. it is the database access that each wt front end shares
    //so AppDb would definitely need some kind of wt front-end identifier in order for the request to be given back to the right user (not all of them are broadcasted, but a lot of them are)
    //for broadcasting, we could have the rpc compiler generate duplicates of all the signals. one for regular reply mode and one for reply + broadcast mode

    //EDIT
    //if the AppDb ever splits in order to scale (lol good luck with that), then the Bank also needs to incorporate broadcast mode and it should use it for every single per-appId bank action

    //bank use case is simpler
    //but in the end, they are both very similar

    //in both cases, i think replying to the requestor FIRST before broadcasting is an ideal priority to improve responsiveness of user interactions. the user that makes a request will most likely be the first user to actually see a response... and so that response should have priority over the [probably not seen] broadcast of the same message

signals:
    void bankAccountCreated(const QString &username);
};

#endif // IBANKRPCINPUT_H

#ifndef IBANKSERVERPROTOCOLKNOWER_H
#define IBANKSERVERPROTOCOLKNOWER_H

#include <QObject>

#include "../../../RpcBankServerAndClientShared/iacceptrpcbankserveractiondeliveries.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/createbankaccountmessage.h"
#include "../../../RpcBankServerAndClientShared/MessagesAndDispensers/Messages/Actions/getaddfundskeymessage.h"

class IBankServerProtocolKnower : public IAcceptRpcBankServerActionDeliveries
{
    Q_OBJECT
public:
    explicit IBankServerProtocolKnower(QObject *parent = 0);
signals:
    //incoming action requests
    void createBankAccount(CreateBankAccountMessage *createBankAccountMessage);
    void getAddFundsKey(GetAddFundsKeyMessage *getAddFundsKeyMessage);
public slots:
    void createBankAccountDelivery(); //deliver'd from rpc server impl. our IRpcBankServerClientProtocolKnower on rpc client also inherits IAcceptRpcBankServerActionDeliveries
    void getAddFundsKeyDelivery();

    //errorrs, delivered similarly to .deliver()
    void createBankAccountFailedUsernameAlreadyExists();
    void createBankAccountFailedPersistError(); //for some reason i don't like this error anymore. perhaps because it is not an error you'd want to show a user
    void getAddFundsKeyFailedUseExistingKeyFirst();
    void getAddFundsKeyFailedWaitUntilConfirmed();

    //my IAcceptRpcBankServerActionDeliveries doesn't account for errors atm, fml. actually i don't think it matters. actually yes it does. because my CreateBankAccountMessage on the rpc client has no need for rigging the errors like deliver()'ing... we just emit them (the errors) as a parameter to the rpc client impl
    //i don't want to have 2 sets of messages. bullshit. reallly though, who gives a shit. auto-generated is auto-generated
    //fml i am overcomplicating the FUCK out of this


    //---------------------------------
    //RpcClientActionRequest -- no need for errors or even for the client to see "recycle" (but no need to hide it). the response is allocated but hidden for now. it does not get sent across the network, only the request params do

    //RpcServerActionMessage -- request params const (read-only), response params accessible, expected to be filled out (can be forced (throwing errors if improperly configured))
    //RpcServerBroadcast -- errors? delivering && recycling definitely.

    //RpcClientActionResponse -- both request and response values read-only
    //RpcClientBroadcast -- errors (if applicable). NO NEED FOR DELIVERING. recycling still used

    //from those 5 message types / message use cases, we should be able to define a perfect class inheritence heirchy

    //similarities amongst all 5: recycling, serializable. this is 2/3 features that currently compose IMessage. the third being .deliver()'ing... which only the RpcClientBroadcast doesn't need. 4/5 it might just be simplest to just leave it in there~

    //similarities amongst Client Code: same as the similarities between all 5. just recycling, serializable. there is a level of abstraction where the response is hidden in the case we are an action request (there doesn't have to be)
    //similarities amongst Server Code: every message is .deliver()'able. maybe that's an inherent property of being a server. and basically the other 2 (recycleable,serializable) apply and that's it.

    //similarities amongst Action Messages: the request params are always read-able at the very least, always recycleable
    //similarities amongst Broadcast Messages: always recycleable -- really that's it. broadcasts on the client are completely different. the params are read-only and there is no deliver. though having both of those not coded is not a big deal.

    //^^wrote "similarities amongst" for every category before actually filling them in. ok filled in the 'all 5' one first. but liked the idea a lot and figured i'd forget the 4 distinct categories


    //time for a mother fucking grid box, my eyes are too lazy and my brain is too disconnected and lazy to want to manually figure out the class inheritence shits

    /*

                                Deliverable     Recycleable     Serializable    OtherHalfHidden     CertainHalfReadOnly     BothHalvesReadOnly
      RpcClientActionRequest    Y               Y               Y               Y                   N                       N
      RpcServerActionMessage    Y               Y               Y               N                   Y                       N
      RpcServerBroadcast        Y               Y               Y               n/a                 n/a                     n/a
      RpcClientActionResponse   N               Y               Y               N                   N                       Y
      RpcClientBroadcast        N               Y               Y               n/a                 n/a                     n/a || Y

      brain is in autopilot mode. relaxing.


      so among the Actions vs. Broadcasts, the main characteristic is that Actions actually have an answer to the Hidden/Read-Only "public API nice'ness"

      err nvm just realized the BothHalvesReadOnly dilema for RpcClientBroadcast. such trivial problems i am overcomplicating

      but how do i want to deal with it?

      man i was stupid for even thinking they'd all be able to use one interface: IMessage

      unless IMessage just does the Recycling and Serialization

      class IMessage : IRecyclable, ISerializable <-- really the code for it is the IMessage impl
      {
      }

      class IDeliverable : IMessage
      {
      }

      class IRpcClientActionRequest : IDeliverable
      {
        request params public, response allocated but private
      }

      class IRpcServerActionMessage : IDeliverable
      {
        both request and response params public in impl
      }

      class RpcServerBroadcast : IDeliverable
      {
        broadcast params public, very basic impl
      }

      class RpcClientActionResponse : IMessage??????????????? methinks not, since the Request is the recycling 'parent'
      {
        both request and response are read-only. doneWithMessage() just calls the request's doneWithMessage. so that means we don't inherit IMesssage because our "parent" request (which is IMessage FOR us) will take care of the recycling. if we try to do it, it will just add another layer of *unecessary* complication
      }

      this makes me want to just make them "Message" again... ignoring the RequestResponse hiding/write-protecting. not worth the coding overhead

      but i feel like i might not be able to

      who cares if my ending broadcast is deliverable? i could make the destination be the same as the recycle owner. and even add code on the rpc client to handle a deliver as a recycle. fuck the police

      jesus i'm fucking stupid

      yes it's "dangerous"
      no it isn't worth coding it out. at least not right now. maybe in the future if other people use the code generator ( i license it or who knows what i decide.. TODOoptional)

      didn't i just call myself stupid for saying they could all be IMessages?

      yep., guess i changed my mind

    */

};

#endif // IBANKSERVERPROTOCOLKNOWER_H

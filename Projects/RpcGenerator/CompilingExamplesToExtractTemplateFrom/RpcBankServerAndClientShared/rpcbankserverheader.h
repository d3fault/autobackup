#ifndef RPCBANKSERVERHEADER_H
#define RPCBANKSERVERHEADER_H

#include <QDataStream>

//this is my "protocol"
//i get the size of the message and the type, then i use my dispenser for that type and get the type. i then pass off delegating the rest of the message (i am only the header) to the type itself via it's datastream operator overloads
//^^^^OLD

//TODOoptimization: it's plausible that we don't have to read the message first into a QByteArray (after making sure we have enough bytes) if we just eat/ignore the QByteArray size parameter that QDataStream puts on there... unsure tbh

//TODOreq: is streaming out over the network using two successive << operations guaranteed that they are both received by the receiver in that order? Is it possible for this to happen:
/*
    same Client reading in all the shit (via >>) whenever it receives it

    Server1 streams out (<<) MessageHeader;
    Server2 streams out (<<) MessageHeader || MessageBody; //any message. the point is, can it get in between em like that?
    Server1 streams out (<<) MessageBody;

    I think the 'socket' nature of networking means that won't happen. One socket/pointer thingo represents one 'server' (peer, w/e)

    SO I _THINK_ THE ANSWER IS NO, IT IS NOT DANGEROUS
*/

//TODOoptimization: might be able to shorten something like RpcServiceId by a single bit using C bit fields and then to use it for some other bool... like success or retryBit or toggleBit or responseRetryBit... you get the idea. bit fields I can get maximum efficiency (i don't need quint16 for RpcServiceId, but quint8 isn't enough! for example)... but I'm not ready for that optimization.

struct RpcBankServerMessageHeader
{
    enum GenericRpcClient2ServerMessageType
    {
        InvalidGenericClient2ServerMessageType = 0x0, //TODOreq: go through all the code in the project looking for all the invalid/default enum types and make sure they are all properly handled. another thing to do would be to do "Find Usages" on every enum value in the project and see if any are completely unused (and therefore can be removed) <- that second part has nothing to do with the invalid/defaults (i think the compiler might notify/warn us anyways but idfk about enums)
        ActionRequestGenericClient2ServerMessageType,
        ActionRequestRetry1Status1GenericClient2ServerMessageType,
        ActionRequestRetry2Status2GenericClient2ServerMessageType, //psbly not needed, could just re-use "1"...but idfk
        BroadcastManualAckGenericClient2ServerMessageType, //wait, what? Does the ack need acking? I forget my design all of the sudden FML. WHAT IF THE MANUAL ACK NEVER MAKES IT? Do I need a ManualAckRetry1/2/etc lol wut????? I might be overthinking this but it's better I overthink it than underthink it. Hopefully at some point I see exactly where I'm overthinking it and then I can sieze the perfection. Is over over under my demise or my greatest asset?
        DisconnectRequestedGenericClient2ServerMessageType
    };
    enum GenericRpcServer2ClientMessageType
    {
        InvalidGenericServer2ClientMessageType = 0x0,
        ActionResponseGenericServer2ClientMessageType,
        ActionResponseHereItIsAgainGenericServer2ClientMessageType, //TODOreq: and what if they don't get THAT one? Is an AgainAgain necessary? mindfuck
        ActionResponseToRequestRetry1StillInBusiness1GenericServer2ClientMessageType,
        ActionResponseToRequestRetry2StillInBusiness2GenericServer2ClientMessageType, //also maybe not needed idfk
        BroadcastGenericServer2ClientMessageType,
        BroadcastRetry1GenericServer2ClientMessageType,
        BroadcastRetry2GenericServer2ClientMessageType, //also maybe not needed no idea at all because i'm putting the broadcasts and manual acking off to begin with (essentiall "one way messages", where actions are "two")
        ReadyForDisconnectGenericServer2ClientMessageType //TODOreq: what about ack'ing THIS (and also the DisconnectRequested above???) --- omfg wat --- TODOreq: make sure every enum type is ack'd in some way or another????? My brain hurts just thinking about this
    };

    //TODOreq: The header should be generic and re-used for all Rpc Services, but the enum needs to be defined at a rpc service specific level. I think this class entirely might be deleted/rewritten so we'll see. Since the class is auto-generated and also will be instantiated in memory for each unique rpc service, it doesn't matter whether I separate the generic'ness of it into a different file for re-use or not. Sure I "should", but it really doesn't matter in this case
    enum RpcBankServerSpecificMessageTypeEnum
    {
        InvalidSpecificMessageType = 0x0,
        CreateBankAccountSpecificMessageType,
        GetAddFundsKeySpecificMessageType,
        PendingBalanceDetectedSpecificMessageType,
        ConfirmedBalanceDetectedSpecificMessageType
    };
    //TODOreq: split the above enum into client2server and server2client. ez but meh i don't feel like prioritizing that JUST YET

    //TODOreq: confusingly, there are two Rpc Message "types". One specifies whether it's an ActionRequest/ActionRequestRetry1Status1/2/DisconnectRequested/etc, the other specifies stuff like "CreateBankAccountMessageType" etc
    //^^ also related but semi-OT, they can and possibly should be split between Client2Server and Server2Client "types" (FOR BOTH CATEGORIES).... but all this really does is gives us more "room" in our protocol for adding actions/broadcasts (in the second case. in the first it does very little except improve organization/sanity (but if we do use c bit fields for compaction, it gives us more room in them))
    //^^^^Just for reference, the other side of the coin for type1 is "ActionResponse, ActionResponseRetryHereItIsAgain, ActionResponseToRequestRetrySTILL_IN_BUSINESS (whether i have 1/2 or just use the same for both is TBD), Broadcast, BroadcastRetryBecauseNoAckJUSTLIKEActionRequest1Status1etc, ReadyForDisconnectAtHelloLevelBecauseAllPendingRequestsWereReturnedAndAcked (TODOreq: manual ACK'ing (not via message id re-using + toggleBit) required for all acking after DisconnectRequested received? reason: no more messages will come so how ELSE to ack them?)"

    //TODOreq(?probably-done? read to end (hmm actually probably NOT done. just added the dashes and changed my mind again)): yes there is client2server and server2client differentiability overall, but any time MANUAL acking is used... the ack will contain an enum value from an enum that seems opposite of the direction the message (an ack in this case) is being sent... so I'll very likely get confused as shit at some point or another (and will likely have forgotten I've written this (but now that I've written it, it's slightly less likely)). Example: I'm ACK'ing a broadcast (server2client) message, but the ack itself is going from the client to the server (and i still need to specify the message type? actually i probably DON'T hahahahaha forget all of this comment (but still leaving it for reference), all that is needed for the ack is the MessageId /facepalm.jpg ----- or wait no don't I need more than the MessageId because we use message specific containers as an optimization? TODOreq: that is NOT an optimization (saves cpu time at the expense of extra protocol bytes) only whenever manual ack'ing is used. So if/when manual acking is used, don't use the per-message-type containers optimization?????? Shit will get incredibly complex for the manual acking of ActionResponses after a DisconnectRequested is received, but KISS even if it means we don't optimize that much for that case (ActionResponses manual acking after DisconnectRequestsed), because it doesn't occur very often (unlike broadcasts, which should contain the optimization (or, un-optimization lolwut) and are manually acking ALWAYS anyways so have a simpler code path)

    //quint16 MessageSize; -- definitely no longer needed because of QByteArrayPeeker <3
    //quint16 RpcServiceId; -- not sure if needed (it is a matter of choice/design)... but commenting out for now because it makes more sense to use PORTS to differentiate between rpc services (at least i think it does but could be wrong???)... AND it isn't referenced anywwhere anyways!!! BTW OT but semi-relevant: AppId (such as specifying a "bank account" in previous uses/examples/designs/prototypes/etc) is NOT related to RpcServiceId. It would be a "rpc service" specific value, sent at a different level. I have said that before but am now saying it again (whatchu gonna do 'bout it?)
    quint16 MessageId; //changing to quint16 because it would be very unlikely that more than 65536 would be received without a single one being responded to lmfao (and therefore reused). saves us 2 bytes per message! hell even a quint8 would be sufficient on low-load servers. TODOreq: this should be generated kinda like we generate teh cookie: ensuring no dupes in a do-while loop thingo. right now i have it using the current date time as a sort of salt, so it should be good enough (famous last words, hence the req)
    quint8 GenericRpcMessageType; //TODOreq: use it!
    quint16 RpcServiceSpecificMessageType; //TODOoptimization: this is probably the best value to use c bit fields in... because 256 message types (quint8) is NOT ENOUGH... and 65536 message types is way too much... so that means we have bits that we can steal from it <3! We'd still need to specify and document a maximum amount of message types (and then our input xml protocol parser should verify that the user didn't specify more than the max)



    //TODOreq: Shit I think I just contradicted completely the "lazy ack ack" design by introducing the "generic type" protocol value... but maybe not. I don't know and have never been more lost in this design as right this very fucking moment. Fantacizing of shotgun to the face etc. I guess if it's just a typical ActionRequest we check to see if it's in lazy ack and then ack it (forget about it) if it is? Otherwise we'll get a ActionRequestRetry and we still check it and in that case, re-send it (or say business pending etc). Bah. Gravity may be in effect but I've hit the titanic's propeller and am now spinning rapidly towards the ocean (kind of fun, but i want to do a pencil dive (or regular dive), not a fucking belly/back flop!)
    //^^^^Could even, if I want to KISS, do an ActionRequestThatAlsoAcksThisSameMessageId, since the client "knows" when it is! Wouldn't take any extra space and have plenty left in that quint8
    //make sure message IDs are only re-used by the same rpc specific message type (since they are stored separately as an optimization). This may or may not affect what I've just written above
};
inline QDataStream &operator>>(QDataStream &in, RpcBankServerMessageHeader &message)
{
    //in >> message.MessageSize;
    //in >> message.RpcServiceId;
    in >> message.MessageId;
    in >> message.GenericRpcMessageType;
    in >> message.RpcServiceSpecificMessageType;
    return in;
}
inline QDataStream &operator<<(QDataStream &out, const RpcBankServerMessageHeader &message)
{
    //TO DONEoptimization: we could detect a broadcast by seeing if messageId is 0 and then not streaming out the message id. but it means on the client i'd have to receive the header in 2 stages (figure out if it's a broadcast or not and then get the message id if applicable). not worth it imo... (also you'd have to change the order of them, but that's given)
    //^^^^^^^SOLUTION: we _DO_ want to stream out the message id, even for broadcasts. for broadcasts, we need it for our ACK
    //out << message.MessageSize;
    //out << message.RpcServiceId;
    out << message.MessageId;
    out << message.GenericRpcMessageType;
    out << message.RpcServiceSpecificMessageType;
    return out;
}
#endif // RPCBANKSERVERHEADER_H

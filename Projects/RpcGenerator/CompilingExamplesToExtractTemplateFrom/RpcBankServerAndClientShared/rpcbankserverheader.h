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

struct RpcBankServerMessageHeader
{
    enum MessageTypeEnum
    {
        InvalidMessageType = 0x0,
        CreateBankAccountMessageType,
        GetAddFundsKeyMessageType,
        PendingBalanceDetectedMessageType,
        ConfirmedBalanceDetectedMessageType
    };
    quint16 MessageSize;
    quint16 RpcServiceId;
    quint32 MessageId; //had this as QByteArray, but decided to change it [back? right? backspaced and forgot what it was lol] to quint32.. because isn't MD5 exactly 32 bits? perfect fit and saves me from the QByteArray size parameter :)
    quint16 MessageType;
};
inline QDataStream &operator>>(QDataStream &in, RpcBankServerMessageHeader &message)
{
    in >> message.MessageSize;
    in >> message.RpcServiceId;
    in >> message.MessageId;
    in >> message.MessageType;
    return in;
}
inline QDataStream &operator<<(QDataStream &out, const RpcBankServerMessageHeader &message)
{
    //TO DONEoptimization: we could detect a broadcast by seeing if messageId is 0 and then not streaming out the message id. but it means on the client i'd have to receive the header in 2 stages (figure out if it's a broadcast or not and then get the message id if applicable). not worth it imo... (also you'd have to change the order of them, but that's given)
    //^^^^^^^SOLUTION: we _DO_ want to stream out the message id, even for broadcasts. for broadcasts, we need it for our ACK
    out << message.MessageSize;
    out << message.RpcServiceId;
    out << message.MessageId;
    out << message.MessageType;
    return out;
}
#endif // RPCBANKSERVERHEADER_H

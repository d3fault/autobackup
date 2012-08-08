#ifndef RPCBANKSERVERHEADER_H
#define RPCBANKSERVERHEADER_H

#include <QDataStream>

//this is my "protocol"
//i get the size of the message and the type, then i use my dispenser for that type and get the type. i then pass off delegating the rest of the message (i am only the header) to the type itself via it's datastream operator overloads
//^^^^OLD
//now i just have the type and the message id. the size was unattainable unless i first stream into a byte array and count it.. so fuck that.
struct RpcBankServerHeader
{
    enum MessageTypeEnum
    {
        InvalidMessageType = 0,
        CreateBankAccountMessageType,
        GetAddFundsKeyMessageType,
        PendingBalanceDetectedMessageType,
        ConfirmedBalanceDetectedMessageType
    };
    quint32 MessageId;
    quint32 MessageType;
    bool Success; //bah we don't need this for the action request. we could do some #ifdef magic to make it not be streamed just like i used to for the messages themselves.. since i don't want to read it when i'm on the server reading IN, but i do want to read it when i'm on the client reading IN (etc). i think i have been over/prematurely optimizing. FML. god fucking damnit. it's a bad habit and i need to kill it with fire. seriously a really limiting habit. you won't go nearly as far in life if you don't get rid of it. at the same time, the world has enough Java implements (enough idiots that don't optimize)
};
inline QDataStream &operator>>(QDataStream &in, RpcBankServerHeader &message)
{
    in >> message.MessageId;
    in >> message.MessageType;
    in >> message.Success; //TODOoptimization: don't stream in Success on server, do stream in Success on client, don't stream out Success on client, do stream out Success on server -- similar to how i used to do IMessage stream shit with ifdefs
    return in;
}
inline QDataStream &operator<<(QDataStream &out, const RpcBankServerHeader &message)
{
    //TODOoptimization: we could detect a broadcast by seeing if messageId is 0 and then not streaming out the message id. but it means on the client i'd have to receive the header in 2 stages (figure out if it's a broadcast or not and then get the message id if applicable). not worth it imo... (also you'd have to change the order of them, but that's given)
    out << message.MessageId;
    out << message.MessageType;
    out << message.Success;
    return out;
}

#endif // RPCBANKSERVERHEADER_H

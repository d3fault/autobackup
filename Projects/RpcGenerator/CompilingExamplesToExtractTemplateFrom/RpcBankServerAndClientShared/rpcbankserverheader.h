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
    uint MessageId;
    uint MessageType;
};
inline QDataStream &operator>>(QDataStream &in, RpcBankServerHeader &message)
{
    in >> message.MessageId;
    in >> message.MessageType;
    return in;
}
inline QDataStream &operator<<(QDataStream &out, const RpcBankServerHeader &message)
{
    out << message.MessageId;
    out << message.MessageType;
    return out;
}

#endif // RPCBANKSERVERHEADER_H

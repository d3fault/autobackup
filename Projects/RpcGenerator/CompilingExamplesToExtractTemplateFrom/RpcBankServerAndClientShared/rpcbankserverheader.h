#ifndef RPCBANKSERVERHEADER_H
#define RPCBANKSERVERHEADER_H

//this is my "protocol"
//i get the size of the message and the type, then i use my dispenser for that type and get the type. i then pass off delegating the rest of the message (i am only the header) to the type itself via it's datastream operator overloads
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
    uint MessageSize;
    uint MessageType;
};
inline QDataStream &operator>>(QDataStream &in, RpcBankServerHeader &message)
{
    in >> message.MessageSize;
    in >> message.MessageType;
    return in;
}
inline QDataStream &operator<<(QDataStream &out, const RpcBankServerHeader &message)
{
    out << message.MessageSize;
    out << message.MessageType;
    return out;
}

#endif // RPCBANKSERVERHEADER_H

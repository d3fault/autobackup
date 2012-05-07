#ifndef APPCLIENTBANKSERVERPROTOCOL_H
#define APPCLIENTBANKSERVERPROTOCOL_H

#include <QDataStream>

struct AppClientBankServerMessage //my thoughts had me wondering if i should just make the RequestResponse and Broadcast messages as different types, with their own unique operator overloads for the streaming. the reason i DO NOT want to do this is because when i'm reading the message off the network, i don't know what type it is. i only know that it is an AppClientBankServerMessage... and will deduce later (during the operator overload) which sub-type it is
{
    enum AppClientBankServerMessageType
    {
        InvalidAppClientBankServerMessageType,
        AppClientBankServerMessageRequestResponseType,
        AppClientBankServerMessageBroadcastType
    };
    AppClientBankServerMessageType m_MessageType;
    QString m_UserName;

    //only for AppClientBankServerMessageRequestResponse
    bool m_IsResponse;
    bool m_ErrorOccurred;
    QString m_ErrorString; //only in message if(error-occured). lol premature ejaculation BUT THIS IS SO FUN (so is cumming). oh and also this is basically what protobuf does for you~
};

struct AppClientBankServerMessageRequestResponse : public AppClientBankServerMessage
{
    enum Action //Actions have both requests and responses
    {
        AddFundsKeyAction,
        BalanceTransferAction,
        DisburseAction
    };
};

struct AppClientBankServerMessageBroadcast : public AppClientBankServerMessage
{
    enum BroadcastMessage
    {
        PendingAmountDetected,
        ConfirmedAmountDetected
    };
};

//so long as i keep these together and basically the exact same (except for the direction of the stream operator), i can do custom/efficient/minimal messaging. the in-memory message will not be efficient/minimal... only the network traffic :(. oh well network traffic is actually one of the most expensive things~
inline QDataStream &operator>>(QDataStream &in, AppClientBankServerMessage &message)
{
    in >> message.m_MessageType;
    if(message.m_MessageType == AppClientBankServerMessage::AppClientBankServerMessageRequestResponseType)
    {
        in >> message.m_IsResponse;
        in >> message.m_ErrorOccurred;
        if(message.m_ErrorOccurred)
        {
            in >> message.m_ErrorString;
        }
    }
    .........username.............

    return in;
}
inline QDataStream &operator<<(QDataStream &out, AppClientBankServerMessage &message)
{
    out << message.m_MessageType;
    if(message.m_MessageType == AppClientBankServerMessage::AppClientBankServerMessageRequestResponseType)
    {
        out << message.m_IsResponse;
        out << message.m_ErrorOccurred;
        if(message.m_ErrorOccurred)
        {
            out << message.m_ErrorString;
        }
    }

    return out;
}

#endif // APPCLIENTBANKSERVERPROTOCOL_H

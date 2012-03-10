#ifndef SHAREDPROTOCOL_H
#define SHAREDPROTOCOL_H

#include <QDataStream>

struct Message
{
    enum MessageType
    {
        InvalidMessageType,
        ClientToServerMessageType,
        ServerToClientMessageType
    };

    inline Message(MessageType messageType = InvalidMessageType, QString appId = QString(), quint16 theMessage = -1, QString extraString = QString(), double extraDouble = -1.0, QString extraString2 = QString())
        : m_MessageType(messageType), m_AppId(appId), m_TheMessage(theMessage), m_ExtraString(extraString), m_ExtraDouble(extraDouble), m_ExtraString2(extraString2)
    { }

    quint16 m_MessageType;
    QString m_AppId;
    quint16 m_TheMessage;
    QString m_ExtraString; //generally used for storing a btc address, but can be empty for certain messages. holds app id and username too
    double m_ExtraDouble; //generally used for storing a btc value, but can be empty for certain messages
    //todo: verify that the db stores/retrieves the double properly, that no rounding errors occur throughout the client/server or anywhere. think of test cases. i THINK that since i'm using C++ (same as bitcoin itself) i should be ok
    QString m_ExtraString2;
};

struct ClientToServerMessage : public Message
{
    enum TheMessage
    {
        InvalidClientToServerMessage,
        AddUserWithThisName,
        GiveMeAKeyForUserXAndWatchForPayment,
        BalanceTransferIfEnough //purchase a slot, for example. we're gonna need another QString() to account for the fact that there's a from and a to user (and of course, the amount)
    };

    inline ClientToServerMessage(QString appId = QString(), TheMessage theMessage = InvalidClientToServerMessage, QString extraString = QString(), double extraDouble = -1.0, QString extraString2 = QString())
        : Message(Message::ClientToServerMessageType, appId, theMessage, extraString, extraDouble, extraString2)
    { }
};

struct ServerToClientMessage : public Message
{
    enum TheMessage
    {
        InvalidServerToClientMessage,
        InvalidAppId,
        UserAdded, //i was going to return the bank account id, but there's no point of it ever leaving the server. appId + username is enough. the local cache will just have a "isAccountSetup" bool value instead of seeing if bank_account_id is not equal to zero like i originally planned...
        HeresAKeyXForUserYAndIllLetYouKnowWhenPaymentIsReceived,
        HeyThisKeyXForUserYGotSomePendingMoneyZ,
        HeyThisKeyXForUserYGotSomeConfirmedMoneyZ
    };

    inline ServerToClientMessage(QString appId = QString(), TheMessage theMessage = InvalidServerToClientMessage, QString extraString = QString(), double extraDouble = -1.0, QString extraString2 = QString())
        : Message(Message::ServerToClientMessageType, appId, theMessage, extraString, extraDouble, extraString2)
    { }
};

inline QDataStream &operator>>(QDataStream &in, Message &message)
{
    in >> message.m_MessageType;
    in >> message.m_AppId;
    in >> message.m_TheMessage;
    in >> message.m_ExtraString;
    in >> message.m_ExtraDouble;
    in >> message.m_ExtraString2;

    return in;
}
inline QDataStream &operator<<(QDataStream &out, Message &message)
{
    out << message.m_MessageType;
    out << message.m_AppId;
    out << message.m_TheMessage;
    out << message.m_ExtraString;
    out << message.m_ExtraDouble;
    out << message.m_ExtraString2;

    return out;
}

#endif // SHAREDPROTOCOL_H

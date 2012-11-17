#ifndef HELLOPROTOCOL_H
#define HELLOPROTOCOL_H

#include <QObject>

#if 0
struct HelloMessage
{
    enum HelloMessageType
    {
        InvalidHelloMessageType,
        HelloMessageFromClientType,
        HelloMessageFromServerType
    };
    HelloMessage(HelloMessageType helloMessageType = InvalidHelloMessageType, quint16 helloMessageSubType = 0, )

    quint16 m_HelloMessageType;
};

struct Message
{
    enum MessageType
    {
        InvalidMessageType,
        MessageFromClientType, //to server. aka "request"
        MessageToClientType //to client. aka "reply/response"
    };

    inline Message(MessageType type = InvalidMessageType, quint16 subType = 0, quint32 messageIntValue = 0, QString messageStringValue = QString())
        : m_MessageType(type), m_MessageSubType(subType), m_MessageIntValue(messageIntValue), m_MessageStringValue(messageStringValue)
    { }

    quint16 m_MessageType;
    quint16 m_MessageSubType;
    quint32 m_MessageIntValue;
    QString m_MessageStringValue;
};

//request
struct MessageFromClient : public Message
{
    enum MessageFromClientSubType
    {
        InvalidMessageFromClientSubType, //should i make this equal to -1 like in the Message constructor above?
        HelloMessageFromClientSubType,
        MayIPleaseHaveABeer,
        ThanksForTheBeer,
        GoodbyeMessageFromClientSubType
    };

    inline MessageFromClient(MessageFromClientSubType messageFromClientSubType = InvalidMessageFromClientSubType, quint32 messageIntValue = 0, QString messageStringValue = QString())
        : Message(MessageFromClientType, messageFromClientSubType, messageIntValue, messageStringValue)
    { }
};

//reply/request
struct MessageToClient : public Message
{
    enum MessageToClientSubType
    {
        InvalidMessageToClientSubType, //-1?
        WelcomeMessageToClientSubType,
        YesHereIsYourBeer,
        OkTakeCareMessageToClientSubType
    };

    inline MessageToClient(MessageToClientSubType messageToClientSubType = InvalidMessageToClientSubType, quint32 messageIntValue = 0, QString messageStringValue = QString())
        : Message(MessageToClientType, messageToClientSubType, messageIntValue, messageStringValue)
    { }
};

inline QDataStream &operator>>(QDataStream &in, Message &message)
{
    in >> message.m_MessageType;
    in >> message.m_MessageSubType;
    in >> message.m_MessageIntValue;
    in >> message.m_MessageStringValue;

    return in;
}

inline QDataStream &operator<<(QDataStream &out, Message &message)
{
    out << message.m_MessageType;
    out << message.m_MessageSubType;
    out << message.m_MessageIntValue;
    //err i think the message length is not necessary as QDataStream puts it in their for us.. which is why i don't need to specify it here. fuck it, it'll just be an ARBITRARY INTEGER VALUE then. quick find/replace and already fixed this.
    out << message.m_MessageStringValue;

    return out;
}
#endif
#endif // HELLOPROTOCOL_H

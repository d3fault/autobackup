#ifndef MYPROTOCOL_H
#define MYPROTOCOL_H

#include <QString>

//in any case, learning protocols + tcp/ssl coding is a VERY useful skill. who cares what project i use it in. i will probably use the knowledge/code learned/typed tonight for the rest of my life.

//base
struct Message
{
    enum MessageType
    {
        InvalidMessageType,
        MessageFromClientType, //to server. aka "request"
        MessageToClientType //to client. aka "reply/response"
    };

    inline Message(MessageType type = InvalidMessageType, quint16 subType = -1, quint32 messageIntValue = 0, QString messageStringValue = QString())
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

#endif //MYPROTOCOL_H

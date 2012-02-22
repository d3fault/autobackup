#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QDataStream>

struct Message
{
    enum MessageType
    {
        InvalidMessageType,
        ClientToServerMessageType,
        ServerToClientMessageType
    };

    inline Message(MessageType messageType = InvalidMessageType, quint16 theMessage = -1)
        : m_MessageType(messageType), m_TheMessage(theMessage)
    { }

    quint16 m_MessageType;
    quint16 m_TheMessage;

};

struct ClientToServerMessage : public Message
{
    enum TheMessage
    {
        InvalidClientToServerMessage,
        HelloSuckMyDick,
        GoodbyeGoFuckYourself
    };

    inline ClientToServerMessage(TheMessage theMessage = InvalidClientToServerMessage)
        : Message(Message::ClientToServerMessageType, theMessage)
    { }
};

struct ServerToClientMessage : public Message
{
    enum TheMessage
    {
        InvalidServerToClientMessage,
        WelcomeAndNoThanks,
        ThatsRudeByeNow
    };

    inline ServerToClientMessage(TheMessage theMessage = InvalidServerToClientMessage)
        : Message(Message::ServerToClientMessageType, theMessage)
    { }
};

inline QDataStream &operator>>(QDataStream &in, Message &message)
{
    in >> message.m_MessageType;
    in >> message.m_TheMessage;

    return in;
}
inline QDataStream &operator<<(QDataStream &out, Message &message)
{
    out << message.m_MessageType;
    out << message.m_TheMessage;

    return out;
}

#endif // PROTOCOL_H

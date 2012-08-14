#ifndef SIMPLEASSPROTOCOL_H
#define SIMPLEASSPROTOCOL_H

#include <QDataStream>

struct MyMessageHeader
{
    enum MessageTypeEnum
    {
        MessageAType = 0,
        MessageBType
    };

    //quint32 MessageMagic;
    quint32 MessageType;
    quint32 MessageSize;
};
//quint32 MyMessageHeader::MessageMagic = (quint32)0xA0B0C0D0;

inline QDataStream &operator>>(QDataStream &in, MyMessageHeader &header)
{
    //in >> header.MessageMagic;
    in >> header.MessageType;
    in >> header.MessageSize;
    return in;
}
inline QDataStream &operator<<(QDataStream &out, MyMessageHeader &header)
{
    //out << header.MessageMagic;
    out << header.MessageType;
    out << header.MessageSize;
    return out;
}

struct MyMessageABody
{
    qint32 RandomInt1;
    quint32 RandomUInt2;
    QString RandomString1;
    QString RandomString2;
};
//QString MyMessageABody::dbgName = "A Message";
inline QDataStream &operator>>(QDataStream &in, MyMessageABody &body)
{
    in >> body.RandomInt1;
    in >> body.RandomUInt2;
    in >> body.RandomString1;
    in >> body.RandomString2;
    return in;
}
inline QDataStream &operator<<(QDataStream &out, MyMessageABody &body)
{
    out << body.RandomInt1;
    out << body.RandomUInt2;
    out << body.RandomString1;
    out << body.RandomString2;
    return out;
}

struct MyMessageBBody
{
    qint32 RandomInt1;
    quint32 RandomUInt2;
    QString RandomString1;
    QString RandomString2;
};
//QString MyMessageBBody::dbgName = "B Message";
inline QDataStream &operator>>(QDataStream &in, MyMessageBBody &body)
{
    in >> body.RandomInt1;
    in >> body.RandomUInt2;
    in >> body.RandomString1;
    in >> body.RandomString2;
    return in;
}
inline QDataStream &operator<<(QDataStream &out, MyMessageBBody &body)
{
    out << body.RandomInt1;
    out << body.RandomUInt2;
    out << body.RandomString1;
    out << body.RandomString2;
    return out;
}

#endif //SIMPLEASSPROTOCOL_H

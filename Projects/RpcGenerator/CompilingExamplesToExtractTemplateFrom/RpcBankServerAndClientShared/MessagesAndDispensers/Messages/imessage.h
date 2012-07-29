#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <QObject>
#include <QDataStream>

#include "../../rpcbankserverheader.h"

class IMessage : public QObject
{
    Q_OBJECT
public:
    explicit IMessage(QObject *owner);
    void doneWithMessage();
    void deliver();
    virtual void streamIn(QDataStream &in)=0;
    virtual void streamOut(QDataStream &out)=0;
    RpcBankServerHeader Header; //it's a member, but we don't want to stream it because we have to stream it into a stack alloc'd header first and then retrieve the stored message from pending (it's stored by the header contents. chicken and egg problem)
signals:
    void doneWithMessageSignal();
    void deliverSignal();
};
inline QDataStream &operator>>(QDataStream &in, IMessage &message)
{
    message.streamIn(in);
    return in;
}
inline QDataStream &operator<<(QDataStream &out, IMessage &message)
{
    message.streamOut(out);
    return out;
}

#endif // IMESSAGE_H

#ifndef IRECYCLEABLEANDSTREAMABLE_H
#define IRECYCLEABLEANDSTREAMABLE_H

#include <QObject>

#include "../../rpcbankserverheader.h"

class IRecycleableAndStreamable : public QObject
{
    Q_OBJECT
public:
    explicit IRecycleableAndStreamable(QObject *parent);
    virtual void streamIn(QDataStream &in)=0;
    virtual void streamOut(QDataStream &out)=0;
    virtual void resetMessageParameters()=0;
    RpcBankServerMessageHeader Header; //it's a member, but we don't want to stream it because we have to stream it into a stack alloc'd header first and then retrieve the stored message from pending (it's stored by the header contents. chicken and egg problem)
    inline void doneWithMessage() { emit doneWithMessageSignal(); } //how can i inline this if inline'ing means the code is copied to the caller... and the signal it emits (the code itself) is protected??? interested in watching the compiler break :-P. But I also wouldn't be surprised if it just works. It's mainly just data encapsulation the compiler doesn't give a fuck
signals:
    void doneWithMessageSignal();
};
inline QDataStream &operator>>(QDataStream &in, IRecycleableAndStreamable &message)
{
    message.streamIn(in);
    return in;
}
inline QDataStream &operator<<(QDataStream &out, IRecycleableAndStreamable &message)
{
    message.streamOut(out);
    return out;
}

#endif // IRECYCLEABLEANDSTREAMABLE_H

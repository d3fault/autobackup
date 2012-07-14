#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <QObject>
#include <QDataStream>

class IMessage : public QObject
{
    Q_OBJECT
public:
    explicit IMessage(QObject *mandatoryParent);
    void doneWithMessage();
    void deliver();
    virtual void streamIn(QDataStream &in)=0;
    virtual void streamOut(QDataStream &out)=0;
signals:
    void doneWithMessageSignal();
    void deliverSignal();
public slots:
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

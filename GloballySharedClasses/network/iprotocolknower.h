#ifndef IPROTOCOLKNOWER_H
#define IPROTOCOLKNOWER_H

#include <QDataStream>

class IProtocolKnower// : public QObject
{
    //Q_OBJECT
public:
    //MEH NVM: parent is required because we're going to be using this as a destination target in QObject::connect calls and this makes it so we can't leave out the parent and set it up wrong. I trust threading most with proper thread affinity and proper parent'ing (to tell the truth, the difference seems blurry in the docs)
    explicit void IProtocolKnower() { } //QObject *parent) : QObject(parent) { }
protected:
    virtual void messageReceived(QDataStream *messageDataStream /*, quint32 clientId */)=0;
};

#endif // IPROTOCOLKNOWER_H

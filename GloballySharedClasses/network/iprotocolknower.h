#ifndef IPROTOCOLKNOWER_H
#define IPROTOCOLKNOWER_H

#include <QDataStream>
#include <QByteArray>
#include <QBuffer>

#include "abstractclientconnection.h"

class IProtocolKnower// : public QObject
{
    //Q_OBJECT
public:
    //MEH NVM: parent is required because we're going to be using this as a destination target in QObject::connect calls and this makes it so we can't leave out the parent and set it up wrong. I trust threading most with proper thread affinity and proper parent'ing (to tell the truth, the difference seems blurry in the docs)
    explicit void IProtocolKnower()
    {
        m_TransmitMessageBuffer.setBuffer(&m_TransmitMessageByteArray);
        m_TransmitMessageBuffer.open(QIODevice::WriteOnly);
        m_TransmitMessageDataStream.setDevice(&m_TransmitMessageBuffer);
    }
    inline void resetTransmitMessage()
    {
        m_TransmitMessageBuffer.seek(0);
        m_TransmitMessageByteArray.clear(); //TODOreq: is this necessary or does "streaming into" it overwrite previous contents?
    }
    inline void setAbstractClientConnection(AbstractClientConnection *abstractClientConnection) { m_AbstractClientConnection = abstractClientConnection; }
private:
    AbstractClientConnection *m_AbstractClientConnection;

    QByteArray m_TransmitMessageByteArray;
    QBuffer m_TransmitMessageBuffer;
    QDataStream m_TransmitMessageDataStream;
protected:
    virtual void messageReceived(QDataStream *messageDataStream /*, quint32 clientId */)=0;
};

#endif // IPROTOCOLKNOWER_H

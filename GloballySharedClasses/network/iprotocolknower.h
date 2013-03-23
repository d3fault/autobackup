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
    explicit IProtocolKnower()
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
    inline void setMessageReceivedDataStream(QDataStream *messageReceivedDataStream) { m_MessageReceivedDataStream = messageReceivedDataStream; }
    virtual void messageReceived()=0; //QDataStream *messageDataStream /*, quint32 clientId */);
    virtual void notifyThatQueueActionResponsesHasBeenEnabled()=0;
private:
    QBuffer m_TransmitMessageBuffer;
protected:
    AbstractClientConnection *m_AbstractClientConnection;
    QDataStream *m_MessageReceivedDataStream;
    QByteArray m_TransmitMessageByteArray;
    QDataStream m_TransmitMessageDataStream;
};

#endif // IPROTOCOLKNOWER_H

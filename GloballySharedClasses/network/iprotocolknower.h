#ifndef IPROTOCOLKNOWER_H
#define IPROTOCOLKNOWER_H

#include <QDataStream>
#include <QByteArray>
#include <QBuffer>

#include "abstractconnection.h"

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
        m_TransmitMessageByteArray.clear(); //TODOoptimization: is this necessary or does "streaming into" it overwrite previous contents? read vs. write, bitch! this is write so it would append to the datastream... so it seems very necessary to clear it in this use
    }
    inline void streamDoneHelloingFromServerIntoMessageAboutToBeTransmittedToClient()
    {
        m_TransmitMessageDataStream << ((quint8)AbstractConnection::DoneHelloingFromServer); //semi-hacky but sure beats building yet another QDS inside AbstractClientConnection just to stream it lmfao
    }
    inline void setAbstractConnection(AbstractConnection *abstractConnection) { m_AbstractConnection = abstractConnection; }
    inline void setMessageReceivedDataStream(QDataStream *messageReceivedDataStream) { m_MessageReceivedDataStream = messageReceivedDataStream; }
    virtual void messageReceived()=0; //QDataStream *messageDataStream /*, quint32 clientId */);
    virtual void queueActionResponsesHasBeenEnabledSoBuildLists()=0; //TODOreq: isn't this server only? Should this file be shared or copy/pasted or even abstracted further? Hell even #ifdef's work and I might need to use them for the streamDoneHelloing shit above. Who cares how hacky this shit is I just NEEEEEEEEEEEEEEEEEEEEEEEED to get it done (was tempted to put /want after that 'need', but eh it's starting to look like a need. Seriously there's nothing else I can do with my life except progress forward (feels good man))
private:
    QBuffer m_TransmitMessageBuffer;
protected:
    AbstractConnection *m_AbstractConnection;
    QDataStream *m_MessageReceivedDataStream;
    QByteArray m_TransmitMessageByteArray;
    QDataStream m_TransmitMessageDataStream;
};

#endif // IPROTOCOLKNOWER_H

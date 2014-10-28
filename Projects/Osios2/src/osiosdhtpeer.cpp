#include "osiosdhtpeer.h"

#include <QtNetwork/QAbstractSocket>
#include <QBuffer>

#include "timelineserializer.h"

OsiosDhtPeer::OsiosDhtPeer(QAbstractSocket *socketToPeer, QObject *parent)
    : QObject(parent)
    , m_SocketToPeer(socketToPeer)
    , m_StreamToPeer(socketToPeer)
{
    connect(socketToPeer, SIGNAL(connected()), this, SLOT(handleSocketToPeerConnected()));
    connect(socketToPeer, SIGNAL(readyRead()), this, SLOT(handleSocketToPeerReadyRead()));
}
void OsiosDhtPeer::sendNewTimelineNodeForFirstStepOfCryptographicVerification(TimelineNode action)
{
    QByteArray actionPayload = action->toByteArray(); //we're peeking into the qbytearray's size when we read
    //m_StreamToPeer << actionPayload;
    sendOsiosMessageToDhtPeer(OsiosDhtMessageTypes::SengingNewTimelineNodeForFirstStepOfCryptographicVerification, actionPayload);
}
void OsiosDhtPeer::respondWithCryptographicHashComputedFromReceivedTimelineNode(QByteArray cryptographicHashOfPreviouslyReceivedTimelineNode)
{
    sendOsiosMessageToDhtPeer(OsiosDhtMessageTypes::RespondingWithCryptographicHashComputedFromReceivedTimelineNode, cryptographicHashOfPreviouslyReceivedTimelineNode);
}
void OsiosDhtPeer::sendOsiosMessageToDhtPeer(OsiosDhtMessageTypes::OsiosDhtMessageTypesEnum osiosDhtMessageType, QByteArray payload)
{
    m_StreamToPeer << static_cast<SerializedDhtMessageType>(osiosDhtMessageType);
    m_StreamToPeer << payload;
}
void OsiosDhtPeer::handleSocketToPeerConnected()
{
    emit osiosDhtPeerConnected(this);
}
void OsiosDhtPeer::handleSocketToPeerReadyRead()
{
    //we use the same protocol as the `gui -> backend` protocol (which uses qlocalserver/qlocalsocket (wait nvm just signals/slots, but i could do qlocalserver/qlocalsocket and a client/daemon thing like in attemp1 of this same app)), but additionally we stream the dht message type and the total message size up front so we can check there's enough bytes to read (localserver/localsocket doesn't have this problem (but you might as well implement it to be on the safe side!))

    qint64 bytesAvailable;
    while((bytesAvailable = m_SocketToPeer->bytesAvailable()) > 0)
    {
        qint64 headerSize = sizeof(SerializedDhtMessageType) /*dht message type*/ + sizeof(quint32) /*payload message size*/; //xD dat name (was 'sizeSize' before i added the dht message type to the header)
        if(bytesAvailable < headerSize)
            return;
        bytesAvailable -= headerSize;
        QByteArray headerPeekedByteArray = m_SocketToPeer->peek(headerSize);
        if(headerPeekedByteArray.size() < headerSize)
            return;
        QBuffer headerPeekedBuffer(&headerPeekedByteArray);
        headerPeekedBuffer.open(QIODevice::ReadOnly);
        QDataStream peekedMessageSizeReader(&headerPeekedBuffer);
        SerializedDhtMessageType dhtMessageTypeData;
        peekedMessageSizeReader >> dhtMessageTypeData;
        OsiosDhtMessageTypes::OsiosDhtMessageTypesEnum dhtMessageType = static_cast<OsiosDhtMessageTypes::OsiosDhtMessageTypesEnum>(dhtMessageTypeData);
        quint32 timelineNodeSize;
        peekedMessageSizeReader >> timelineNodeSize;
        if(timelineNodeSize == 0xFFFFFFFF)
        {
            //null byte array/timeline-node (wtf?), 'read it' and ignore it and continue
            m_StreamToPeer >> headerPeekedByteArray; //this just happens to be a byte array we have laying around and are done with, so the name doesn't match what we're doing
            continue;
        }
        if(static_cast<qint64>(timelineNodeSize) < bytesAvailable)
            return;

        //getting here means we know there are enough bytes available to read the entire message
        m_StreamToPeer >> dhtMessageTypeData; //derp, get dat peeked shit out of the way
        //m_StreamToPeer >> timelineNodeSize; //derp, get dat peeked shit out of the way. nvm anymore since the message size is 'built into' the qbytearray payload (h4x)

        QByteArray payload;
        m_StreamToPeer >> payload;

        if(dhtMessageType == OsiosDhtMessageTypes::SengingNewTimelineNodeForFirstStepOfCryptographicVerification)
        {
            TimelineNode theTimelineNodeReceivedFromPeer = ITimelineNode::fromByteArray(payload);
            emit timelineNodeReceivedFromPeer(this, theTimelineNodeReceivedFromPeer);
            continue;
        }
        if(dhtMessageType == OsiosDhtMessageTypes::RespondingWithCryptographicHashComputedFromReceivedTimelineNode)
        {
            emit responseCryptoGraphicHashReceivedFromNeighbor(this, payload);
            continue;
        }
    }
}
#ifndef OSIOSDHTPEER_H
#define OSIOSDHTPEER_H

#include <QObject>
#include <QDataStream>

#include "itimelinenode.h"
#include "osioscommon.h"

class QAbstractSocket;

class OsiosDhtMessageTypes
{
public:
    enum OsiosDhtMessageTypesEnum
    {
          SengingNewTimelineNodeForFirstStepOfCryptographicVerification = 0
        , RespondingWithCryptographicHashComputedFromReceivedTimelineNode = 1
    };
};

typedef quint8 SerializedDhtMessageType;

class OsiosDhtPeer : public QObject
{
    Q_OBJECT
public:
    explicit OsiosDhtPeer(QAbstractSocket *socketToPeer, DhtPeerAddressAndPort peerConnectionInfo, QObject *parent = 0);
    void sendNewTimelineNodeForFirstStepOfCryptographicVerification(TimelineNode action);
    void respondWithCryptographicHashComputedFromReceivedTimelineNode(QByteArray cryptographicHashOfPreviouslyReceivedTimelineNode);
private:
    QAbstractSocket *m_SocketToPeer;
    QDataStream m_StreamToPeer;
    bool m_QTcpSocketHasToldUsWeAreConnected; //cryptographic verification is MUCH more reliable, so I'm NOT going to call this m_Connected as I was tempted to
    DhtPeerAddressAndPort m_PeerConnectionInfo;

    void sendOsiosMessageToDhtPeer(OsiosDhtMessageTypes::OsiosDhtMessageTypesEnum osiosDhtMessageType, QByteArray payload);
signals:
    void osiosDhtPeerConnected(OsiosDhtPeer *newOsiosDhtPeer);
    void timelineNodeReceivedFromPeer(OsiosDhtPeer *osiosDhtPeer, TimelineNode actionReceivedFromPeer);
    void responseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer *osiosDhtPeer, const QByteArray &responseCryptographicHash);

    void connectionNoLongerConsideredGood(OsiosDhtPeer *osiosDhtPeer);
private slots:
    void handleSocketToPeerConnected();
    void handleSocketToPeerReadyRead();
    void emitConnectionNoLongerConsideredGood();
};

#endif // OSIOSDHTPEER_H

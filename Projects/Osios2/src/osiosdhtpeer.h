#ifndef OSIOSDHTPEER_H
#define OSIOSDHTPEER_H

#include <QObject>
#include <QDataStream>

#include "itimelinenode.h"

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
    explicit OsiosDhtPeer(QAbstractSocket *socketToPeer, QObject *parent = 0);
    void sendNewTimelineNodeForFirstStepOfCryptographicVerification(TimelineNode action);
    void respondWithCryptographicHashComputedFromReceivedTimelineNode(QByteArray cryptographicHashOfPreviouslyReceivedTimelineNode);
private:
    QAbstractSocket *m_SocketToPeer;
    QDataStream m_StreamToPeer;

    void sendOsiosMessageToDhtPeer(OsiosDhtMessageTypes::OsiosDhtMessageTypesEnum osiosDhtMessageType, QByteArray payload);
signals:
    void osiosDhtPeerConnected(OsiosDhtPeer *newOsiosDhtPeer);
    void timelineNodeReceivedFromPeer(OsiosDhtPeer *osiosDhtPeer, TimelineNode actionReceivedFromPeer);
    void responseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer *osiosDhtPeer, const QByteArray &responseCryptographicHash);
private slots:
    void handleSocketToPeerConnected();
    void handleSocketToPeerReadyRead();
};

#endif // OSIOSDHTPEER_H

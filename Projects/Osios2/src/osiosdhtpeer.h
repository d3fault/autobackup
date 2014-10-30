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
        //HELLO (was calling this stuff BOOTSTRAP, but this is on a 1-connection basis whereas bootstrap is dht-wide
          Hello1of4_WeInitiatorAkaSender_SayingHelloIAmProfileNameAndWhatIThinkCurrentRollingHashForMyProfileIs
        , Hello2of4_WeResponder_SayingOhHaiProfileNameIamMyProfileNameAndMyCurrentRollingHashForMyProfileIsX_AndIThinkYourCurrentRollingHashForYourProfileIsY //sending Y is our our implicit ask for catching up, if needed
        , Hello3of4_WeInitiatorAkaSender_OkIllStartSendingTheOnesYouNeed_ThisIsWhatIThinkYourCurrentRollingHashIsAt
        //*joinee and joiner exchange the missing timeline nodes, if needed*/
        , Hello4of4_HelloCompleteBecauseYouHaveAllMyTimelineNodes_AndIHaveReceivedAllOfYours //both sender and receiver send this when appropriate, and both of them only ever move the connection to m_Healthy once it is valid for both (determined on self, received as message from other)
        //Hmm maybe if I just require bootstrapped state before allowing profile creation (the timeline node, both to disk and to neighbor) I can do without catch-up code (it will at least allow copycat to work if I did it that way), but still I mean there are node-joins (well not yet, what I mean is I want there to be able to be node joins. How else can you deal with node failure other than to get another node and to join. Whether or not we can log into any profile could depend on whether or not the dht state is bootstrapped, but I'm hesitant as to whether or not it should. In any case, I've realized that while I do want boostrap/catchup logic, I don't NEED it for testing the copycast code I've just written (so long as I don't enable profile manager's "ok" button until we are bootstrapped (and obviously "auto-login to last used profile on system bootstrap" opt-in checkbox :-). The GUI would become part of the logic (unless I use interface hacks (should, but idk if I will)), which means doing a CLI version becomes a tad harder

        //NORMAL OPERATION
        , SengingNewTimelineNodeForFirstStepOfCryptographicVerification
        , RespondingWithCryptographicHashComputedFromReceivedTimelineNode
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
    DhtPeerAddressAndPort peerConnectionInfo() const;
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

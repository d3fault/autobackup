#ifndef OSIOS_H
#define OSIOS_H

#include <QObject>
#include <QCryptographicHash>

#include "itimelinenode.h"
#include "osioscommon.h"

class QFile;

class OsiosDht;
class OsiosDhtPeer;

//the qba is timeline node hash
//the qset is list of peers that have verified the timeline node hash. we are removed from m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromTheNeighbors when a sufficient number are seen
typedef QHash<QByteArray, QSet<OsiosDhtPeer*>*> CryptographicHashAndTheListofDhtPeersThatHaveVerifiedItSoFar;

class Osios : public QObject
{
    Q_OBJECT
public:
    explicit Osios(const QString &profileName, quint16 localServerPort_OrZeroToChooseRandomPort, ListOfDhtPeerAddressesAndPorts bootstrapAddressesAndPorts, QObject *parent = 0);
    ~Osios();
    QList<TimelineNode> timelineNodes() const;
private:
    friend class TimelineSerializer;

    QString m_ProfileName;
    QList<TimelineNode> m_TimelineNodes;
    CryptographicHashAndTheListofDhtPeersThatHaveVerifiedItSoFar m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighbors_AndTheNeighborsWhoHaveVerifiedThisHashAlready;
    int m_LastSerializedTimelineIndex;
    QFile *m_LocalPersistenceDevice;
    QDataStream m_LocalPersistenceStream;
    OsiosDht *m_OsiosDht;

    void readInAllPreviouslySerializedEntries();
    ITimelineNode *deserializeNextTimelineNode();
    void serializeMyTimelineAdditionsLocally();
    void serializeTimelineActionLocally(TimelineNode action);
    void cyryptoNeighborReplicationVerificationStep0ofX_WeSender_propagateActionToNeighbors(TimelineNode action);
    void cyryptoNeighborReplicationVerificationStep1bOfX_WeReceiver_hashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
    void cryptoNeighborReplicationVerificationStep2ofX_WeSenderOfTImelineOriginallyAndNowReceiverOfHashVerification_removeThisHashFromAwaitingVerificationListAfterCheckingIfEnoughNeighborsHaveCryptographicallyVerifiedBecauseWeJustAddedAPeerToThatList_AndStopTheTimeoutForThatPieceIfRemoved(QByteArray keyToListToRemoveFrom, QSet<OsiosDhtPeer *> *listToMaybeRemove);
    void serializeNeighborActionLocally(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
    QByteArray calculateCrytographicHashOfTimelineNode(TimelineNode action, QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha1);
    inline QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
        {
            return inputString;
        }
        return inputString + "/";
    }
signals:
    void connectionColorChanged(int color);
    void notificationAvailable(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel = OsiosNotificationLevels::StandardNotificationLevel);
    void timelineNodeAdded(TimelineNode action);
public slots:
    void recordMyAction(TimelineNode action);
private slots:
    void flushDiskBuffer();
    void handleDhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum newDhtState);
    void cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
    void cyryptoNeighborReplicationVerificationStep2OfX_WeReceiver_handleResponseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer *osiosDhtPeer, QByteArray cryptoGraphicHashOfTimelineNodePreviouslySent);
};

#endif // OSIOS_H

#ifndef OSIOS_H
#define OSIOS_H

#include <QObject>
#include <QCryptographicHash>

#include "itimelinenode.h"
#include "osioscommon.h"

class QFile;

class IOsiosDhtBootstrapClient;
class IOsiosDhtBootstrapSplashScreen;
class IOsiosClient;
class OsiosDht;
class OsiosDhtPeer;

//the qba is timeline node hash
//the qset is list of peers that have verified the timeline node hash. we are removed from m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromTheNeighbors when a sufficient number are seen
typedef QHash<QByteArray, QSet<OsiosDhtPeer*>*> CryptographicHashAndTheListofDhtPeersThatHaveVerifiedItSoFar;
#define CryptographicHashAndTimeoutTimestamp QByteArray, qint64

class Osios : public QObject
{
    Q_OBJECT
public:
    static QByteArray calculateCrytographicHashOfByteArray(const QByteArray &inputByteArray);

    explicit Osios(QObject *parent = 0);
    ~Osios();
    QList<TimelineNode> timelineNodes() const;

    void setCopycatModeEnabledForUsername(const QString &nameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone);
private:
    friend class TimelineSerializer;

    QStringList m_Arguments;
    IOsiosDhtBootstrapClient *m_OsiosDhtBootstrapClient;
    QString m_ProfileNameUserWantsToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet; //TODOreq: merge with below m_ProfileName?
    QString m_ProfileName;
    QList<TimelineNode> m_TimelineNodes;
    CryptographicHashAndTheListofDhtPeersThatHaveVerifiedItSoFar m_RecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighbors_AndTheNeighborsWhoHaveVerifiedThisHashAlready;
    QHash<CryptographicHashAndTimeoutTimestamp> m_RecentlyGeneratedTimelineNodesAndTheirTimeoutTimestamps;
    int m_LastSerializedTimelineIndex;
    QFile *m_LocalPersistenceDevice;
    QDataStream m_LocalPersistenceStream;
    OsiosDht *m_OsiosDht;

    QString m_NameOfNeighborToCopycatActionsTimeline_OrEmptyStringIfNone;

    bool parseOptionalSettingsFilepathArgDidntReturnParseError(QString *theSettingsFilepathOutput);
    bool parseOptionalLocalServerPortFromArgsDidntReturnParseError(quint16 *portToParseInto);
    bool parseOptionalBootstrapAddressesAndPortsFromArgsDidntReturnParseError(ListOfDhtPeerAddressesAndPorts *listOfDhtPeersAddressesAndPortsToParseInto);

    bool checkWhetherOrNotCopycatShouldBeEnabledAndThenEnableItIfRelevant();
    IOsiosClient *profileAgnosticUiSetup();
    void connecToAndFromFrontendSignalsAndSlots(IOsiosClient *frontEnd);
    void beginUsingProfileNameInOsios(const QString &profileName);
    void readInAllMyPreviouslySerializedLocallyEntries();
    ITimelineNode *deserializeNextTimelineNode(TimelineNodeByteArrayContainsProfileNameEnum::TimelineNodeByteArrayContainsProfileNameEnumActual whetherOrNotTheByteArrayHasProfileNameInIt_IfYouGotItFromNetworkThenYesItDoesButIfFromDiskThenNoItDoesnt);
    void serializeMyTimelineAdditionsLocally();
    void serializeTimelineActionLocally(TimelineNode action);
    void cyryptoNeighborReplicationVerificationStep0ofX_WeSender_propagateActionToNeighbors(TimelineNode action, const QByteArray &cryptographicHashOfTheRenderingOfClientAfterTheActionWasApplied);
    void cyryptoNeighborReplicationVerificationStep1bOfX_WeReceiver_hashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action, const QByteArray &sha1OfRenderedWidgetPngBytes);
    void cryptoNeighborReplicationVerificationStep2ofX_WeSenderOfTImelineOriginallyAndNowReceiverOfHashVerification_removeThisHashFromAwaitingVerificationListAfterCheckingIfEnoughNeighborsHaveCryptographicallyVerifiedBecauseWeJustAddedAPeerToThatList_AndStopTheTimeoutForThatPieceIfRemoved(QByteArray keyToListToRemoveFrom, QSet<OsiosDhtPeer *> *listToMaybeRemove);
    void serializeNeighborActionLocally(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
    QByteArray calculateCrytographicHashOfTimelineNode(TimelineNode action);
    inline QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
        {
            return inputString;
        }
        return inputString + "/";
    }
    QString usage();
    void usageAndQuit();
signals:
    //bootstrapping
    void bootstrapped();

    //regular
    void connectionColorChanged(int color);
    void notificationAvailable(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel = OsiosNotificationLevels::StandardNotificationLevel);
    void timelineNodeAdded(TimelineNode action);

    //copycat
//#ifndef OSIOS_DHT_CONFIG_NEIGHBOR_SENDS_BACK_RENDERING_WITH_CRYPTOGRAPHIC_VERIFICATION_OF_TIMELINE_NODE
    //void timelineNodeReceivedFromCopycatTarget(TimelineNode timelineNode);
//#else
    void timelineNodeReceivedFromCopycatTarget(OsiosDhtPeer *osiosDhtPeer, TimelineNode timelineNode);
//#endif

    void quitRequested();
public slots:
    void initializeAndStart(IOsiosDhtBootstrapClient *osiosDhtBootstrapClient);
    void recordMyAction(TimelineNode action, const QByteArray &cryptographicHashOfTheRenderingOfClientAfterTheActionWasApplied);
private slots:
    void showProfileCreatorOrManagerOrSkipAndDisplayMainMenuIfRelevant();
    void flushDiskBuffer();
    void checkRecentlyGeneratedTimelineNodesAwaitingCryptographicVerificationFromMoreNeighborsForTimedOutTimelineNodes();
    void handleDhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum newDhtState);
    void cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
    void cyryptoNeighborReplicationVerificationStep2OfX_WeReceiver_handleResponseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer *osiosDhtPeer, QByteArray cryptographicHashOfTimelineNodeDataAndOfTimelineNodeRendering_OfTimelineNodePreviouslySent);

//#ifdef OSIOS_DHT_CONFIG_NEIGHBOR_SENDS_BACK_RENDERING_WITH_CRYPTOGRAPHIC_VERIFICATION_OF_TIMELINE_NODE
    void handleTimelineNodeAppliedAndRendered(OsiosDhtPeer *osiosDhtPeer, TimelineNode timelineNode, const QByteArray &sha1OfRenderedWidgetPngBytes);
//#endif
};

#endif // OSIOS_H

#ifndef OSIOS_H
#define OSIOS_H

#include <QObject>
#include <QCryptographicHash>

#include "itimelinenode.h"
#include "osiosdht.h"

class QFile;

class OsiosDht;
class OsiosDhtPeer;

class OsiosNotificationLevels
{
public:
    enum OsiosNotificationLevelsEnum
    {
          StandardNotificationLevel = 0
        , WarningNotificationLevel = 1
        , ErrorNotificationLevel = 2
        , FatalNotificationLevel = 3
    };
};

class Osios : public QObject
{
    Q_OBJECT
public:
    explicit Osios(const QString &profileName, quint16 localServerPort, ListOfDhtPeerAddressesAndPorts bootstrapAddressesAndPorts, QObject *parent = 0);
    ~Osios();
    QList<TimelineNode> timelineNodes() const;
private:
    friend class TimelineSerializer;

    QString m_ProfileName;
    QList<TimelineNode> m_TimelineNodes;
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
    void replicateNeighborActionLocally(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
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
public slots:
    void recordMyAction(TimelineNode action);
private slots:
    void flushDiskBuffer();
    void handleDhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum newDhtState);
    void cyryptoNeighborReplicationVerificationStep1aOfX_WeReceiver_storeAndHashNeighborsActionAndRespondWithHash(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
    void cyryptoNeighborReplicationVerificationStep2OfX_WeReceiver_handleResponseCryptoGraphicHashReceivedFromNeighbor(OsiosDhtPeer *osiosDhtPeer, QByteArray cryptoGraphicHashOfTimelineNodePreviouslySent);
};

#endif // OSIOS_H

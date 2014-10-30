#ifndef OSIOSDHT_H
#define OSIOSDHT_H

#include <QObject>
#include <QtNetwork/QHostAddress>

#include "osios.h"
#include "itimelinenode.h"

class QTcpServer;
class QTimer;

class Osios;
class OsiosDhtPeer;

#define OSIOS_DHT_MAP_TYPE_OF_PEERS_TO_TRY_CONNECTING_TO_RETRYING_WITH_EXPONENTIAL_BACKOFF qint64 /* next connect/hello attempt*/, QPair<int /* last exponent */, QUrl /*connection info*/> /*this comment is just to keep qt creator from fucking my space after the greater than*/

class OsiosDht : public QObject
{
    Q_OBJECT
public:
    explicit OsiosDht(Osios *osios, QObject *parent = 0);
    void sendNewTimelineNodeToAllDhtPeersWithHealthyConnectionForFirstStepOfCryptographicVerification(TimelineNode action);

    OsiosDhtStates::OsiosDhtStatesEnum dhtState() const;
    void setDhtState(const OsiosDhtStates::OsiosDhtStatesEnum &dhtState);
private:
    Osios *m_Osios;
    QTcpServer *m_LocalNetworkServer;
    QMap<OSIOS_DHT_MAP_TYPE_OF_PEERS_TO_TRY_CONNECTING_TO_RETRYING_WITH_EXPONENTIAL_BACKOFF> m_PotentialOrOldDhtPeersToConnectAndSayHelloToRetryingWithExponentialBackoff;
    QList<OsiosDhtPeer*> m_DhtPeersWithHealthyConnection; //fuck rpc generator and acking, cryptographic verifiability is better and weirdly easier (honestly i was just in refactor hell with rpc generator. the design wasn't THAT complex and i wasn't necessarily stuck). If any of the timeline nodes don't cryptographically ack a timeline node within... 5 seconds... the connection is no longer considered healthy and an error notification is presented on the screen of anyone that notices
    //QList<OsiosDhtPeer*> m_PotentialOrOldDhtPeersToConnectAndSayHelloToWithExponentialBackoff;
    OsiosDhtStates::OsiosDhtStatesEnum m_DhtState;

    QTimer *m_RetryConnectionAndSayHelloToWithExponentialBackoffTimer;

    quint16 generateRandomPort();
    void startLocalNetworkServer(quint16 localServerPort);
    void attemptToEstablishNetworkClientConnectionTo(DhtPeerAddressAndPort dhtPeerAddressAndPort);
    void maybeChangeStateBecauseThereWasNewConnection();
    void maybeChangeStateBecauseThereWasADisconnection();
signals:
    void notificationAvailable(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel = OsiosNotificationLevels::StandardNotificationLevel);
    void dhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum newDhtState);
    //void connectionToPeerEstablished(OsiosDhtPeer *newConnection); //could be incoming or outgoing, we don't care once it's established :-D
    //void timelineNodeReceivedFromPeer(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
public slots:
    void bootstrap(ListOfDhtPeerAddressesAndPorts bootstrapHostAddresses, quint16 localServerPort_OrZeroToChooseRandomPort);
private slots:
    void handleRetryConnectionAndSayHelloToWithExponentialBackoffTimerTimedOut();
    void handleLocalNetworkServerNewIncomingConnection();
    void handleOsiosDhtPeerConnected(OsiosDhtPeer *newDhtPeer);
    void handleConnectionNoLongerConsideredGood(OsiosDhtPeer *oldDhtPeer);
};

#endif // OSIOSDHT_H

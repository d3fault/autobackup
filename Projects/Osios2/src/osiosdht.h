#ifndef OSIOSDHT_H
#define OSIOSDHT_H

#include <QObject>
#include <QtNetwork/QHostAddress>

#include "itimelinenode.h"

typedef QPair<QHostAddress, quint16> DhtPeerAddressAndPort;
typedef QList<DhtPeerAddressAndPort> ListOfDhtPeerAddressesAndPorts;

class QTcpServer;

class Osios;
class OsiosDhtPeer;

class OsiosDhtStates
{
public:
    enum OsiosDhtStatesEnum
    {
          InitialIdleNoConnectionsState = 0 //black
        , BootstrappingForFirstTimeState = 1 //yellow
        , BootstrappedState = 2 //green
        , FellBelowMinBootstrapNodesState = 3 //this is more or less identical to BootstrappingForFirstTimeState, except we use red instead of yellow. TODOreq: red should stay on the notification plain text edit panel (at the bottom) indefinitely, until either fixed or "okay'd" (but when okay'ing, what should I set the color to? yellow). Honestly just a solid little square or any kind of graphic really would do wonders. The dht state is directly connected to that. The dht error notifications that are also send when red happens STILL should be specially shown in the notification log. Perhaps an upside-down tab widget used as a qdock widget (bottom) on the main window, and each tab is the different notification level (far left = all/aggregate). whenever an "error" notification is received, you have to switch to the error tab (we could use an asterisk on the tab to show that it has an entry (but this asterisk should only be used for red mode). of course, if the error resolves itself and we go back to green, maybe we should get rid of that asterisk-on-tab-widget-tab-for-error-notifications. I should maybe call it PendingErrorNotification, to indicate that it is solveable and once solved, deletes itself. For example it just takes note of the disconnected peer (IN the error notification itself!!! (fuck yea interfaces/pure virtuals)) and watches for the peer to reconnect (again, using an abstract interface "bool pendingErrorHasBeenResolved()"
        , CleanlyDisconnectingOutgoingNotAcceptingnewIncomingConnectionsState = 4 //cyan (had:orange)
    };
};

class OsiosDht : public QObject
{
    Q_OBJECT
public:
    explicit OsiosDht(Osios *osios, QObject *parent = 0);
    void sendNewTimelineNodeToAllDhtPeersWithHealthyConnectionForFirstStepOfCryptographicVerification(TimelineNode action);
private:
    Osios *m_Osios;
    QTcpServer *m_LocalNetworkServer;
    QList<OsiosDhtPeer*> m_DhtPeersWithHealthyConnection; //fuck rpc generator and acking, cryptographic verifiability is better and weirdly easier (honestly i was just in refactor hell with rpc generator. the design wasn't THAT complex and i wasn't necessarily stuck). If any of the timeline nodes don't cryptographically ack a timeline node within... 5 seconds... the connection is no longer considered healthy and an error notification is presented on the screen of anyone that notices TODOreq
    OsiosDhtStates::OsiosDhtStatesEnum m_DhtState;

    void startLocalNetworkServer(quint16 localServerPort);
    void setDhtState(const OsiosDhtStates::OsiosDhtStatesEnum &dhtState);
    void attemptToEstablishNetworkClientConnectionTo(DhtPeerAddressAndPort dhtPeerAddressAndPort);
    void maybeChangeStateBecauseThereWasNewConnection();
signals:
    void dhtStateChanged(OsiosDhtStates::OsiosDhtStatesEnum newDhtState);
    //void connectionToPeerEstablished(OsiosDhtPeer *newConnection); //could be incoming or outgoing, we don't care once it's established :-D
    //void timelineNodeReceivedFromPeer(OsiosDhtPeer *osiosDhtPeer, TimelineNode action);
public slots:
    void bootstrap(ListOfDhtPeerAddressesAndPorts bootstrapHostAddresses, quint16 localServerPort);
private slots:
    void handleLocalNetworkServerNewIncomingConnection();
    void handleOsiosDhtPeerConnected(OsiosDhtPeer *newDhtPeer);
};

#endif // OSIOSDHT_H

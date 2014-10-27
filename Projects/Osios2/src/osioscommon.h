#ifndef OSIOSCOMMON_H
#define OSIOSCOMMON_H

#include <QtNetwork/QHostAddress>

#define PROFILES_GOUP_SETTINGS_KEY "profiles"
#define OSIOS_DATA_DIR_SETTINGS_KEY "dataDir"

class MainMenuActivitiesEnum
{
public:
    enum MainMenuActivitiesEnumActual
    {
          TimelineViewActivity = 0
        , WriterActivity = 1
    };
};

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

//fuck it, stream to any single connection, or every. what exactly would be --
//OH RIGHT, the "green status" of knowing you have 3 nodes online! that's the utility of the bootstrap. Bootstrapping (yellow), At least 2 neighbors (green), less than 2 neighbors (red). STILL I obviously want to keep sending the timeline node stream when there is only 1 connection. So bootstrap is PURELY AESTHETIC (but aesthetics are core part of design. you SHOULD rage if your connection is red, because that means your data is not as safe as it would be if your connection is green)
#define OSIOS_DHT_MIN_PEERS_CONNECTED_FOR_BOOTSTRAP_TO_BE_CONSIDERED_COMPLETE 2

typedef QPair<QHostAddress, quint16> DhtPeerAddressAndPort;
typedef QList<DhtPeerAddressAndPort> ListOfDhtPeerAddressesAndPorts;

#endif // OSIOSCOMMON_H

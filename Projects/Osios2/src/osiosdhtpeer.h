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
        Hello1ofX_TODOreq_read_directly_below_comment
#if 0 //old, before realizing that the hello needs to sync ALL profiles on dht, just just profile chosen. tons of the comments are probably still relevant. Right now just to get copycat testing, I'm going to consider a peer "healthy" right after connection. After the hello protocol is in place, I'll consider it healthy as soon as the pull sync state is caught up (we can async send as the app is running, but actually maybe we shouldn't because we WANT ALL OF OUR SEND CONTENTS to be crypto verified before bootstrap complete)
          Hello1of4_WeInitiatorAkaSender_SayingHelloIAmProfileNameAndWhatIThinkCurrentRollingHashForMyProfileIs
        , Hello2of4_WeResponder_SayingOhHaiProfileNameIamMyProfileNameAndMyCurrentRollingHashForMyProfileIsX_AndIThinkYourCurrentRollingHashForYourProfileIsY //sending Y is our our implicit ask for catching up, if needed
        , Hello3of4_WeInitiatorAkaSender_OkIllStartSendingTheOnesYouNeed_ThisIsWhatIThinkYourCurrentRollingHashIsAt
        //*joinee and joiner exchange the missing timeline nodes, if needed*/
        , Hello4of4_HelloCompleteBecauseYouHaveAllMyTimelineNodes_AndIHaveReceivedAllOfYours //both sender and receiver send this when appropriate, and both of them only ever move the connection to m_Healthy once it is valid for both (determined on self, received as message from other)
        //Hmm maybe if I just require bootstrapped state before allowing profile creation (the timeline node, both to disk and to neighbor) I can do without catch-up code (it will at least allow copycat to work if I did it that way), but still I mean there are node-joins (well not yet, what I mean is I want there to be able to be node joins. How else can you deal with node failure other than to get another node and to join. Whether or not we can log into any profile could depend on whether or not the dht state is bootstrapped, but I'm hesitant as to whether or not it should. In any case, I've realized that while I do want boostrap/catchup logic, I don't NEED it for testing the copycast code I've just written (so long as I don't enable profile manager's "ok" button until we are bootstrapped (and obviously "auto-login to last used profile on system bootstrap" opt-in checkbox :-). The GUI would become part of the logic (unless I use interface hacks (should, but idk if I will)), which means doing a CLI version becomes a tad harder
        //TODOreq: The DHT node is fully operational even before the profile manager "ok" button is pressed. It need not be pressed and still does replicate and cryptographically verify hashes of neighbors. With the "auto-login on bootstrap" checkbox, we now require a File->Log Out action. It is worth noting that such logging out does NOT disconnect the DHT. It only that we go back to the profile manager to select a different profile
        //TODOreq: Uh create profile ability/widget also needs to be disabled until we are bootstrapped, until we get HELLO/replaying coded. Perhaps profile manager and create profile widgets should be refactored so and are made children of a "dht settings" (pseudo-main-menu) widget (allowing you to add bootstrap nodes at runtime, in addition to displaying bootstrap results). Really though, "DHT Settings" could be considered "Dht Bootstrap Splash" if/when the "auto-login on bootstrap" is checked. I supposed "Select Profile" and "Create Profile" could just be tabs on the "DHT Bootstrap Splash". TODOreq: when changing from the default "select profile" tab to "create profile", we need to un-check the "auto-login on bootstrap" checkbox so they can create their new profile duh
        //TODOoptional: bitches love fancy loading indicator animations (boostrapping... connected to 0/TARGET-NUM... bootstrapped! etc, but obviously all visual and shit)
        //Semi-OT, I think QSettings provides a run-time configurable to specify a settings file, so I can better test multiple profiles simultaneously without them all being a part of the same QSettings file/session (even though I do want that as well once I get client/daemonization xD)
        // /me wonders if I should create the interface needed to do a "dht bootstrapping splash" (and profile management) in the CLI later on -- err should I make the interface now even though I'm not going to do the CLI until later on? TODOreq: once client/deamonization is finished, gui daemon and cli daemon are identical (one just has more/different lib requirements). CLI client can connect to GUI daemon and vice versa. The target is one dht peer per machine (unless explicitly requested otherwise), N profiles per machine.
        // TODOreq: profile selection and profile creation are both disabled until bootstrap completes. Yes we still select last used profile after that happens, and yes we still auto-login-to-last-used-profile after that happens, but we can't allow them to select/create profile until they've received the list of already existing profiles (a la, they bootstrap). Hmm up till now I hadn't considered profile name exchanging to be part of the hello process, but I guess it can/should be because of that (can't create a profile name that already exists). I guess selecting your profile is ok, but it's semi-dangerous to allow because the profiles combo box will be updated with new profiles (either periodically when new applicable data is gathered, or right at bootstrap). Dht bootstrap splash almost sounds deserving of it's own pre-profile-stuff phase/dialog. Whether or not we combine it with profile stuff is implementation details. If I put dht splash all by itself (no profile stuff), there's a "auto-login" button that reflects your same choice in profile manager (which is displayed next). unchecking it overrides the auto-login feature of profile manager, obviously (but, *gasp*, unchecking it). We should show (in a read-only fashion) the last used profile that the auto-login checkbox correlates with (to help them with their decision)
#endif

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

#include <QApplication>
#include <QSettings>

#include "itimelinenode.h"
#include "osiosgui.h"

//As far as expanding this dht is concerned, a darknet would be relatively easy to code, including per-node public/private keys and etc. Public dhts are a bitch to code [securely]. For now this is going to simply be a dumb dht that sends it's messages to every peer, as well as saves every message from every peer it receives. It's a "I own all the computers the code will be running on" dht, so security is pretty lax (we trust that the machine isn't compromised to do the bulk of our security)

//TODOoptimization: keystrokes (etc) are too small to benefit from bit torrent, but if a peer connects and requests a/the full timeline node tree (or just a lot of it), we could utilize libtorrent to distribute it more efficiently. I guess it's a matter of a simple threshold check for how far behind they are

//Merging each of the 3 'profiles' (monitors/nodes) timeline node streams into one is doable, but I'm going to resist for now (KISS). The safety achieved with 3 independent streams (replicated in 2 places each) is all I want for now

//TODOreq: only one instance can be using a profile at a time. this change could/would/should be done at the same time that the daemonization/clientization of the application itself is performed (code-wise, not runtime-wise), or at least after it

//TODOmb: this app does not scale horizontally because every [dht] node sends every [timeline] node to every [dht] node xD. However, I'm not sure I care to solve the problem seeing as this app is intended private LAN use and is intended for BACKUP purposes (or, realtime data archival ;-P). BACKUP solutions aren't supposed to be efficient, they make tons of redundant copies

//TODOreq: right now the application state (dht state really) will look valid so long as no actions are performed. we should probably use a heartbeat to prove that the connection is still healthy, regardless of whether or not any actions are triggered

//If I ever do implement sharding (to make it scale better, regardless of whether or not it's public dht also), with care I could make the "replication strategy" a compile time define. Right now my replication is "EVERY NODE GETS EVERY NODE", but it could be "SHARD BLAH EFFICIENT-ER"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Osios");
    QCoreApplication::setOrganizationDomain("com.net.org.osios");
    QCoreApplication::setApplicationName("Osios");
    QSettings::setDefaultFormat(QSettings::IniFormat); //because we use the dir selected as our default data dir in profile create dialog

    OsiosGui gui;
    Q_UNUSED(gui)

    return a.exec();
}

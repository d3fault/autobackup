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

//TODOreq: offline read-only mode, perhaps a separate fucking binary to discourage use. I mean idk if it should be discouraged, but it's imparative that user input is literally disabled in such a mode. The argument for it's existence is that you should not have to be bootstrapped to access your data.  Proper offline/online sync will probably eventually come too, BUT AS I KEEP RE-REMEMBERING, I DON'T WANT OFFLINE MODE GAH (still, read-only is justified methinks (but still, creating a localhost dht is a matter of launching the binary 3 times xD (assuming you provide different --system-wide-settings for each instance))

//It's weird I keep wanting to contradict myself: when you say "operating system" and look at that by itself, it's STUPID AS FUCK to say that a network connection to 2 nodes (DERP BTW THE EASIEST WAY TO GET NON-NETWORK/single-offline USE WOULD BE TO PUT MIN_NEIGHBORS TO ZERO) is required before you can start using it. But then I re-remember AGAIN that the entire purpose of the app/os is data integrity/backup, so really it does make sense to require it. I'm entertaining (surely) the idea of selling kits of 3-5 computers all in one single unit (independent power supplies (one charge port though), etc). Completely open source build instructions from easily attainable components and all free software, with of course a bitchin easy to read (pictures) or watch (vidya) tutorial on assembly (fun). Or pay a little extra (bitch/pussy) and it comes pre-assembled. Integrate with WASDF and oh my.

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

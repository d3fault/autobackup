#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

- make some use of auto-failover shit?
- make slot fillers upload max is accurate. currently a < 1mb image (just barely, was testing) becomes 2808647 bytes before going onto the queue. b64 makes it ~1.5x larger, and the rest is from boost serialization. either lower the displayed/wt-config'd max, or raise the queue message size, idgaf


==1.1 Bugs==

- fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
- get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))
- buy slot[s], log out, log in as new user, old "attempting/SUCCESS" messages still there. only visual artifacts, idgaf
- literal question marks, commas, and exclaimation points in ad slot filler hovertext. might be easy but i suck at regex and don't want to open up every character in doing so. also don't care enough to research it further
- subscription update to all old subscribers (could be ~10k if app scales like it should), which only happens at buy event, shouldn't make the db unusable and currently would/does. basically just put the series of WServer::post calls on a separate thread (or all the get and subscribe stuff on a separate thread...)

*/

int main(int argc, char **argv)
{
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}

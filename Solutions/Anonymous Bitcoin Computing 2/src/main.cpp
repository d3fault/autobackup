#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

- exponential backoff for app ops, namely key claiming and key range claiming, but others may apply
- durability polling? worth it? on adds it should always succeed, but cas swaps (even asked on their forum thing). also realized that before a durability poll completes, the key can still be get'd. even when my durability fails, at this point my code just cerr's the dbFail and that's it (whereas it SHOULD... err... failover and retry?? idfk but i'm starting to not like couchbase :-/. BUT that's the entire point. the durability polling fails is SUPPOSED to indicate the value isn't stored, except with couchbase it doesn't (the value has to have been stored [on only one node] before durability polling even begins, *sigh*))
- doing any app-op multiple times in same session functions correctly (re-using widgets where appropriate, namely not segfaulting xD)
- No ops cause too big of messages to be sent through the message queue. This is mostly done already via sanitization, but one place that needs some thought is 'all ad slot fillers' doc. Either need a max amount of ad slot fillers.. or... [???]. The fact that nickname can be on it makes it variable (but nickname is max 64 chars, so...). It is not a problem when GET'ing the doc (std::string dynamic), but only when STORE'ing it (message queue fixed message size)


==1.1 Bugs==

- fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
- get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))
- buy slot[s], log out, log in as new user, old "attempting/SUCCESS" messages still there. only visual artifacts, idgaf
- literal question marks, commas, and exclaimation points in ad slot filler hovertext. might be easy but i suck at regex and don't want to open up every character in doing so. also don't care enough to research it further
- subscription update to all old subscribers (could be ~10k if app scales like it should), which only happens at buy event, shouldn't make the db unusable and currently would/does. basically just put the series of WServer::post calls on a separate thread (or all the get and subscribe stuff on a separate thread...)

*/

//TODOreq: two tabs open sharing same session, one of them performs an action that then deletes a WContainerWidget [that is still visiable/cached on the other tab]. The other tab with the cached view then performs an action on the deleted WContainerWidget. segfault? My login/logout widgets comes to mind, because I was thinking about 'deleting' the "welcome, [name]" shit on logout and recreating it again on login (but i guess i don't have to). Safest way is to just never delete anything and only let WApplication do it when it is deleted
//^this appears to be handled correctly by Wt, which states that the signal is not exposed. HOWEVER, it made me think of a race condition and ultimately makes me wonder even further what exactly deferRendering does. Initiate an op that does one or more messages to db, click 'log out' button (on other tab with same session). I guess really I'm just repeating myself here, but idk like for example if I cleared the username on logout, and then the async callbacks later used that username for something, it could be a json injection or segfault
int main(int argc, char **argv)
{
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}

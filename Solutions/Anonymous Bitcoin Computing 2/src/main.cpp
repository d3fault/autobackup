#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

- exponential backoff (app + db + recovery-possy)
- all ad slot fillers preview (view query or expensive? maybe just expensive but only for owner as optimization (not security/privacy(LOL)). would still warrant pagination imo)
- doing any app-op multiple times in same session functions correctly (re-using widgets where appropriate, namely not segfaulting xD)
- No ops cause too big of messages to be sent through the message queue
- upload no-defer bug


==1.1 Bugs==

- fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
- get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))
- buy slot[s], log out, log in as new user, old "attempting/SUCCESS" messages still there. only visual artifacts, idgaf
- literal question marks, commas, and exclaimation points in ad slot filler hovertext. might be easy but i suck at regex and don't want to open up every character in doing so. also don't care enough to research it further

*/

//TODOreq: two tabs open sharing same session, one of them performs an action that then deletes a WContainerWidget [that is still visiable/cached on the other tab]. The other tab with the cached view then performs an action on the deleted WContainerWidget. segfault? My login/logout widgets comes to mind, because I was thinking about 'deleting' the "welcome, [name]" shit on logout and recreating it again on login (but i guess i don't have to). Safest way is to just never delete anything and only let WApplication do it when it is deleted
//^this appears to be handled correctly by Wt, which states that the signal is not exposed. HOWEVER, it made me think of a race condition and ultimately makes me wonder even further what exactly deferRendering does. Initiate an op that does one or more messages to db, click 'log out' button (on other tab with same session). I guess really I'm just repeating myself here, but idk like for example if I cleared the username on logout, and then the async callbacks later used that username for something, it could be a json injection or segfault
int main(int argc, char **argv)
{
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}

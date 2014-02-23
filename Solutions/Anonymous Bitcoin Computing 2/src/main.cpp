#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

- exponential backoff (app + db + recovery-possy)
- test sanitization
- all ad slot fillers preview (view query or expensive? maybe just expensive but only for owner as optimization (not security/privacy(LOL)). would still warrant pagination imo)
- doing any app-op multiple times in same session functions correctly (re-using widgets where appropriate, namely not segfaulting xD)
- No ops cause too big of messages to be sent through the message queue
- missing the slot (confirmed miss) unlocks user account (just jump to login recovery code that already does that (to be safe, jump to the pre-confirm-i-didnt-get-the-slot code)). i guess also related to this is that if a no-js user presses buy step 2 a long time after the buy event, we go all the way to locking and attempting to LCB_ADD before we see it being full.. so maybe no-js should do a campaign-get to see if the one they are saying they are interested in after pressing buy step 2 is still even available? i guess really as long as the user-account auto-unlocks it's fine the way it is


==1.1 Bugs==

- fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
- get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))
- buy slot[s], log out, log in as new user, old "attempting/SUCCESS" messages still there. only visual artifacts, idgaf

*/

//TODOreq: two tabs open sharing same session, one of them performs an action that then deletes a WContainerWidget [that is still visiable/cached on the other tab]. The other tab with the cached view then performs an action on the deleted WContainerWidget. segfault? My login/logout widgets comes to mind, because I was thinking about 'deleting' the "welcome, [name]" shit on logout and recreating it again on login (but i guess i don't have to). Safest way is to just never delete anything and only let WApplication do it when it is deleted
int main(int argc, char **argv)
{
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}

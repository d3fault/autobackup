#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

1) send 'time' for currentPrice calculation
2) 'buy what intended and never what was just doubled to moments ago' race condition, rolling back to pre-step 1 on buy event. ultra fast double click (just did on accident) buys two in a row (in fact you can buy as many as you want, BUT definitely need to ignore '2nd' click if faster than gui can make buttan disappear)
3) exponential backoff (app + db + recovery-possy)
4) couchbase key collisions from usernames
5) sanitize
6) all ad slot fillers preview (view query or expensive? maybe just expensive but only for owner as optimization (not security/privacy(LOL)). would still warrant pagination imo)
7) doing any app-op multiple times in same session functions correctly (re-using widgets where appropriate, namely not segfaulting xD)
8) No ops cause too big of messages to be sent through the message queue
9) Tell date range (day) for currently purchaseable slot
10) js-slot-buy doesn't update current price up top [for himself] (or maybe it does, can't tell)
11) Login -> Buy Step 1 -> Log out -> Log in as different user: see first user's ad slot fillers (gui problem, db adds using index and correct username)

==1.1 Bugs==

fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))

*/

//TODOreq: two tabs open sharing same session, one of them performs an action that then deletes a WContainerWidget [that is still visiable/cached on the other tab]. The other tab with the cached view then performs an action on the deleted WContainerWidget. segfault? My login/logout widgets comes to mind, because I was thinking about 'deleting' the "welcome, [name]" shit on logout and recreating it again on login (but i guess i don't have to). Safest way is to just never delete anything and only let WApplication do it when it is deleted
int main(int argc, char **argv)
{
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}

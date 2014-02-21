#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

send 'time' for currentPrice calculation
'buy what intended and never what was just doubled to moments ago' race condition, rolling back to pre-step 1 on buy event. ultra fast double click (just did on accident) buys two in a row (in fact you can buy as many as you want, BUT definitely need to ignore '2nd' click if faster than gui can make buttan disappear)
exponential backoff (app + db + recovery-possy)
couchbase key collisions from usernames
sanitize
upload as many ad slot fillers as you want without new session
all ad slot fillers preview (view query or expensive? maybe just expensive but only for owner as optimization (not security/privacy(LOL)). would still warrant pagination imo)
doing any app-op multiple times in same session functions correctly (re-using widgets where appropriate, namely not segfaulting xD)
unsubscribe segfault (applies to changing session id too)

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

#include "anonymousbitcoincomputing.h"

/*

==1.0 [Launch] Blockers==

- make some use of auto-failover shit?
- doing any app-op multiple times in same session functions correctly (re-using widgets where appropriate, namely not segfaulting xD)
- No ops cause too big of messages to be sent through the message queue. This is mostly done already via sanitization
- satoshis, since i've witnessed the hilarious rounding stuff (only where it matters (js does not))
- no-js: "Sorry, someone bought the slot just before you" doesn't get current slot value, so same error message over and over. this is related to not having no-js pull from subscription cache (although it's a slightly different path (the 'resubscribe no-js' variant))
- clicking buy step 1 with no-js doesn't update to most recent price (if just bought in another tab/session), and obviously results in "Sorry, someone bought...". The correct thing to do is to show the updated price right when they click buy step 1 (thought i was doing this).
- no-js, trying to trigger 'sorry, somoene bought the slot' resulted in "Don't use multiple tabs/windows plz", and still seeing the stale value. trying to buy again hangs (no segfault). trying to buy NEXT in the tab used to attempt to trigger 'sorry' also hangs, wtf
- when the tab that 'was just behind' goes ahead with buy event, it CAN NOT put the user account into attempting to buy slot mode, because if it's put into attempting to buy slot mode for one that's already purchased by us in another tab, we have fucked the state entirely. the login recovery sees it as "ok recovery possy will fix it" (since it points to slot we already own (not necessarily, but in my test case yes)), but the recovery possy doesn't do anything because the campaign doc is already up to date from the 'other' tab (the one that made the other one behind) purchasing it. handling this incorrectly would cause the user's account to be debittted twice, since it has been locked to it twice (the second time shouldn't have). this bug makes suicide sound tasty. hmm actually i think just checking that the slot we're about to lock to doesn't already exist just before locking should fix it, because cas itself handles the race condition of neighbors "double locking" in that case. the problem only/mainly arises when the same user that purchased the slot locks AGAIN their account towards the slot. if it is a different user, login recovery would have handled it. so checking it doesn't already exist + cas-lock-user-account(already-doing) is the solution methinks
- ^^^be careful that if you do a 'get current user account' just before the 'ensure slot doesn't already exist'... bleh lost my train of thought but had something to do with maybe a race condition where the neighbor-session/same-user locks+unlocks, but actually cas should stop that so i think nevermind (and if it's unlocked, then the slot [that we haven't yet checked] will definitely be filled)
- ^^^^^should we check that the slot doesn't exist or should we get the campaign doc again? i think the slot existence check is betterer because of fail conditions AFTER the slot filler (campaign doc not getting updated). the user account could have already been debitted (allowing re-lock!) without the campaign doc being updated, so yes CHECK SLOT EXISTENCE NOT CAMPAIGN DOC (seems dumb to check it twice (2nd time being the LCB_ADD itself), but appears to be necessary at this point). AND OF COURSE check that slot attempting to fill isn't now set, but that should seem obvious
- ^^^^^^^^NOPE: WAIT, WHAT THE FUCK? how was the 'sorry just missed it' tab even able to lock the user-account? his cas-swap-lock of the user-account should have fucking failed!!!!!!! or have i since changed the code to get the user account right when/after buy step 2 is pressed? i can't remember anymore gahhhhh suicidieididieidiedieididieidieidiedieidie. maybe my cas shit isn't working though. nvm i do do a fresh get of user account right after buy step 2 is clicked, i was confusing it for the campaign doc, which is already in memory at that time
- buy step 1 updates the price, but using possibly stale information. have already mentioned this above, but should re-get the campaign doc after buy step 1 is clicked [when no-js]. getting from get and subscribe cache is a-ok, BUT can't use the same "doHacky" method because that one ends up with us in pre-step1-clicked. still a copy/paste job with slight modification is in order

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

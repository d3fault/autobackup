#include "anonymousbitcoincomputing.h"

/*

==1.0 BLOCKER Bugs==
- Price isn't updated for the purchaser of a slot after they purchase (it is being purchased, and observing the campaign on a different tab does update the price). This includes rolling back to before step 1 (although both step buttons remained grayed out at least). I think this is related to the new multi campaign owner code, and I'm seeing an error "I'm not sure what it means if we get a change session id request for a wapplication that isn't subscribed..." that I've never seen before as well. OMGWTF IT DISAPPEARED!!! nvm it's back again (WTF WTF)! WTF IT'S GONE AGAIN! intermittent bugs ftl, but for the record I've only seen it with multi campaign owners enabled. ok back again and this time it caused a segfault at buy step 2 (line 571 of wt frontend, a freaking read_json call, but wtf the json string looks valid to me!?!?)! rofl. ok scratch that, even present on single user mode WTFWTF. hard to trigger but if i keep logging in with new tabs (leaving other ones open) and buying then it eventually triggers the segfault at that same spot. MAYBE i'm not using std::string (the json doc) in a thread-safe manner, and it's intermittent because sometimes the backend thread hasn't deleted the string and sometimes it has?? bleh qstring so much easier xD, but I've gone through so much effort to not depend on Qt (why, again?). Ok I think the segfault was because boost spirit didn't have a multi-threaded flag defined, but still seeing the "session" error first descbribed (I guess they are unrelated). HOWEVER i'm still inclined to move the fuck away from std::string since it might use COW (idfk). I should just copy the bytes and pass a void pointer like a real man (no std::string should cross thread boundaries (not even the key input! TODOreq))
STEPS TO REPRODUCE CHANGE SESSION ERROR:
0) open tab, open d3fault's campaign 0
1) open tab, open user2's campaign 0, log in, buy slot
^^found the REASON (but not the source/solution): change session id is looking in the wrong cacheItem (key) for the wtapp ptr. user2 looks in d3fault and/or vice versa. weird that it's so intermittent (but with the above steps i get it pretty often). LOL'D so hard when I finally fucking solved this (hours digging/debugging): I need per-key lists for m_PendingChangeSessionId and m_PendingUnsubscribes. The reason it was intermittent was because whichever polling timeout timed out first (incl the getCallback) would then process the pending ones, regardless of if it was even for the key that just finished being gotten xD. Solution is ez as fuck, just move the lists into cacheItem :-P. Explains why the bug was about 50/50 intermittent hahahahahaha

- Segfault when stopping the app, I think related to above bug
- Saw a negative value for the "You will get the ad space for no more than" price. Weirdly, the first shown price was correct (it was min price, so maybe I am calculating the price for "You will get the ad space for no more than" when I should be using min price). Would be hilarious if I tried to "deduct" a negative value and went broke because of it xD (TODOreq: as a safeguard, check the amount deducting is > 0 and then scream loudly etc (and obviously don't do the deduction) right before doing the maths. Check earlier too before even attempting to fill the slot (perhaps right at buy step 1 clicked, maybe EVEN EARLIER -- but meh I think this is just a visual bug))

==1.1 Bugs==

- A lot of my error messages (not logged in, 500 internal server error, etc -- but also even things like "you successfully bought" too. but come to think of it, if it doesn't work without javascript, fuck that) should be in a messagebox instead of just added to the current page. I wasn't aware of Wt's message box functionality until recently
- the all ad slot fillers pages hypothetical ddos where tons of images are still in memory and never requested. a stupid but effective way to solve this is to use wfileresource instead of a memory resource. stupid because ddb -> hdd -> serve -> delete hdd copy, but effective because don't have to worry about running out of hdd space (well, still do, but not nearly as fast as memory). other options include timeouts for keeping the resource in memory (few seconds), etc. another weird solution would be to block the resource request and to get the ddb doc on demand, though this seems grossly inefficient (i wonder, but doubt, if request continuations would help here)
- fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
- get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))
- buy slot[s], log out, log in as new user, old "attempting/SUCCESS" messages still there. only visual artifacts, idgaf
- literal question marks, commas, and exclaimation points in ad slot filler hovertext. might be easy but i suck at regex and don't want to open up every character in doing so. also don't care enough to research it further
- subscription update to all old subscribers (could be ~10k if app scales like it should), which only happens at buy event, shouldn't make the db unusable and currently would/does. basically just put the series of WServer::post calls on a separate thread (or all the get and subscribe stuff on a separate thread...)
- find a more optimal max message size queue for StoreLarge. right now it's larger than it needs to be, but i'm too lazy to calculate the exact max
- find way to accept png/gif/webp when no animation used. svg will probably never be added because scripting (but really, i don't know what context the js/etc runs in (still the problem of animation EVEN IF the scripting is 'safe'))...

==68.9999 Optimizations==

- A lot of places use delete/new WContainerWidget, instead of just WContainerWidget::clear
- Might be possible to use 10x (N, really) identical backend/db threads (frontend would need minimal changing since it already uses N lockfree::queues). I was unsure how libevent worked at the time of writing, but now I guess I've just sat on the knowledge longer and I think unique libevent contexts in their own threads would work just fine. There was some doc somewhere on the couchbase site saying something about whether sharing couchbase instances being safe or not depending on the libevent usage (so I assumed that meant "nope not safe" (I wasn't wrong, but the solution is 1:1:1 libevent:couchbase:thread (I *think*))). However, that means we'll have 10x as many connections to the couchbase nodes

*/

//I'd imagine lots would want to use this + hackyvideobullshitsite themselves (yw)... so here's a note to anyone genricizing (making not-'d3fault' specific): abstract my specific stuff into some kind of "profile" loaded at runtime or whatever, that way i can pull your changes back in and spread the software further around :-P (row row fight the powa)
int main(int argc, char *argv[])
{
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}

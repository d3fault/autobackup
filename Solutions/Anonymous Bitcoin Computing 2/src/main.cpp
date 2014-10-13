#include "anonymousbitcoincomputing.h"

/*

==1.0 BLOCKER Bugs==
- move the fuck away from std::string for cross thread comm, since string might use COW (implementation dependent), and COW means internal reference count, and that means it isn't thread-safe (since it doesn't declare itself as such). I should just copy the bytes and pass a void pointer like a real man (no std::string should cross thread boundaries (not even the key input! TODOreq)). I think I've just been getting really lucky so far xD

==1.1 Bugs==

- A lot of my error messages (not logged in, 500 internal server error, etc -- but also even things like "you successfully bought" too. but come to think of it, if it doesn't work without javascript, fuck that -- it should work without js soon, it's a pending wt bug report) should be in a messagebox instead of just added to the current page. I wasn't aware of Wt's message box functionality until recently
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

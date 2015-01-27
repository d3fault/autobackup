#include <Wt/WServer>
#include <QCoreApplication>

#include "anonymousbitcoincomputing.h"

/*

Test the html encoding of urls
width/height (ad slot filler upload, ad campaign preview, ad campaign purchase/size-match-verification -- no need for api because they will 'know')
optional description (512)

==1.1 Bugs==

- API: user supplied push-url (http) triggers api pull (https) -- solves 'security' problem of push. could/should be incorporated into the php client example
- Ad Campaigns should properly 404, be careful not to delete/clear anything that might cause segfault (zero the pointer out if deleting (but just don't, since there are tons of children doing the same thing xD fml))
- C++11 makes cow in std::string illegal, so even gcc/clang won't have it anymore. I should use shared_ptr probably, now that I finally understand it
- A lot of my error messages (not logged in, 500 internal server error, etc -- but also even things like "you successfully bought" too. but come to think of it, if it doesn't work without javascript, fuck that -- it should work without js soon, it's a pending wt bug report) should be in a messagebox instead of just added to the current page. I wasn't aware of Wt's message box functionality until recently
- the all ad slot fillers pages hypothetical ddos where tons of images are still in memory and never requested. a stupid but effective way to solve this is to use wfileresource instead of a memory resource. stupid because ddb -> hdd -> serve -> delete hdd copy, but effective because don't have to worry about running out of hdd space (well, still do, but not nearly as fast as memory). other options include timeouts for keeping the resource in memory (few seconds), etc. another weird solution would be to block the resource request and to get the ddb doc on demand, though this seems grossly inefficient (i wonder, but doubt, if request continuations would help here)
- fail after user-account locked to buying a slot -> allow recovery via buying slot locked to
- get-and-subscribe only works with one key (timeout stuff needs changing (all share timer? or each get own?), as does other stuff such as subscription-update callback receiving update to something we just unsubscribed from (segfaulting almost every time))
- buy slot[s], log out, log in as new user, old "attempting/SUCCESS" messages still there. only visual artifacts, idgaf
- literal question marks, commas, and exclaimation points in ad slot filler hovertext. might be easy but i suck at regex and don't want to open up every character in doing so. also don't care enough to research it further
- subscription update to all old subscribers (could be ~10k if app scales like it should), which only happens at buy event, shouldn't make the db unusable and currently would/does. basically just put the series of WServer::post calls on a separate thread (or all the get and subscribe stuff on a separate thread...)
- find a more optimal max message size queue for StoreLarge. right now it's larger than it needs to be, but i'm too lazy to calculate the exact max
- find way to accept png/gif/webp when no animation used. svg will probably never be added because scripting (but really, i don't know what context the js/etc runs in (still the problem of animation EVEN IF the scripting is 'safe'))...
- move the fuck away from std::string for cross thread comm, since string might use COW (implementation dependent), and COW means internal reference count, and that means it isn't thread-safe (since it doesn't declare itself as such). I should just copy the bytes and pass a void pointer like a real man (no std::string should cross thread boundaries (not even the key input! TODOreq)). I think I've just been getting really lucky so far xD. Actually fuck it, since both GCC and Clang (the only two relevant compilers in existence) use atomic reference counting and are more or less thread safe, I'll just leave it because I'm a lazy bastard. Also worth noting that if I'm NOT using lockfree::queue, this isn't an issue [since I'm making deep copies during serialization]
- change password functionality

==68.9999 Optimizations==

- A lot of places use delete/new WContainerWidget, instead of just WContainerWidget::clear. Make sure not to call clear() when there are 'placeholder members' (WTexts, usually) that can be updated later, such as when navigating back to the widget but with different params (different users, indexes, etc)
- Might be possible to use 10x (N, really) identical backend/db threads (frontend would need minimal changing since it already uses N lockfree::queues). I was unsure how libevent worked at the time of writing, but now I guess I've just sat on the knowledge longer and I think unique libevent contexts in their own threads would work just fine. There was some doc somewhere on the couchbase site saying something about whether sharing couchbase instances being safe or not depending on the libevent usage (so I assumed that meant "nope not safe" (I wasn't wrong, but the solution is 1:1:1 libevent:couchbase:thread (I *think*))). However, that means we'll have 10x as many connections to the couchbase nodes. Sharing the various backend "caches" (as would be desired) among each of those 10 would also be a royal pain in the ass, so fuck it I think

*/

//Note: storing unencoded user input as a couchbase doc/VALUE could lead to weird/infinite problems. Lots of couchbase views (the examples especially) are not written with "key prefix checking", but instead "json field existence checking". If for example you accept "image uploads" (like this site), and you don't base64 encode the image bytes BEFORE storing in couchbase (yes, couchbase stores in base64, but ONLY IF it isn't valid json, so...), the user could supply a json doc instead and could manipulate the results of your view queries in any arbitrary way they desire. I'm of course ignoring the fact that my image uploads are being strictly sanitized to either BMP/JPEG (and are therefore not valid json (but shit I wouldn't be surprised if there was some 'valid bmp/jpeg header that is also valid json')), that's just an example. That couchbase base64 encodes non-json data gives a false sense of security. You must either base64/hex/etc encode it before handing to couchbase, OR _EVERY_ view you write must do key prefix checking (like my one view does now) (and of course your user input keys would use different key prefixes)

//I'd imagine lots would want to use this + hackyvideobullshitsite themselves (yw)... so here's a note to anyone genricizing (making not-'d3fault' specific): abstract my specific stuff into some kind of "profile" loaded at runtime or whatever, that way i can pull your changes back in and spread the software further around :-P (row row fight the powa)
//TODOoptional: any time I create a db document (as opposed to modifying one), whether in this app or in recovery possy etc, I should use some method that returns a ptree with the document's defaults. This allows me to add fields without having to change code in lots of supporting places. I'm scared to add fields now because I'd have to search for all the places where the same doc is created (and then add the field)
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    AnonymousBitcoinComputing abc;
    return abc.startAbcAndWaitForFinished(argc, argv);
}

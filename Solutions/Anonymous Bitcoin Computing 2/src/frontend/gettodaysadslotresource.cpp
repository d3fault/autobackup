#include "gettodaysadslotresource.h"

#if 0
//TODOreq: ONE instance of this resource handles all requests [concurrently], so thread safety/etc must be considered. Good thing lockfree::queue does the heavy lifting for me :-D
//Shit I was planning on passing the wresource pointer to the backend along with the request and having it call haveMoreData when the couchbase db hit finishes, but since there is only one instance of the resource, I'm not sure I am able to have the "one resource" provide "many different resources" (based on the request params). Surely there is a way to do what I desire, otherwise Wt sucks. Wt allows for delivering different resources based on params, BUT it appears that it is NOT possible when the waitForMoreData/haveMoreData (async) functionality is used. haveMoreData() should be a method on the continuation object itself, not the resource object. Additionally, it appears calling haveMoreData (which MIGHT be threadsafe, I'm not even sure of that tbh) serves up the response. I don't want my couchbase db thread to be serving up web replies xD (using ANOTHER thread solves this (but introduces more complexity fml))
//Perhaps I should research a solution not involving Wt (but then guh it won't use my same lcb_t that abc2 is using (still, async is much better than sync))
GetTodaysAdSlotResource::GetTodaysAdSlotResource(WObject *parent)
    : WResource(parent)
{ }
void GetTodaysAdSlotResource::handleRequest(const Http::Request &request, Http::Response &response)
{

}
GetTodaysAdSlotResource::~GetTodaysAdSlotResource()
{
    beingDeleted();
}
#endif

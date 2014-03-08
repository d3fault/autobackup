#include "gettodaysadslotwresource.h"

#include <Wt/Http/Response>
#include <Wt/WDateTime>

#include "synchronoustodaysadslotgetter.h"

GetTodaysAdSlotWResource::GetTodaysAdSlotWResource(WObject *parent)
    : WResource(parent)
{ }
GetTodaysAdSlotWResource::~GetTodaysAdSlotWResource()
{
    beingDeleted();
}
void GetTodaysAdSlotWResource::handleRequest(const Http::Request &request, Http::Response &response)
{
    (void)request;
    //TODOreq: something <3
}
//I don't know if I'm going to have the constructor call this or maybe a timer or what, but I do know I'll call it at some point, so it's my starting point. Here's what it does: gets the "current slot cache", but if it doesn't exist then it just crawls up the slots incrementing 1 at a time until it either finds one where the date range is TODAY, or it finds a non-existent slot (in which case, we... I guess... return a placeholder ("no ad") that is valid for like 5 mins or so? 5 mins to serve as a reasonable max before a purchase will show up, etc idfk). In both cases, we set up the slot cache to optimize our own next run. I am thinking that this resource will own a mutex that it will lock before calling this method, or something.
void GetTodaysAdSlotWResource::getTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex)
{
    SynchronousTodaysAdSlotGetter getter; //scoped var because no need to keep the couchbase object/connections open for 23 hours of the day, nah mean. TODOreq: before it goes out of scope, we should save as a member the ad for the day and also when it expires (we cache it (hmm actually not sure that's a good idea seeing as it'll likely only be requested once or twice (or N where N is amount of 'users' of abc2, but there's so many options here in distributing it amongst those N in a more efficient manner (to each other) it blah)))
    getter.getTodaysAdSlot(campaignOwnerUsername, campaignIndex, static_cast<long long>(WDateTime::currentDateTime().toTime_t()));

    //maybe we should/could grab getters results before it goes out of scope, or perhaps just a return value? idk still thinking about it all
}

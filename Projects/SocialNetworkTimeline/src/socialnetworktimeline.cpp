#include "socialnetworktimeline.h"

#include "stupidkeyvaluecontenttracker.h"

//TODOreq: "last modified timestamps of _IMPORTED_ files" needs to live somewhere!! -- see stupidkeyvaluecontenttracker.cpp comments for more thoughts on this (because I'm not sure yet at which layer it belongs)
SocialNetworkTimeline::SocialNetworkTimeline(QObject *parent)
    : QObject(parent)
{
    StupidKeyValueContentTracker *keyValueStore_WithHistory = new StupidKeyValueContentTracker(this);
    StupidKeyValueContentTracker::establishConnectionsToAndFromBackendAndUi<SocialNetworkTimeline>(keyValueStore_WithHistory, this);
    keyValueStore_WithHistory->initialize();
}
void SocialNetworkTimeline::appendJsonObjectToTimeline(const QJsonObject &data)
{
    //should this be 'insert' instead of 'append'? Can we add things with timestamps that happened years ago? should the desired timestamp be an arg TODOreq?
}

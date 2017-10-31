#include "socialnetworktimeline.h"

#include "stupidkeyvaluecontenttracker.h"

//TODOreq: "last modified timestamps of _IMPORTED_ files" needs to live somewhere!! -- see stupidkeyvaluecontenttracker.cpp comments for more thoughts on this (because I'm not sure yet at which layer it belongs)
SocialNetworkTimeline::SocialNetworkTimeline(QObject *parent)
    : QObject(parent)
{
    StupidKeyValueContentTracker *keyValueStore_WithHistory = new StupidKeyValueContentTracker(this);
    StupidKeyValueContentTracker::establishConnectionsToAndFromBackendAndUi<SocialNetworkTimeline>(keyValueStore_WithHistory, this);

    connect(keyValueStore_WithHistory, &StupidKeyValueContentTracker::initializeFinished, this, &SocialNetworkTimeline::initializeFinished);
}
void SocialNetworkTimeline::initialize()
{
    emit initializeRequested();
}
void SocialNetworkTimeline::appendJsonObjectToTimeline(const QJsonObject &data)
{
    //should this be 'insert' instead of 'append'? Can we add things with timestamps that happened years ago? should the desired timestamp be an arg TODOreq?
}
void SocialNetworkTimeline::handleE(QString msg)
{
    emit e(msg);
}
void SocialNetworkTimeline::handleO(QString msg)
{
    emit o(msg);
}
void SocialNetworkTimeline::handleInitializeFinished(bool success)
{
    //TODOstub
    qWarning("stub not implemented: SocialNetworkTimeline::handleInitializeFinished(bool success)");
}
void SocialNetworkTimeline::handleAddFinished(bool success)
{
    //TODOstub
    qWarning("stub not implemented: SocialNetworkTimeline::handleAddFinished(bool success)");
}
void SocialNetworkTimeline::handleModifyFinished(bool success)
{
    //TODOstub
    qWarning("stub not implemented: SocialNetworkTimeline::handleModifyFinished(bool success)");
}
void SocialNetworkTimeline::handleRemoveKeyFinished(bool success)
{
    //TODOstub
    qWarning("stub not implemented: SocialNetworkTimeline::handleRemoveKeyFinished(bool success)");
}
void SocialNetworkTimeline::handleCommitFinished(bool success)
{
    //TODOstub
    qWarning("stub not implemented: SocialNetworkTimeline::handleCommitFinished(bool success)");
}
void SocialNetworkTimeline::handleReadKeyFinished(bool success, QString key, QString revision, QString data)
{
    //TODOstub
    qWarning("stub not implemented: SocialNetworkTimeline::handleReadKeyFinished(bool success, QString key, QString revision, QString data)");
}

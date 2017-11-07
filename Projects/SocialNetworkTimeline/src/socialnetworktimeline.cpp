#include "socialnetworktimeline.h"

#include <QCoreApplication>

#include "socialnetworktimelinerequestresponsecontracts.h"
#include "stupidkeyvaluecontenttracker.h"

using namespace SocialNetworkTimelineRequestResponseContracts;

//TODOreq: "last modified timestamps of _IMPORTED_ files" needs to live somewhere!! -- see stupidkeyvaluecontenttracker.cpp comments for more thoughts on this (because I'm not sure yet at which layer it belongs)
SocialNetworkTimeline::SocialNetworkTimeline(QObject *parent)
    : QObject(parent)
    , m_Contracts(new Contracts(this))
{
    StupidKeyValueContentTracker *keyValueStore_WithHistory = new StupidKeyValueContentTracker(this);
    StupidKeyValueContentTracker::establishConnectionsToAndFromBackendAndUi<SocialNetworkTimeline>(keyValueStore_WithHistory, this);

    QCoreApplication::setOrganizationName("SocialNetworkTimelineOrganization");
    QCoreApplication::setOrganizationDomain("SocialNetworkTimelineDomain");
    QCoreApplication::setApplicationName("SocialNetworkTimeline");
    QSettings::setDefaultFormat(QSettings::IniFormat);
}
SocialNetworkTimeline::~SocialNetworkTimeline()
{
    //non-inline destructor for m_Contracts forward's declaration
}
void SocialNetworkTimeline::initializeSocialNetworkTimeline()
{
    InitializeSocialNetworkTimelineScopedResponder scopedResponder(m_Contracts->initializeSocialNetworkTimeline());
    emit initializeRequested(); //initialize StupidKeyValueContentTracker requested
    scopedResponder.deferResponding();
}
void SocialNetworkTimeline::appendJsonObjectToSocialNetworkTimeline(const QJsonObject &data)
{
    AppendJsonObjectToSocialNetworkTimelineScopedResponder scopedResponder(m_Contracts->appendJsonObjectToSocialNetworkTimeline());
    //should this be 'insert' instead of 'append'? Can we add things with timestamps that happened years ago? should the desired timestamp be an arg TODOreq?
    emit addRequested(keyForJsonData(data), data);
    scopedResponder.deferResponding();
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
    //handle StupidKeyValueContentTracker initialization finished
    InitializeSocialNetworkTimelineScopedResponder scopedResponder(m_Contracts->initializeSocialNetworkTimeline());
    if(success)
        handleO("StupidKeyValueContentTracker initialization finished successfully");
    else
        handleE("StupidKeyValueContentTracker initialization finished unsuccessfully");

    scopedResponder.response()->setSuccess(success); //sexex daishy chainin
    return /*emit*/;
}
void SocialNetworkTimeline::handleAddFinished(bool success)
{
    AppendJsonObjectToSocialNetworkTimelineScopedResponder scopedResponder(m_Contracts->appendJsonObjectToSocialNetworkTimeline());
    if(!success)
    {
        emit e("StupidKeyValueContentTracker failed to add data." /*TODOwhatever: show data here, handleAddFinished would return our reference to it back to us, ideally*/);
        return /*emit*/;
    }
    emit commitRequested("eat shit 69420 " + QString::number(QDateTime::currentMSecsSinceEpoch()));
    scopedResponder.deferResponding();
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
    AppendJsonObjectToSocialNetworkTimelineScopedResponder scopedResponder(m_Contracts->appendJsonObjectToSocialNetworkTimeline());
    if(success)
        emit o("StupidKeyValueContentTracker committed successfully");
    else
        emit e("StupidKeyValueContentTracker committed unsuccessfully");
    scopedResponder.response()->setSuccess(success);
    return /*emit*/;
}
void SocialNetworkTimeline::handleReadKeyFinished(bool success, QString key, QString revision, QString data)
{
    //TODOstub
    qWarning("stub not implemented: SocialNetworkTimeline::handleReadKeyFinished(bool success, QString key, QString revision, QString data)");
}

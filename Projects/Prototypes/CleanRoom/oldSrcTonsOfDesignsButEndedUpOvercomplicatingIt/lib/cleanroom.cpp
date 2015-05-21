#include "cleanroom.h"

#include <QMetaObject>

#include "icleanroomsessionprovider.h"
#include "cleanroomdb.h"
#include <cleanroomdoc.h>

CleanRoom::CleanRoom(QObject *parent)
    : QObject(parent)
    , m_Db(new CleanRoomDb(this))
{
    //API call connections: initial front page view get and subscribe
    connect(this, SIGNAL(queryFrontPageViewRequested(ICleanRoomSession*)), m_Db, SLOT(queryFrontPageView(ICleanRoomSession*))); //right now my couchbase db has the cache/"subscription" logic in it, but really it should live somewhere else?
    connect(m_Db, SIGNAL(finishedQueryingFrontPageView(ICleanRoomSession*, QList<CleanRoomDoc>)), this, SLOT(handleDbFinishedQueryingFrontPageView(ICleanRoomSession*, QList<CleanRoomDoc>)));

    //API call connections: doc submit
    connect(this, SIGNAL(submitCleanRoomDocumentToDbBeginRequested(qint64,CleanRoomUsername,CleanRoomLicenseShorthandIdentifier,QByteArray)), m_Db, SLOT(submitCleanRoomDocumentBegin(qint64,CleanRoomUsername,CleanRoomLicenseShorthandIdentifier,QByteArray)));
    connect(m_Db, SIGNAL(finishedSubmittingCleanRoomDocument(bool,QString)), this, SLOT(handleDbFinishedSubmittingCleanRoomDocument(bool,QString)));
}
void CleanRoom::registerSessionProvider(ICleanRoomSessionProvider *cleanRoomSessionProvider)
{
    cleanRoomSessionProvider->setMetaMethodToCallWhenNewSession(this,
metaObject()->method(metaObject()->indexOfMethod(
#CleanRoomHandleNewSession
    )));
}
void CleanRoom::connectToAndFromCleanRoomSession(ICleanRoomSession *cleanRoomSession)
{
    QObject *cleanRoomSessionAsQObject = static_cast<QObject*>(cleanRoomSession); //this static cast will fail at compile time if any ICleanRoomSession implementers do not also implement QObject
    connect(cleanRoomSessionAsQObject, SIGNAL(getAndSubscribeToFrontPageRequested(ICleanRoomSession*)), this, SLOT(getAndSubscribeToFrontPageBegin(ICleanRoomSession*)));
    connect(this, SIGNAL(frontPageUpdated(QList<CleanRoomDoc>)), cleanRoomSessionAsQObject, SLOT(handleFrontPageUpdated(QList<CleanRoomDoc>)));
}
#if 0
void CleanRoom::showDefaultStaticallyLinkedPlugin(CleanRoomSession cleanRoomSession)
{
    //our default statically linked plugin is the "cleanroom unfiltered [app] view of the [couchbase] map/view"
}
#endif
void CleanRoom::submitCleanRoomDocumentBegin(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data)
{
    emit submitCleanRoomDocumentToDbBeginRequested(timestamp, username, licenseShorthandIdentifier, data);
}
void CleanRoom::handleDbFinishedSubmittingCleanRoomDocument(bool success, const QString &documentKey_aka_Sha1)
{
    //OT: since the document key is the sha1, the submitter would have been already able to calculate it. however, returning SOME kind of identifier as to what document just finished beign submitted, is helpful
    emit finishedSubmittingCleanRoomDocument(success, documentKey_aka_Sha1); //my business (this class) does basically nothing in this simple af app, but it's still a good idea to have one for future proofing
}
void CleanRoom::getAndSubscribeToFrontPageBegin(ICleanRoomSession *sessionToGetAndSubscribeToFrontPage)
{
    emit queryFrontPageViewRequested(sessionToGetAndSubscribeToFrontPage);
}
void CleanRoom::handleDbFinishedQueryingFrontPageView(ICleanRoomSession *cleanRoomSession, QList<CleanRoomDoc> frontPageDocs)
{
    cleanRoomSession->handleFinishedQueryingFrontPageView(frontPageDocs);
}
void CleanRoom::CleanRoomHandleNewSession(ICleanRoomSession *cleanRoomSession)
{
#if 0
    if(!pluginsLoaded().isEmpty())
        askPluginsIfTheyWantToMakeAnyPublicationOfThemselvesToTheUser(cleanRoomSession);
    else
        showDefaultStaticallyLinkedPlugin(cleanRoomSession); //CleanRoomWWW initial frontpage view is but one implementation of a defaultStaticallyLinkedPlugin. this is the default/unfiltered 'everything' couchbase-view

    //connect(this, SIGNAL(finishedSubmittingCleanRoomDocument(bool,QString)), sessionProvider, SLOT(handleFinishedSubmittingCleanRoomDocument(bool,QString)));
    //cleanRoomSession.metaMethodToCallWhenFinishedSubmittingCleanRoomDocument()
    //need requests/responses. the session requests an api call from the business, and is responded to
#endif
    connectToAndFromCleanRoomSession(cleanRoomSession);
    cleanRoomSession->getAndSubscribeToFrontPage();
}


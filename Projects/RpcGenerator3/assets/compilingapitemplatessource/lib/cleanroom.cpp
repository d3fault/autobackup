#include "cleanroom.h"

#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>

#include "cleanroomdb.h"

//TODOmb: shouldn't the db be responsible for providing the timestamp it was accepted on the timeline db?
//TODOreq: need a map of short to long license identifiers (and why stop there, might as well show the license itself ofc if/when requested) to make them valid
//TODOoptional: AutoDb should come later, after AutoDrilldown. Another phrase for AutoDb is "object relation[al?] mapping (had:management)" or someshit. AutoDrilldown (AutoGui) sounds much more interesting/useful (but together, they sound an order of magnitude more useful than either one of them alone (mfw this very second (calm af. I feel like I've inherited the pure virtual interface of the world. Itself being an aggregation of trillions of other pure virtual interfaces)))
//I totally should have made a "read and insert template markers based on known words" app instead of my current hardcoded apitemplates
CleanRoom::CleanRoom(QObject *parent)
    : ICleanRoom(parent)
    , m_Db(new CleanRoomDb(this))
{
    connect(m_Db, &CleanRoomDb::getLatestCleanRoomDocsFinished, this, &CleanRoom::handleGetLatestCleanRoomDocsFinished);

    //"NOT RPC GENERATED CODE" BEGIN
    //this code is just to make the compiling example actually have something to do
    for(int i = 0; i < NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE; ++i)
    {
        CleanRoomDoc cleanRoomDoc(QDateTime::currentDateTime(), "d3fault_" + QString::number(i), QByteArray("poop lol" + QString::number(i).toUtf8()), "DPL3+");
        m_Db->postCleanRoomDoc(cleanRoomDoc.toJson());
    }
    //"NOT RPC GENERATED CODE" END
}
void CleanRoom::getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0) //lol this is so hacked. "get front page default view" is pretending to be "postCleanRoomDoc
{
    //eventually the business gathers the frontPageDocs and calls finishedGettingFrontPageDefaultView, but we synthesize/fake that here
    //this is a good starting point for AutoDrilldown

    //"NOT RPC GENERATED CODE" BEGIN
    Q_UNUSED(someArg0)
    //this code is just to make the compiling example actually have something to do
    m_Db->getLatestCleanRoomDocs(request, NUM_CLEAN_ROOM_DOCS_ON_FRONT_PAGE);
    //"NOT RPC GENERATED CODE" END
}
//"NOT RPC GENERATED CODE" BEGIN
void CleanRoom::handleGetLatestCleanRoomDocsFinished(ICleanRoomFrontPageDefaultViewRequest *requestWeWereMerelyForwardingAroundInDbLikeUserDataAkaCookiePointer, bool dbError, bool postCleanRoomDocSuccess_aka_LcbOp, QStringList frontPageDocs)
{
    //OT: for such a "bulk" get to the db, each key should have it's own lcbOp bool (I'm unsure how couchbase handles this)
    requestWeWereMerelyForwardingAroundInDbLikeUserDataAkaCookiePointer->respond(frontPageDocs); //TODOreq: respond needs dbError and lcbOp (but maybe provide overloads for both successful? NAH)
}
//"NOT RPC GENERATED CODE" END

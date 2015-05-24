#include "cleanroom.h"

#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>

#include "cleanroomdb.h"

CleanRoom::CleanRoom(QObject *parent)
    : ICleanRoom(parent)
    , m_Db(new CleanRoomDb(this))
{
    connect(m_Db, &CleanRoomDb::postCleanRoomDocFinished, this, &CleanRoom::handlePostCleanRoomDocFinished);
}
void CleanRoom::getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0) //lol this is so hacked. "get front page default view" is pretending to be "postCleanRoomDoc
{
    //eventually the business gathers the frontPageDocs and calls finishedGettingFrontPageDefaultView, but we synthesize/fake that here
    //this is a good starting point for AutoDrilldown
    CleanRoomDoc doc;
    doc.Timestamp = QDateTime::currentDateTime(); //TODOmb: shouldn't the db be responsible for providing the timestamp it was accepted on the timeline db?
    doc.Username = "d3fault"; //TODOreq
    QVariant someArg0Variant(someArg0);
    //doc.Data = someArg0Variant.toJsonDocument().toJson(QJsonDocument::Compact); //AutoDb should come later, after AutoDrilldown
    doc.Data = someArg0Variant.toByteArray();
    doc.LicenseIdentifier = "DPL3+"; //TODOreq: need a map of short to long license identifiers (and why stop there, might as well show the license itself ofc if/when requested) to make them valid

    QByteArray docAsJson = doc.toJson();
    m_Db->postCleanRoomDoc(request, docAsJson);
}
void CleanRoom::handlePostCleanRoomDocFinished(ICleanRoomFrontPageDefaultViewRequest *request, bool dbError, bool postCleanRoomDocSuccess_aka_LcbOp, QByteArray dbKey_aka_Sha1)
{
    QString sha1Hex = dbKey_aka_Sha1;
    request->respond(QStringList() << sha1Hex); //TODOreq: respond needs dbError and lcbOp (but maybe provide overloads for both successful? NAH)
}

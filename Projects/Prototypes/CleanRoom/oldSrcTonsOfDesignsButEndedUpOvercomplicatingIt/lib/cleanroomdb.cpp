#include "cleanroomdb.h"

#include <QMap> //temp while QSettings
#include <QMapIterator>

#include "cleanroomdoc.h"

CleanRoomDb::CleanRoomDb(QObject *parent)
    : QObject(parent)
{ }
QString CleanRoomDb::docKeyFromDocBodyHash(const QByteArray &bodyHash)
{
    //QString ret("data_" + QString(bodyHash));
    //return ret;
    return QString(bodyHash);
}
void CleanRoomDb::queryFrontPageView(ICleanRoomSession *cleanRoomSession)
{
    //come to think of it, get and subscribe to couchbase VIEW is kind of new (but really nothing special)

    //a legit couchbase view is much more scalable than this inefficient prototype hackery using qsettings
    QStringList docKeys = m_Settings.childKeys();
    QMap<qint64, CleanRoomDoc> allDocsSortedByTimestamp>;
    Q_FOREACH(QString currentDocKey, docKeys)
    {
        CleanRoomDoc currentDoc = CleanRoomDoc::fromJson(m_Settings.value(currentDocKey).toByteArray());
        allDocsSortedByTimestamp.insert(currentDoc.timestamp(), currentDoc);
    }
    QList<CleanRoomDoc> frontPageDocs;
    QMapIterator<qint64, CleanRoomDoc> it(allDocsSortedByTimestamp);
    while(it.hasNext() && frontPageDocs.size() < CLEANROOM_QUERY_NUM_ITEMS)
    {
        it.next();
        frontPageDocs.append(it.value());
    }

    emit finishedQueryingFrontPageView(cleanRoomSession, frontPageDocs);
}
void CleanRoomDb::submitCleanRoomDocumentBegin(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data)
{
    CleanRoomDoc newCleanRoomDoc(timestamp, username, licenseShorthandIdentifier, data);
    QByteArray newCleanRoomDocJson = newCleanRoomDoc.toJson();
    QByteArray hexHashIdentifierForDocKey = newCleanRoomDoc.hexHashIdentifierForDocKey();
    QString docKey_aka_sha1 = docKeyFromDocBodyHash(hexHashIdentifierForDocKey);
    m_Settings.setValue(docKey_aka_sha1, newCleanRoomDocJson);
    //TODOoptimization: Instead of QSettings, use LCB_ADD, durability poll, then finally:
    emit finishedSubmittingCleanRoomDocument(true, docKey_aka_sha1); //TODOoptimization: I guess "data_" prefix is implied, so unecessary? keeping it to KISS. I supposed actually that the "data_" prefix is unecessary in the entire application! Designs, everywhere.
}

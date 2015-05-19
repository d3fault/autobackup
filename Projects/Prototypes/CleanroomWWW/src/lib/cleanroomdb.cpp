#include "cleanroomdb.h"

#include <QSettings>

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
void CleanRoomDb::submitCleanRoomDocumentBegin(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data)
{
    QSettings settings;
    CleanRoomDoc newCleanRoomDoc(timestamp, username, licenseShorthandIdentifier, data);
    QByteArray newCleanRoomDocJson = newCleanRoomDoc.toJson();
    QByteArray hexHashIdentifierForDocKey = newCleanRoomDoc.hexHashIdentifierForDocKey();
    QString docKey_aka_sha1 = docKeyFromDocBodyHash(hexHashIdentifierForDocKey);
    settings.setValue(docKey_aka_sha1, newCleanRoomDocJson);
    //TODOoptimization: Instead of QSettings, use LCB_ADD, durability poll, then finally:
    emit finishedSubmittingCleanRoomDocument(true, docKey_aka_sha1); //TODOoptimization: I guess "data_" prefix is implied, so unecessary? keeping it to KISS. I supposed actually that the "data_" prefix is unecessary in the entire application! Designs, everywhere.
}

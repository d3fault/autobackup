#ifndef CLEANROOMDB_H
#define CLEANROOMDB_H

#include <QObject>

#include "cleanroomcommon.h"

class CleanRoomDb : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomDb(QObject *parent = 0);
private:
    QByteArray cleanroomMandatoryFieldsToMinifiedJson(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data);
    static QString docKeyFromDocBodyHash(const QByteArray &bodyHash);
signals:
    void finishedSubmittingCleanRoomDocument(bool success, const QString &documentKey_aka_Sha1);
public slots:
    void submitCleanRoomDocumentBegin(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data);
};

#endif // CLEANROOMDB_H

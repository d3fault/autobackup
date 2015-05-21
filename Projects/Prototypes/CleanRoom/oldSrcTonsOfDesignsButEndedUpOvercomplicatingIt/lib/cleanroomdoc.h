#ifndef CLEANROOMDOC_H
#define CLEANROOMDOC_H

#include "cleanroomcommon.h"

class CleanRoomDoc
{
public:
    CleanRoomDoc(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data);
    QByteArray toJson() const;
    QByteArray hexHashIdentifierForDocKey() const;
private:
    qint64 m_Timestamp;
    const CleanRoomUsername &m_Username;
    const CleanRoomLicenseShorthandIdentifier &m_LicenseShorthandIdentifier;
    const QByteArray &m_Data;
};

#endif // CLEANROOMDOC_H

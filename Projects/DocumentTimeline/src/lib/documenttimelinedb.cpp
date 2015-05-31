#include "documenttimelinedb.h"

bool DocumentTimelineDb::addDocToDb(QByteArray docToAddToDb)
{
    m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->setValue(QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Sha1).toHex(), jsonString);

    request->respond(true, b64sha1OfRegistrationDetails_ForUserToReciteOnVid);
}

#include "cleanroomdoc.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>

CleanRoomDoc::CleanRoomDoc(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data)
    : m_Timestamp(timestamp)
    , m_Username(username)
    , m_LicenseShorthandIdentifier(licenseShorthandIdentifier)
    , m_Data(data)
{ }
QByteArray CleanRoomDoc::toJson() const
{
    //QJsonObject minifiedJsonCreator;
    //QByteArray ret("{\"timestamp\":\"" + QString::number(timestamp))
    QJsonObject minifiedJsonCreator;
    minifiedJsonCreator.insert(CLEANROOM_DB_TIMESTAMP_JSON_KEY, QString::number(m_Timestamp));
    minifiedJsonCreator.insert(CLEANROOM_DB_USERNAME_JSON_KEY, m_Username);
    minifiedJsonCreator.insert(CLEANROOM_DB_LICENSE_JSON_KEY, m_LicenseShorthandIdentifier);
    minifiedJsonCreator.insert(CLEANROOM_DB_DATA_JSON_KEY, QString(m_Data.toBase64()));
    QJsonDocument jsonDoc(minifiedJsonCreator);
    QByteArray ret = jsonDoc.toJson(QJsonDocument::Compact);
    return ret;
}
QByteArray CleanRoomDoc::hexHashIdentifierForDocKey() const
{
    //TODOoptimization: toJson result could be cached in tons of different ways
    QByteArray documentBody = toJson();
    QByteArray ret = QCryptographicHash::hash(documentBody, QCryptographicHash::Sha1).toHex();
    return ret;
}
//hexSha1OfMinifiedJson(newCleanRoomDocJson)

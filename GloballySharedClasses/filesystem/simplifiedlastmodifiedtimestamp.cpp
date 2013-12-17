#include "simplifiedlastmodifiedtimestamp.h"

const QString SimplifiedLastModifiedTimestamp::m_Colon = ":";
const QString SimplifiedLastModifiedTimestamp::m_ColonEscaped = "\\:";
const QString SimplifiedLastModifiedTimestamp::m_IllegalFilePath = "......./////////......\\\\\\\\BUTTS;;;}[0+^%12!@#$%^&*()_+=-`~-/oneofthesehastobe<>,.? ]";

SimplifiedLastModifiedTimestamp::SimplifiedLastModifiedTimestamp(const QString &colonSeparatedLastModifiedTimestampLine)
{
    QString escapedColonsReplaced = colonSeparatedLastModifiedTimestampLine;
    escapedColonsReplaced = escapedColonsReplaced.replace(m_ColonEscaped, m_IllegalFilePath);
    QStringList splitAtColons = escapedColonsReplaced.split(m_Colon, QString::SkipEmptyParts);
    if(splitAtColons.size() != 2)
        return;
    bool convertOk = false;
    qint64 lastModifiedTimestamp = splitAtColons.at(1).toLongLong(&convertOk);
    if(!convertOk)
        return;

    //empty filePath() is how we identify errors

    m_LastModifiedTimestamp = QDateTime::fromMSecsSinceEpoch(lastModifiedTimestamp*1000);
    m_FilePath = splitAtColons.at(0);
    m_FilePath = m_FilePath.replace(m_IllegalFilePath, m_Colon);
    m_IsDirectory = m_FilePath.endsWith("/") ? true : false;
}
const QString &SimplifiedLastModifiedTimestamp::filePath()
{
    return m_FilePath;
}
bool SimplifiedLastModifiedTimestamp::isDirectory()
{
    return m_IsDirectory;
}
const QDateTime &SimplifiedLastModifiedTimestamp::lastModifiedTimestamp()
{
    return m_LastModifiedTimestamp;
}
QString SimplifiedLastModifiedTimestamp::toColonSeparatedLineOfText()
{
    QString ret = m_FilePath + m_Colon + QString::number(m_LastModifiedTimestamp.toMSecsSinceEpoch()/1000);
    return ret;
}

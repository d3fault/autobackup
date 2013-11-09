#include "easytreehashitem.h"

const QString EasyTreeHashItem::m_Colon = ":";
const QString EasyTreeHashItem::m_ColonEscaped = "\\:";
QString EasyTreeHashItem::m_TempStringToInsertForEscapedColons = "ASDF";

EasyTreeHashItem::EasyTreeHashItem(QObject *parent)
    : QObject(parent), m_AbsoluteReplacementFilePath("")
{ }
EasyTreeHashItem *EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(const QString &lineOfText, bool lineOfTextSuppliesAbsoluteReplacementFilePath, bool lineHasCreationDate, QObject *parent)
{
    //TODOrobust: Input sanitization. For example, splitAtColons.takeFirst() requires that > 0 entries exist
    EasyTreeHashItem *ret = new EasyTreeHashItem(parent);

    ret->setHasCreationDateTime(lineHasCreationDate);

    bool usingTempStringForEscapedColons = false;
    QString lineOfTextModifiable = lineOfText;
    if(lineOfText.contains(m_ColonEscaped))
    {
        //insert a random string into the shit just to differentiate it from the other colons when splitting
        usingTempStringForEscapedColons = true;
        EasyTreeHashItem::m_TempStringToInsertForEscapedColons = generateStringNotInString_ButMakeSureTheOutputDoesntContainAcolon(lineOfTextModifiable);
        lineOfTextModifiable.replace(m_ColonEscaped, m_TempStringToInsertForEscapedColons);
    }

    QStringList splitAtColons = lineOfTextModifiable.split(m_Colon, QString::SkipEmptyParts);

    if(usingTempStringForEscapedColons)
    {
        //now put the colons back in the filename portion (or any other portion that can allow a colon (none atm (could just loop em all (will fuck it))))
        int splitAtColonsLength = splitAtColons.length();
        for(int i = 0; i < splitAtColonsLength; ++i)
        {
            QString currentSegment = splitAtColons.at(i);
            currentSegment.replace(m_TempStringToInsertForEscapedColons, m_Colon);
            splitAtColons.replace(i, currentSegment); //would be unnecessary with pointers...
        }
    }

    if(lineOfTextSuppliesAbsoluteReplacementFilePath)
    {
        QString temp = splitAtColons.takeFirst();
        ret->setAbsoluteReplacementFilePath(temp); //too many times I've seen when you pass a "take" into a "set" that the implicit sharing causes it to get deleted before being set.... or something
    }

    ret->setIsDirectory(splitAtColons.at(0).endsWith("/"));
    ret->setRelativeFilePath(splitAtColons.at(0));

    if(ret->isDirectory())
    {
        ret->setFileSize(static_cast<qint64>(-1));
    }
    else
    {
        QByteArray fileSizeQint64ReadProperlyArray(splitAtColons.at(1).toLatin1());
        QBuffer fileSizeQint64ReadProperlyBuffer(&fileSizeQint64ReadProperlyArray);
        fileSizeQint64ReadProperlyBuffer.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream fileSizeQint64ReadProperlyTextStream(&fileSizeQint64ReadProperlyBuffer);
        qint64 fileSizeAsQint64;
        fileSizeQint64ReadProperlyTextStream >> fileSizeAsQint64;
        ret->setFileSize(fileSizeAsQint64);
    }

    int dynamicIndexForFilesAndDirectories = 1;
    if(!ret->isDirectory())
    {
        ++dynamicIndexForFilesAndDirectories;
    }

    if(lineHasCreationDate)
    {
        QByteArray creationDateTimeQint64ReadProperlyArray(splitAtColons.at(dynamicIndexForFilesAndDirectories).toLatin1()); //for directories creation date is index 1, for files it's 2
        QBuffer creationDateTimeQint64ReadProperlyBuffer(&creationDateTimeQint64ReadProperlyArray);
        creationDateTimeQint64ReadProperlyBuffer.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream creationDateTimeQint64ReadProperlyTextStream(&creationDateTimeQint64ReadProperlyBuffer);
        qint64 creationDateTimeAsQint64;
        creationDateTimeQint64ReadProperlyTextStream >> creationDateTimeAsQint64;

        ret->setCreationDateTime(QDateTime::fromMSecsSinceEpoch(creationDateTimeAsQint64*1000));

        ++dynamicIndexForFilesAndDirectories;
    }

    QByteArray lastModifiedDateTimeQint64ReadProperlyArray(splitAtColons.at(dynamicIndexForFilesAndDirectories).toLatin1());
    QBuffer lastModifiedDateTimeQint64ReadProperlyBuffer(&lastModifiedDateTimeQint64ReadProperlyArray);
    lastModifiedDateTimeQint64ReadProperlyBuffer.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream lastModifiedDateTimeQint64ReadProperlyTextStream(&lastModifiedDateTimeQint64ReadProperlyBuffer);
    qint64 lastModifiedDateTimeAsQint64;
    lastModifiedDateTimeQint64ReadProperlyTextStream >> lastModifiedDateTimeAsQint64;

    ret->setLastModifiedDateTime(QDateTime::fromMSecsSinceEpoch(lastModifiedDateTimeAsQint64*1000));


    if(!ret->isDirectory()) //directories definitely don't have a hash
    {
        if(splitAtColons.length() > 4) //had == 5, but > 4 is more dynamic in case i add fields in the future...
        {
            ret->setHasHash(true);
            ++dynamicIndexForFilesAndDirectories; //it's merely an optimization to have this right here, but it needs to go outside of the if(!isDirectory()) check if i ever add more fields (i think, but could be wrong actually)
            ret->setHash(QByteArray::fromHex(splitAtColons.at(dynamicIndexForFilesAndDirectories).toLatin1()));
        }
    }
    return ret;
}
EasyTreeHashItem *EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(const QString &lineOfText, bool lineOfTextSuppliesAbsoluteReplacementFilePath, QObject *parent)
{
    return newEasyTreeHashItemFromLineOfText(lineOfText, lineOfTextSuppliesAbsoluteReplacementFilePath, true, parent);
}
QString EasyTreeHashItem::relativeFilePath()
{
    return m_RelativeFilePath;
}
bool EasyTreeHashItem::isDirectory()
{
    return m_IsDirectory;
}
qint64 EasyTreeHashItem::fileSize()
{
    return m_FileSize;
}
bool EasyTreeHashItem::hasCreationDateTime()
{
    return m_HasCreationDateTime;
}
QDateTime EasyTreeHashItem::creationDateTime()
{
    return m_CreationDateTime;
}
QDateTime EasyTreeHashItem::lastModifiedDateTime()
{
    return m_LastModifiedDateTime;
}
bool EasyTreeHashItem::hasHash()
{
    return m_HasHash;
}
QByteArray EasyTreeHashItem::hash()
{
    return m_Hash;
}
//currently only being used by files right now (in easytreehasher), but would probably work with directories
QString EasyTreeHashItem::toColonSeparatedLineOfText()
{
    QString relative = relativeFilePath();
    QString ret = relative.replace(m_Colon, m_ColonEscaped);
    if(!isDirectory())
    {
        ret += m_Colon + QString::number(fileSize());
    }
    else
    {
        ret += "/"; //trailing slash for directories
    }
    ret += m_Colon + QString::number(creationDateTime().toMSecsSinceEpoch()/1000) + m_Colon + QString::number(lastModifiedDateTime().toMSecsSinceEpoch()/1000);
    if(hasHash())
    {
        ret += (m_Colon + hash().toHex());
    }
    return ret;
}
QString EasyTreeHashItem::absoluteReplacementFilePath()
{
    return m_AbsoluteReplacementFilePath;
}
void EasyTreeHashItem::setRelativeFilePath(const QString &relativeFilePath)
{
    m_RelativeFilePath = relativeFilePath;
}
void EasyTreeHashItem::setIsDirectory(bool isDirectory)
{
    m_IsDirectory = isDirectory;
}
void EasyTreeHashItem::setFileSize(qint64 fileSize)
{
    m_FileSize = fileSize;
}
void EasyTreeHashItem::setHasCreationDateTime(bool hasCreationDateTime)
{
    m_HasCreationDateTime = hasCreationDateTime;
}
void EasyTreeHashItem::setCreationDateTime(const QDateTime &creationDateTime)
{
    m_CreationDateTime = creationDateTime;
}
void EasyTreeHashItem::setLastModifiedDateTime(const QDateTime &lastModifiedDateTime)
{
    m_LastModifiedDateTime = lastModifiedDateTime;
}
void EasyTreeHashItem::setHasHash(bool hasHash)
{
    m_HasHash = hasHash;
}
void EasyTreeHashItem::setHash(const QByteArray &hash)
{
    m_Hash = hash;
}
void EasyTreeHashItem::setAbsoluteReplacementFilePath(const QString &absoluteReplacementFilePath)
{
    m_AbsoluteReplacementFilePath = absoluteReplacementFilePath;
}
QString EasyTreeHashItem::generateStringNotInString_ButMakeSureTheOutputDoesntContainAcolon(const QString &inputStringToCheckGeneratedOutputAgainst)
{
    Q_UNUSED(inputStringToCheckGeneratedOutputAgainst)
    //TODOreq

    return QString("DSOU23048^(#&*%&*(#((#__@*@**----262634847////slashesAreIllegalInFilenamesWoo///29345345$$%802308SDFLKJFKJLA*(#&Q$SD");
}

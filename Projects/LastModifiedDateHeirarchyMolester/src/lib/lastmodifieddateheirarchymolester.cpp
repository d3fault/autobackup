#include "lastmodifieddateheirarchymolester.h"

LastModifiedDateHeirarchyMolester::LastModifiedDateHeirarchyMolester(QObject *parent) :
    QObject(parent)
{
    connect(&m_FileModificationDateChanger, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void LastModifiedDateHeirarchyMolester::molestLastModifiedDateHeirarchy(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &easyTreeFilePath, bool easyTreeLinesHaveCreationDate, bool xmlFormattedTreeCommandOutputActually)
{
    if(easyTreeLinesHaveCreationDate && xmlFormattedTreeCommandOutputActually)
    {
        emit d("You can't have a creation date in an XML formatted tree command output, so uhh just stopping to play it safe because I dunno wtf you wan't");
        return;
    }
    QString directoryHeirarchyCorrespingToEasyTreeFileWithSlashAppended = directoryHeirarchyCorrespingToEasyTreeFile;
    if(!directoryHeirarchyCorrespingToEasyTreeFileWithSlashAppended.endsWith("/"))
    {
        directoryHeirarchyCorrespingToEasyTreeFileWithSlashAppended.append("/");
    }
    QFileInfo dirToMolestFileInfo(directoryHeirarchyCorrespingToEasyTreeFileWithSlashAppended);
    if(!dirToMolestFileInfo.exists())
    {
        emit d("Directory to Molest does Not Exist");
        return;
    }
    if(!dirToMolestFileInfo.isDir())
    {
        emit d("Directory to Molest is not a directory");
        return;
    }
    QFile easyTreeFile(easyTreeFilePath); //TODOreq: doesn't belong here in this project, but I need to make a mental note to make sure that easy tree file doesn't "stat" itself
    if(!easyTreeFile.exists())
    {
        emit d("Easy Tree File Does Not Exist");
        return;
    }
    if(!easyTreeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("Failed to open Easy Tree File for reading");
        return;
    }
    emit d("Beginning Molestation...");
    if(xmlFormattedTreeCommandOutputActually)
    {
        QXmlStreamReader treeXmlStream(&easyTreeFile);
        QStringRef currentElement;
        QDir currentDir(directoryHeirarchyCorrespingToEasyTreeFileWithSlashAppended);
        QStringRef currentNameAttribute;
        QDateTime currentDateTimeAttribute;
        quint32 currentDepthHack = 0; //hack so we don't "cd up" when we encounter the end element corresponding to the "directory name='.'" start element -_-
        while(!treeXmlStream.atEnd())
        {
            QXmlStreamReader::TokenType tokenType = treeXmlStream.readNext();
            if(treeXmlStream.hasError())
            {
                emit d("Error reading XML from tree command (row: " + QString::number(treeXmlStream.lineNumber()) + ",col: " + QString::number(treeXmlStream.columnNumber()) + "): " + treeXmlStream.errorString());
                return;
            }
            if(tokenType == QXmlStreamReader::StartElement)
            {
                currentElement = treeXmlStream.name();
                QXmlStreamAttributes attributes = treeXmlStream.attributes();
                if(attributes.hasAttribute("name"))
                {
                    currentNameAttribute = attributes.value("name");
                }

                //file size would be gathered here, if we cared about it...

                if(attributes.hasAttribute("time"))
                {
                    QByteArray lastModifiedDateTimeQint64ReadProperlyArray(attributes.value("time").toLatin1());
                    QBuffer lastModifiedDateTimeQint64ReadProperlyBuffer(&lastModifiedDateTimeQint64ReadProperlyArray);
                    lastModifiedDateTimeQint64ReadProperlyBuffer.open(QIODevice::ReadOnly | QIODevice::Text);
                    QTextStream lastModifiedDateTimeQint64ReadProperlyTextStream(&lastModifiedDateTimeQint64ReadProperlyBuffer);
                    qint64 lastModifiedDateTimeAsQint64;
                    lastModifiedDateTimeQint64ReadProperlyTextStream >> lastModifiedDateTimeAsQint64;

                    currentDateTimeAttribute = QDateTime::fromMSecsSinceEpoch(lastModifiedDateTimeAsQint64*1000);
                }

                if(currentElement == "directory")
                {
                    //cd into it, then touch the dir's last modified date (the one we just cd'd into)
                    if(currentNameAttribute != ".") //don't touch or cd this special case
                    {
                        if(!currentDir.cd(currentNameAttribute.toString()))
                        {
                            emit d("failed to cd into a directory, so stopping");
                            return;
                        }
                        if(!m_FileModificationDateChanger.changeModificationDate(currentDir.absolutePath(), currentDateTimeAttribute))
                        {
                            emit d("failed to change modification date for an entry, so stopping");
                            return;
                        }
                        ++currentDepthHack;
                    }
                }
                else if(currentElement == "file")
                {
                    //touch
                    if(!m_FileModificationDateChanger.changeModificationDate(currentDir.absolutePath() + "/" + currentNameAttribute.toString(), currentDateTimeAttribute))
                    {
                        emit d("failed to change modification date for an entry, so stopping");
                        return;
                    }
                }
            }
            else if(tokenType == QXmlStreamReader::EndElement)
            {
                currentElement = treeXmlStream.name();
                if(currentElement == "directory")
                {
                    //cd up, UNLESS it's our last directory tag (the one at the beginning with file=".")
                    if(currentDepthHack > 0)
                    {
                        currentDir.cdUp();
                        --currentDepthHack;
                    }
                }
            }
        }
    }
    else
    {
        QTextStream easyTreeFileStream(&easyTreeFile);
        QString currentLine;
        while(!easyTreeFileStream.atEnd())
        {
            currentLine = easyTreeFileStream.readLine();
            if(!currentLine.trimmed().isEmpty())
            {
                EasyTreeHashItem *easyTreeItem = EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(currentLine, false, easyTreeLinesHaveCreationDate);

                //if age >= 18, touch them softly
                if(!m_FileModificationDateChanger.changeModificationDate(directoryHeirarchyCorrespingToEasyTreeFileWithSlashAppended + easyTreeItem->relativeFilePath(), easyTreeItem->lastModifiedDateTime()))
                {
                    emit d("failed to change modification date for an entry, so stopping");
                    delete easyTreeItem;
                    return;
                }
                delete easyTreeItem;
            }
        }
    }
    emit d("got to the end of the easy tree file, so if you didn't see any errors, molestation probably went smoothly");
}

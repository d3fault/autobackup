#include "lastmodifieddateheirarchymolester.h"

LastModifiedDateHeirarchyMolester::LastModifiedDateHeirarchyMolester(QObject *parent) :
    QObject(parent)
{
    connect(&m_FileModificationDateChanger, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void LastModifiedDateHeirarchyMolester::molestLastModifiedDateHeirarchy(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &easyTreeFilePath, bool easyTreeLinesHaveCreationDate)
{
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
    emit d("got to the end of the easy tree file, so if you didn't see any errors, molestation probably went smoothly");
}

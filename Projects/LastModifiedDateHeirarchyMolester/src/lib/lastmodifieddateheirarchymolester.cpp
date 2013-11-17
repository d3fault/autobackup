#include "lastmodifieddateheirarchymolester.h"

LastModifiedDateHeirarchyMolester::LastModifiedDateHeirarchyMolester(QObject *parent) :
    QObject(parent), m_FirstTimeDoingHackyOccuranceRateMerging(true)
{
    connect(&m_FileModificationDateChanger, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
bool LastModifiedDateHeirarchyMolester::loadFromXml(const QString &directoryHeirarchyCorrespingToXmlTreeFile, const QString &absoluteFilePathToXmlFormattedTreeFile)
{
    if(!doSharedInitBetweenXmlAndEasyTree(directoryHeirarchyCorrespingToXmlTreeFile, absoluteFilePathToXmlFormattedTreeFile))
    {
        emit d("failed to doSharedInitBetweenXmlAndEasyTree");
        return false;
    }

    emit d("Beginning trying to read XML timestamp file");

    QXmlStreamReader treeXmlStream(&m_TimestampFile);
    QStringRef currentElement;
    QDir currentDir(m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended);
    QStringRef currentNameAttribute;
    QDateTime currentDateTimeAttribute;
    quint32 currentDepthHack = 0; //hack so we don't "cd up" when we encounter the end element corresponding to the "directory name='.'" start element -_-
    while(!treeXmlStream.atEnd())
    {
        QXmlStreamReader::TokenType tokenType = treeXmlStream.readNext();
        if(treeXmlStream.hasError())
        {
            emit d("Error reading XML from tree command (row: " + QString::number(treeXmlStream.lineNumber()) + ",col: " + QString::number(treeXmlStream.columnNumber()) + "): " + treeXmlStream.errorString());
            return false;
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
                    QString dirAbsolutePath = currentDir.absolutePath() + "/" + currentNameAttribute.toString();
                    if(!QFile::exists(dirAbsolutePath)) //empty dirs not in git commits hack
                    {
                        if(!currentDir.mkdir(dirAbsolutePath))
                        {
                            emit d("failed to make a dir that was missed in git commit because it was empty (xml format tree file processing)");
                            return false;
                        }
                    }
                    if(!currentDir.cd(currentNameAttribute.toString()))
                    {
                        emit d("failed to cd into a directory, so stopping");
                        return false;
                    }
                    m_TimestampsByAbsoluteFilePathHash_Folders.insert(currentDir.absolutePath() + "/", currentDateTimeAttribute);
#if 0
                    if(!m_FileModificationDateChanger.changeModificationDate(currentDir.absolutePath(), currentDateTimeAttribute))
                    {
                        emit d("failed to change modification date for an entry, so stopping");
                        return;
                    }
#endif
                    ++currentDepthHack;
                }
            }
            else if(currentElement == "file")
            {
                m_TimestampsByAbsoluteFilePathHash_Files.insert(currentDir.absolutePath() + "/" + currentNameAttribute.toString(), currentDateTimeAttribute);
#if 0
                //touch
                if(!m_FileModificationDateChanger.changeModificationDate(currentDir.absolutePath() + "/" + currentNameAttribute.toString(), currentDateTimeAttribute))
                {
                    emit d("failed to change modification date for an entry, so stopping");
                    return;
                }
#endif
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
    m_TimestampFile.close(); //We didn't use this before the load/do refactor because it closes when it goes out of scope, but now we're re-using it etc and you shouldn't call setFileName when it's still/already open

    emit d("Finished reading XML timestamp file");

    return true;
}
bool LastModifiedDateHeirarchyMolester::loadFromEasyTreeFile(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &absoluteFilePathToEasyTreeFile, bool easyTreeLinesHaveCreationDate)
{
    doSharedInitBetweenXmlAndEasyTree(directoryHeirarchyCorrespingToEasyTreeFile, absoluteFilePathToEasyTreeFile);

    QTextStream easyTreeFileStream(&m_TimestampFile);
    QString currentLine;
    while(!easyTreeFileStream.atEnd())
    {
        currentLine = easyTreeFileStream.readLine();
        if(!currentLine.trimmed().isEmpty())
        {
            EasyTreeHashItem *easyTreeItem = EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(currentLine, false, easyTreeLinesHaveCreationDate);

            //if age >= 18, touch them softly
            if(easyTreeItem->isDirectory())
            {
                m_TimestampsByAbsoluteFilePathHash_Folders.insert(m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended + easyTreeItem->relativeFilePath(), easyTreeItem->lastModifiedDateTime());
            }
            else
            {
                m_TimestampsByAbsoluteFilePathHash_Files.insert(m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended + easyTreeItem->relativeFilePath(), easyTreeItem->lastModifiedDateTime());
            }
#if 0
            if(!m_FileModificationDateChanger.changeModificationDate(directoryHeirarchyCorrespingToEasyTreeFileWithSlashAppended + easyTreeItem->relativeFilePath(), easyTreeItem->lastModifiedDateTime()))
            {
                emit d("failed to change modification date for an entry, so stopping");
                delete easyTreeItem;
                return;
            }
#endif
            delete easyTreeItem;
        }
    }

    m_TimestampFile.close();
    return true;
}
bool LastModifiedDateHeirarchyMolester::molestUsingInternalTables()
{
    emit d("Beginning Molestation...");

    QHashIterator<QString, QDateTime> fileIterator(m_TimestampsByAbsoluteFilePathHash_Files);
    while(fileIterator.hasNext())
    {
        fileIterator.next();
        if(!m_FileModificationDateChanger.changeModificationDate(fileIterator.key(), fileIterator.value()))
        {
            emit d("failed to change timestamp of: " + fileIterator.key());
            return false;
        }
    }

    QHashIterator<QString, QDateTime> folderIterator(m_TimestampsByAbsoluteFilePathHash_Folders);
    while(folderIterator.hasNext())
    {
        folderIterator.next();
        if(!m_FileModificationDateChanger.changeModificationDate(folderIterator.key(), folderIterator.value()))
        {
            emit d("failed to change timestamp of: " + folderIterator.key());
            return false;
        }
    }


    //derp, i hope COW/implicit sharing makes this as easy as it appears :)
    m_OldTimestampsByAbsoluteFilePathHash_Files = m_TimestampsByAbsoluteFilePathHash_Files;
    m_OldTimestampsByAbsoluteFilePathHash_Folders = m_TimestampsByAbsoluteFilePathHash_Folders;
    m_FirstTimeDoingHackyOccuranceRateMerging = false;

    //random: i think that the implicit sharing'ness of Qt makes the 'old' tables take up virtually no memory when the "hacks" aren't used... so that's nice :)

    emit d("Finished Molesting");
    return true;
}
bool LastModifiedDateHeirarchyMolester::loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp(const QString &absoluteDirToSearchForFilesIn, QDateTime dateTimeToGiveThem)
{
    //no stack explode kthx
    m_DateTimeToGiveMissedAndManuallyFoundFiles = dateTimeToGiveThem;

    QDir currentDir(absoluteDirToSearchForFilesIn);
    if(!currentDir.exists())
    {
        emit d("dir to loadAnyMissedFilesByRecursivelyScanningDirectoriesAndGiveThemThisTimestamp does not exist");
        return false;
    }
    currentDir.setSorting(QDir::DirsFirst | QDir::Name);
    currentDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Hidden);

    QFileInfoList initialFileInfoList = currentDir.entryInfoList();

    if(!recursivelyCheckForMissedFilesAndFolders(initialFileInfoList))
    {
        emit d("failed while recursivelyCheckForMissedFilesAndFolders");
        return false;
    }

    return true;
}
void LastModifiedDateHeirarchyMolester::performHackyOccuranceRateMerging(quint32 occuranceRateThresholdToTriggerIgnoringOfTimestamp)
{
    if(m_FirstTimeDoingHackyOccuranceRateMerging)
    {
        //m_FirstTimeDoingHackyOccuranceRateMerging = false;
        return; //do nothing since we don't have an 'old table' to pull from even if occurance rate is met/exceeded, but set m_FirstTimeDoingHackyOccuranceRateMerging to false in the hack where molest makes current table the old table... TODOreq
    }

    //TODOreq: for analysis ONLY, we want to combine the files/folders hashes again -- this might make shit complicated later, but idk atm
    QHash<QString /*unixTimestamp*/,quint32 /*occuranceRate*/> timestampsOccurancesHash;

    //FILES
    QHashIterator<QString /*absoluteFilePath*/, QDateTime /*dateTime*/> fileIterator(m_TimestampsByAbsoluteFilePathHash_Files);
    while(fileIterator.hasNext())
    {
        fileIterator.next();
        QString unixTimestampString = QString::number(fileIterator.value().toMSecsSinceEpoch() / 1000);
        if(timestampsOccurancesHash.contains(unixTimestampString))
        {
            quint32 currentOccuranceCount = timestampsOccurancesHash.value(unixTimestampString);
            ++currentOccuranceCount;
            timestampsOccurancesHash.insert(unixTimestampString, currentOccuranceCount);
        }
        else
        {
            timestampsOccurancesHash.insert(unixTimestampString, 1);
        }
    }
    //FOLDERS
    QHashIterator<QString /*absoluteFilePath*/, QDateTime /*dateTime*/> folderIterator(m_TimestampsByAbsoluteFilePathHash_Folders);
    while(folderIterator.hasNext())
    {
        folderIterator.next();
        QString unixTimestampString = QString::number(folderIterator.value().toMSecsSinceEpoch() / 1000);
        if(timestampsOccurancesHash.contains(unixTimestampString))
        {
            quint32 currentOccuranceCount = timestampsOccurancesHash.value(unixTimestampString);
            ++currentOccuranceCount;
            timestampsOccurancesHash.insert(unixTimestampString, currentOccuranceCount);
        }
        else
        {
            timestampsOccurancesHash.insert(unixTimestampString, 1);
        }
    }

    //in my throw away analysis program that i'm stealing code from, i sorted the hash by value.. but in this one, i don't need to... just need to remember the key for any value that meets/exceeds our threshold

    QList<QDateTime> dateTimesThatMeetOrExceedThreshold;

    //Files/Folders combined in timestampsOccurancesHash at this point
    QHashIterator<QString /*unixTimestamp*/,quint32 /*occuranceRate*/> occurancesIterator(timestampsOccurancesHash);
    while(occurancesIterator.hasNext())
    {
        occurancesIterator.next();
        if(occurancesIterator.value() >= occuranceRateThresholdToTriggerIgnoringOfTimestamp)
        {
            //either save the key or 'pull from old table' now, idfk (who cares)
            bool convertOk = false;
            qint64 timestampAsQint64 = occurancesIterator.key().toLongLong(&convertOk);
            if(!convertOk)
            {
                emit d("failed to convert timestamp to qint64: " + occurancesIterator.key());
                return; //TODOreq: false?
            }
            QDateTime aDateTimeExceeding = QDateTime::fromMSecsSinceEpoch(timestampAsQint64 * 1000);
            dateTimesThatMeetOrExceedThreshold.append(aDateTimeExceeding);
        }
    }

    //now that we have all the datetimes we don't want to use, check to see if the corresponding filepaths for both files/folders (keys in internal tables) are in 'old tables' and if they are, use those instead (write them to new table (so they become old table and 'recursion' works)). if they AREN'T, use TODOreq idfk (there might not even be any of these cases but idk my head hurts)

    int dateTimesThatMeetOrExceedThresholdSize = dateTimesThatMeetOrExceedThreshold.size();
    for(int i = 0; i < dateTimesThatMeetOrExceedThresholdSize; ++i)
    {
        QDateTime currentDateTimeExceedingThreshold = dateTimesThatMeetOrExceedThreshold.at(i);

        //files
        QList<QString /*absoluteFilePath*/> filesKeysToTryToUseOldTableTimestamp = m_TimestampsByAbsoluteFilePathHash_Files.keys(currentDateTimeExceedingThreshold);
        //folders
        QList<QString /*absoluteFilePath*/> foldersKeysToTryToUseOldTableTimestamp = m_TimestampsByAbsoluteFilePathHash_Folders.keys(currentDateTimeExceedingThreshold);

        //iterate old tables, and if the key is in the old table, use the value from the old table in the new table :)
        //Files
        int filesKeysToTryToUseOldTableTimestampSize = filesKeysToTryToUseOldTableTimestamp.size();
        for(int j = 0; j < filesKeysToTryToUseOldTableTimestampSize; ++j)
        {
            QString currentFilePath = filesKeysToTryToUseOldTableTimestamp.at(j);
            if(m_OldTimestampsByAbsoluteFilePathHash_Files.contains(currentFilePath))
            {
                QDateTime unFuckedDateTime = m_OldTimestampsByAbsoluteFilePathHash_Files.value(currentFilePath); //Was like years of suppression now satisfied in writing that variable name...
                m_TimestampsByAbsoluteFilePathHash_Files.insert(currentFilePath, unFuckedDateTime);
            }
            else
            {
                emit d("fucked state detected XYZ, more hacking required"); //since I'm testing with live data anyways... :)
                //If I do more hackery here, should copy/paste it to Folders below (ABC)
                return;
            }
        }
        //Folders
        int foldersKeysToTryToUseOldTableTimestampSize = foldersKeysToTryToUseOldTableTimestamp.size();
        for(int j = 0; j < foldersKeysToTryToUseOldTableTimestampSize; ++j)
        {
            QString currentFilePath = foldersKeysToTryToUseOldTableTimestamp.at(j);
            if(m_OldTimestampsByAbsoluteFilePathHash_Folders.contains(currentFilePath))
            {
                QDateTime unFuckedDateTime = m_OldTimestampsByAbsoluteFilePathHash_Folders.value(currentFilePath);
                m_TimestampsByAbsoluteFilePathHash_Folders.insert(currentFilePath, unFuckedDateTime);
            }
            else
            {
                emit d("fucked state detected ABC, more hacking required");
                return;
            }
        }

        //TODOreq: i think i need to do something with the timestamp if it isn't found in either file/folder hash, which would mean it's a..... new or renamed file. but maybe it doesn't matter idfk. so many corner cases xD
    }
}
bool LastModifiedDateHeirarchyMolester::doSharedInitBetweenXmlAndEasyTree(const QString &directoryHeirarchyCorrespingToTimestampFile, const QString &timestampFilePath)
{
    m_TimestampsByAbsoluteFilePathHash_Files.clear();
    m_TimestampsByAbsoluteFilePathHash_Folders.clear();

    m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended = directoryHeirarchyCorrespingToTimestampFile;
    if(!m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended.endsWith("/"))
    {
        m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended.append("/");
    }
    QFileInfo dirToMolestFileInfo(m_DirectoryHeirarchyCorrespingToTimestampFileWithSlashAppended);
    if(!dirToMolestFileInfo.exists())
    {
        emit d("Directory to Molest does Not Exist");
        return false;
    }
    if(!dirToMolestFileInfo.isDir())
    {
        emit d("Directory to Molest is not a directory");
        return false;
    }
    m_TimestampFile.setFileName(timestampFilePath); //TODOreq: doesn't belong here in this project, but I need to make a mental note to make sure that easy tree file doesn't "stat" itself
    if(!m_TimestampFile.exists())
    {
        emit d("Timestamp File Does Not Exist");
        return false;
    }
    if(!m_TimestampFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("Failed to open timestamp File for reading");
        return false;
    }
    if(!m_TimestampFile.seek(0))
    {
        emit d("failed to seek to beginning of timestamp file");
        return false;
    }
    return true;
}
bool LastModifiedDateHeirarchyMolester::recursivelyCheckForMissedFilesAndFolders(const QFileInfoList fileInfoList)
{
    QListIterator<QFileInfo> it(fileInfoList);
    while(it.hasNext())
    {
        QFileInfo currentFileInfo = it.next();
        if(currentFileInfo.isDir())
        {
            QString dirNameForComparing = currentFileInfo.canonicalFilePath() + "/";
            if(!m_TimestampsByAbsoluteFilePathHash_Folders.contains(dirNameForComparing))
            {
                emit d("found a missing directory woot:" + dirNameForComparing);
                m_TimestampsByAbsoluteFilePathHash_Folders.insert(dirNameForComparing, m_DateTimeToGiveMissedAndManuallyFoundFiles);
            }

            //regardless of whether or not it's in the hash, we still need to cd into it and make sure all it's children are too
            QDir nextDir(dirNameForComparing);
            nextDir.setSorting(QDir::DirsFirst | QDir::Name);
            nextDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Hidden);
            QFileInfoList nextDirFileInfoList = nextDir.entryInfoList();

            if(!recursivelyCheckForMissedFilesAndFolders(nextDirFileInfoList)) //the joys of recursion that your average human never experiences ^_^ ("recursion is itself a reward" -xkcd)
            {
                return false;
            }
        }
        else if(currentFileInfo.isFile())
        {
            QString absoluteFilePath = currentFileInfo.canonicalFilePath();
            if(!m_TimestampsByAbsoluteFilePathHash_Files.contains(absoluteFilePath))
            {
                emit d("found a missing file woot: " + absoluteFilePath);
                m_TimestampsByAbsoluteFilePathHash_Files.insert(absoluteFilePath, m_DateTimeToGiveMissedAndManuallyFoundFiles); //almost forgot this line, would have been a bitch of a bug (or maybe not, but i hypothesize (had:theorize) yes (but could be wrong (but i think i'm right (you just never know (you can only make educated guesses (but no matter what, you're either right or wrong (that is not an educated guess, so i proved that earlier sentence wrong xD))))))
            }
        }
        else
        {
            emit d("found something in recursivelyCheckForMissedFilesAndFolders that's not a file or folder and dunno how to handle it: " + currentFileInfo.canonicalFilePath());
            return false; //meh that wasn't so hard what was i bitching about last time (just scurred i guess)
        }
    }
    return true;
}
//TODOreq: fix bug where files are touched after folders, which has the consequence of touching the folder [to the time that the file was touched (not to, but AT (execution time)!). Easy solution: touch all files first, then all folders. Hmm actually just tested and it doesn't appear to be the case EXCEPT when file _contents_ are modified (touching is not that). STILL I should re-arrange files first just to be on the safe side, especially since I am modifying files~
void LastModifiedDateHeirarchyMolester::molestLastModifiedDateHeirarchy(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &easyTreeFilePath, bool easyTreeLinesHaveCreationDate, bool xmlFormattedTreeCommandOutputActually)
{
    if(easyTreeLinesHaveCreationDate && xmlFormattedTreeCommandOutputActually)
    {
        emit d("You can't have a creation date in an XML formatted tree command output, so uhh just stopping to play it safe because I dunno wtf you wan't");
        return;
    }

    if(xmlFormattedTreeCommandOutputActually)
    {
        if(!loadFromXml(directoryHeirarchyCorrespingToEasyTreeFile, easyTreeFilePath))
        {
            emit d("failed to load timestamps from xml");
            return;
        }
    }
    else
    {
        if(!loadFromEasyTreeFile(directoryHeirarchyCorrespingToEasyTreeFile, easyTreeFilePath, easyTreeLinesHaveCreationDate))
        {
            emit d("failed to load timestamps from easy tree file");
            return;
        }
    }

    if(!molestUsingInternalTables())
    {
        emit d("failed to molest");
        return;
    }

    emit d("got to the end of the easy tree file, so if you didn't see any errors, molestation probably went smoothly");
}

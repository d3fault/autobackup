#include "qfilesystemwatcher2.h"

QFileSystemWatcher2::QFileSystemWatcher2(QObject *parent) :
    QFileSystemWatcher(parent)
{
    connect(this, SIGNAL(directoryChanged(QString)), this, SLOT(figureOutFileChangeFromDirChange(QString)));
    m_OldDirStructure = new DirStructure(); //TODO: we should populate the dir structure state when addPath is called. this will probably cause a rename from old to just 'current', but the code stays the same

}
void QFileSystemWatcher2::figureOutFileChangeFromDirChange(const QString &dirString)
{
    QDir dir(dirString);
    QFileInfoList dirInfoList = dir.entryInfoList();

    m_NewDirStructure = populateDirStructureFromFileInfoList(dirInfoList);

    emitSignalsForDirInfoListChanges();
    m_OldDirStructure = m_NewDirStructure;
}
void QFileSystemWatcher2::populateDirStructureFromFileInfoList(QFileInfoList &newFileInfoList)
{
    DirStructure *newDirStructure = new DirStructure();

    QListIterator<QFileInfo> it(newFileInfoList);

    while(it.hasNext())
    {
        QFileInfo newDirEntry(it.next());
        newDirEntry.setCaching(false);

        FileInfoHardCopy *fileInfoHardCopy = new FileInfoHardCopy();
        fileInfoHardCopy->CreationDate = newDirEntry.created();
        fileInfoHardCopy->ModifiedDate = newDirEntry.lastModified();
        fileInfoHardCopy->Size = newDirEntry.size();

        newDirStructure->insert(newDirEntry.absoluteFilePath(), fileInfoHardCopy);
    }
    return newDirStructure;
}
void QFileSystemWatcher2::emitSignalsForDirInfoListChanges(const QFileInfoList &oldInfoList, const QFileInfoList &newList)
{
    QList<QString> entriesAdded;
    QList<QString> entriesDeleted;

    QStringList alreadyEmittedAbsolutePaths;

    //renames

    QHashIterator<QString, FileInfoHardCopy*> it(m_NewDirStructure);
    QHashIterator<QString, FileInfoHardCopy*> oldIt(m_OldDirStructure);

    //first, we figure out what was added and what was deleted
    while(it.hasNext())
    {
        it.next();
        //FileInfoHardCopy *fileInfoHardCopy = it.value();

        //it's confusing at first, but these two bool variables can't be merged because each one must be "proved" and we start with the default of what it isn't (or something)
        bool wasHereBefore = false;
        bool wasntHereBefore = true;
        while(oldIt.hasNext())
        {
            oldIt.next();
            //FileInfoHardCopy *oldFileInfo = oldIt.value();
            if(it.key() == oldIt.key())
            {
                //filenames match
                wasHereBefore = true;
                wasntHereBefore = false;
                break;
            }
        }

        //this is where it gets especially confusing. it appears as though we are comparing the same two booleans (because of my variable naming choice) but we really not
        if(!wasHereBefore)
        {
            //the new file failed to prove that it was here before by setting itself to true, so we know it's a new file
            entriesAdded.append(fileInfo);
        }
        if(wasntHereBefore)
        {
            //the new file failed to prove that it wasn't here before by setting itself to false, so we know it's a file deletion
            entriesDeleted.append(fileInfo);
        }
    }

    QList<QFileInfo> addedAndDeletedEntriesWithMatchingCriteriaForRename;
    QListIterator<QFileInfo> aIt(entriesAdded);
    QListIterator<QFileInfo> dIt(entriesDeleted);
    bool notEnoughCriteriaAndWeShouldAbortTheRenameDetectionOrMaybeDoCrc32CheckingAsLastResort = false;



    while(aIt.hasNext())
    {
        aIt.next();
        const QDateTime &newCreationDate = aIt.value()->CreationDate;
        bool weAlreadyFoundMatchingDelete = false;

        while(oldIt.hasNext())
        {
            oldIt.next();

        }
    }



    while(aIt.hasNext())
    {
        QFileInfo fileInfo(aIt.next());
        fileInfo.setCaching(false);
        bool weAlreadyFoundMatchingDelete = false;
        while(dIt.hasNext())
        {
            QFileInfo oldFileInfo(dIt.next());
            oldFileInfo.setCaching(true); // i think we _NEED_ caching so we get the old info? the manner in which it lazy loads is messing this up
            if(oldFileInfo.created() == fileInfo.created() && oldFileInfo.size() == fileInfo.size())
            {
                if(weAlreadyFoundMatchingDelete)
                {
                    notEnoughCriteriaAndWeShouldAbortTheRenameDetectionOrMaybeDoCrc32CheckingAsLastResort = true; //ABORT
                    break;
                }
                addedAndDeletedEntriesWithMatchingCriteriaForRename.append(fileInfo);
                weAlreadyFoundMatchingDelete = true;
            }
        }
        if(notEnoughCriteriaAndWeShouldAbortTheRenameDetectionOrMaybeDoCrc32CheckingAsLastResort)
        {
            break;
        }
    }


    if(notEnoughCriteriaAndWeShouldAbortTheRenameDetectionOrMaybeDoCrc32CheckingAsLastResort)
    {
        //for now, just abort. in the future we could do a crc32 for this folder or maybe chunks and then hash them as a way to test for change without testing the entire file IDFK
        //^^possibility of false positives with this too in similar files (for whatever reasons) during renames fml
    }
    else
    {
         QListIterator<QFileInfo> renamesIterator(addedAndDeletedEntriesWithMatchingCriteriaForRename);
         while(renamesIterator.hasNext())
         {
             QFileInfo renamedFileInfo(renamesIterator.next());
             emit fileChanged(renamedFileInfo.absoluteFilePath());
             alreadyEmittedAbsolutePaths.append(renamedFileInfo.absoluteFilePath());
         }
    }


    //content changes

    //TODO: modified date???

    //resize
    Q

    it.toFront();
    oldIt.toFront();
    while(it.hasNext())
    {
        QFileInfo fileInfo(it.next());
        fileInfo.setCaching(false);
        while(oldIt.hasNext())
        {
            QFileInfo oldFileInfo(oldIt.next());
            oldFileInfo.setCaching(true); //i am pretty sure i can't use QFileInfo because of this
            if(fileInfo.absoluteFilePath() == fileInfo.absoluteFilePath())
            {
                if(fileInfo.size() != oldFileInfo.size())
                {
                    sizeChangesList.append(fileInfo);
                }
            }
        }
    }





    //5 types of changes need to be detected

    //1) rename
    //2) resize
    //3) contentsChanged ... crc32 or what? this would be where those flag enum filters would come in handy so you can just go by checking the date if it's enough (by default, it isn't.. but that can be changed later)
    //4) newFile
    //5) fileDelete

    //1) rename is a matching (filesize, creationDate) newFile with a fileDelete <---- this could trigger false positives unless you do contents checking (and the time is not enough)................................................ seriously there's like no good solution to this that i can think of. does qfsw even react to renames?

    //i think the creation date checking is the only savior of renames. if the creation date does not live through the rename, there is no hope.
}

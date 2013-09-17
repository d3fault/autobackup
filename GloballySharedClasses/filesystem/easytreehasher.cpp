#include "easytreehasher.h"

const qint64 EasyTreeHasher::m_MaxReadSize = 4194304; //4mb (max) read buffer
const QString EasyTreeHasher::m_DirSeparator = "/";

EasyTreeHasher::EasyTreeHasher(QObject *parent)
    : QObject(parent), m_Colon(":")
{
    m_EscapedColon.append("\\");
    m_EscapedColon.append(m_Colon);

    m_SortFlags |= QDir::DirsFirst;
    m_SortFlags |= QDir::Name;

    //for conservative filter, fuck symbolic links (?????????? right??? idfk tbh hard to answer that... mb I should have it as a UI checkbox TODOoptional?) and system files :-P
    m_ConservativeFiltersForSeeingIfThereAreFilesToCopy |= QDir::Dirs;
    m_ConservativeFiltersForSeeingIfThereAreFilesToCopy |= QDir::Files;
    m_ConservativeFiltersForSeeingIfThereAreFilesToCopy |= QDir::NoSymLinks;
    m_ConservativeFiltersForSeeingIfThereAreFilesToCopy |= QDir::NoDotAndDotDot;
    m_ConservativeFiltersForSeeingIfThereAreFilesToCopy |= QDir::Hidden;
}
void EasyTreeHasher::recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(QDir &sourceDir, QDir &emptyDestinationDir, QIODevice *easyTreeHashOutputIODevice, QCryptographicHash::Algorithm algorithm)
{
    if(!sourceDir.exists())
    {
        emit e("The Source Directory Does Not Exist");
        return;
    }

    if(!emptyDestinationDir.exists())
    {
        emit e("The Destination Directory Does Not Exist");
        return;
    }

    sourceDir.setSorting(m_SortFlags);
    sourceDir.setFilter(m_ConservativeFiltersForSeeingIfThereAreFilesToCopy);
    QFileInfoList initialFileInfoList = sourceDir.entryInfoList();
    if(initialFileInfoList.count() < 1)
    {
        emit d("Error-ish: There Are No Files In The Source Directory. Or I guess you could say: DONE BITCH I AM SO FAST");
        return;
    }
    m_SourceDirectoryAbsolutePathLength = sourceDir.canonicalPath().length() + 1; //the extra one is to account for the trailing slash, that canonical path returned here doesn't have a trailing slash here but there will be a directory separator when calling .remove on some other (child) canonical path... which is either a sub-file or sub-folder... so the directory separator is obviously needed

    QDir::Filters liberalFiltersForDetectingIfDirIsEmpty; //symlinks! system files!
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::Dirs;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::Files;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::NoDotAndDotDot;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::System;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::Hidden;
    emptyDestinationDir.setFilter(liberalFiltersForDetectingIfDirIsEmpty);
    if(emptyDestinationDir.count() > 0)
    {
        emit e("The Destination Directory Is Not Empty");
        return;
    }

    if(!easyTreeHashOutputIODevice->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit e("Failed To Open Easy Tree Hash Output Device For Writing");
        return;
    }

    m_EasyTreeHashTextStream.setDevice(easyTreeHashOutputIODevice);
    if(!easyTreeHashOutputIODevice->isSequential())
    {
        m_EasyTreeHashTextStream.seek(0);
    }

    m_CryptographicHashAlgorithm = algorithm;

    //leaving in for lulz, i really wrote it:
    //theFuckingRecursiveFunction(GodDamnitImplicitSharingEitherMakesThisTooEasyOrTooHardICantTell);

    emit d("About to call the recursive function for the first time");

    copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(initialFileInfoList, emptyDestinationDir);

    easyTreeHashOutputIODevice->close();

    emit d("The Recursive Copy/Tree/Hash Function Has Finished");
}
QString EasyTreeHasher::getCurrentSourceFileInfo_RelativePath()
{
    return m_CurrentSourceFileInfo.canonicalFilePath().remove(0, m_SourceDirectoryAbsolutePathLength);
}
QString EasyTreeHasher::getCurrentSourceFileInfo_FilenameOrDirnameOnly()
{
    return getFilenameOrDirnameOnly(m_CurrentSourceFileInfo.canonicalFilePath());
}
QString EasyTreeHasher::getFilenameOrDirnameOnly(QString filepathOrDirPath)
{
    return filepathOrDirPath.split(m_DirSeparator,QString::SkipEmptyParts).last();
}
QString EasyTreeHasher::getCurrentSourceFileInfoPath_ColonEscapedAndRelative()
{
    return getCurrentSourceFileInfo_RelativePath().replace(m_Colon, m_EscapedColon, Qt::CaseSensitive);
}
void EasyTreeHasher::copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(const QFileInfoList &filesAndFoldersInCurrentDirToCopyAndTreeAndHash, QDir &destAlreadyMkdirDAndCDdInto)
{
    QListIterator<QFileInfo> it(filesAndFoldersInCurrentDirToCopyAndTreeAndHash);
    while(it.hasNext())
    {
        m_CurrentSourceFileInfo = it.next();
        m_CurrentFilenameOrDirnameOnly = getCurrentSourceFileInfo_FilenameOrDirnameOnly();
        if(m_CurrentSourceFileInfo.isDir())
        {
            addDirectoryEntryToEasyTreeHashOutput();

            //I'm more confident that this belongs here than doesn't, but if it doesn't work this will be the first thing to try without:
            QDir nextDestDir = destAlreadyMkdirDAndCDdInto; //It seems kind of strange that there's no other usages in this method... except ok when you factor in the while loop then it's used lots of times! Bleh. I suck at implicit sharing but should try to learn it better

            if(nextDestDir.mkdir(m_CurrentFilenameOrDirnameOnly))
            {
                if(nextDestDir.cd(m_CurrentFilenameOrDirnameOnly))
                {
                    //TODOoptimization: can maybe use member variable to lessen heap allocs... but I bet we're IO bound anyways...
                    QDir nextSourceDir(m_CurrentSourceFileInfo.canonicalFilePath());
                    nextSourceDir.setSorting(m_SortFlags);
                    nextSourceDir.setFilter(m_ConservativeFiltersForSeeingIfThereAreFilesToCopy);

                    QFileInfoList nextFileInfoList = nextSourceDir.entryInfoList();
                    copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(nextFileInfoList, nextDestDir);
                }
                else
                {
                    //TODOreq: cd failed
                    emit e("cd failed");
                }
                //TO DOnereq (a cdUp might have fixed it, but so does the seemingly unncecessary variable introduced: nextDestDir. Sole purpose is so destAlreadyMkdirDAndCDdInto retains current CD when nextDestDir detaches during mkdir/cd): do i need to cdUp the dest here or does implicit sharing take care of that? wtfz i should just stick with strings etc. At the very least it's confusing to think that the same copy gets mkdir'd/cd'd multiple times in the while(it.hasNext()) loop. That leads me to believe we need to cdUp here...... OR perhaps another solution is to make another QDir on the stack and assign it to destAlreadyMkdirDAndCDdInto... just before the mkdir call (so it (the new one) detaches and we don't have to worry about doing cdUp)... idfk

                //TODOreq(? App seems to work so I highly doubt it): also possibly for nextSourceDir, but i think that one is ok actually because it's scoped?
            }
            else
            {
                //TODOreq: mkdir failed
                emit e("mkdir failed");
            }
        }
        else if(m_CurrentSourceFileInfo.isFile())
        {
            EasyTreeHashItem *easyTreeHashItem = copyAndHashSimultaneously(m_CurrentSourceFileInfo, destAlreadyMkdirDAndCDdInto, m_CryptographicHashAlgorithm, m_SourceDirectoryAbsolutePathLength);

            if(!easyTreeHashItem)
            {
                emit e("there was an error in the copy/hash routine, check stderr");
                return; //TODOreq: error out of recursion properly
            }

            m_EasyTreeHashTextStream << easyTreeHashItem->toColonSeparatedLineOfText() << endl;

            //Debug:
            emit d("Copied/Tree'd/Hashed: " + m_CurrentFilenameOrDirnameOnly);

            delete easyTreeHashItem;
        }
        else
        {
            //TODOreq: perhaps report that some kind of file (special?) was found? idfk. error out?
            emit e("not a file or a directory");
        }
    }
}
//this method is semi-outdated now that we've migrated to EasyTreeHashItem::toColonSeparatedLineOfText -- but if it ain't broke don't fix it
void EasyTreeHasher::addDirectoryEntryToEasyTreeHashOutput()
{
    m_EasyTreeHashTextStream << getCurrentSourceFileInfoPath_ColonEscapedAndRelative() << m_DirSeparator;

    //stream a colon, the creation date, another colon, the last modified date, and lastly a newline
    m_EasyTreeHashTextStream << m_Colon << QString::number(m_CurrentSourceFileInfo.created().toMSecsSinceEpoch()/1000) << m_Colon << QString::number(m_CurrentSourceFileInfo.lastModified().toMSecsSinceEpoch()/1000) << endl;

    //Debug:
    emit d("Entering/Making Dir: " + m_CurrentFilenameOrDirnameOnly);
}
//caller takes owner of returned EasyTreeHasher
EasyTreeHashItem *EasyTreeHasher::copyAndHashSimultaneously(const QFileInfo &sourceFileInfoWithAbsolutePath, const QDir &destDir, QCryptographicHash::Algorithm cryptographicHashAlgorithm, int absoluteSourcePathLengthWithTrailingSlash)
{
    EasyTreeHashItem *easyTreeHashItem = 0;

    QCryptographicHash hasher(cryptographicHashAlgorithm);
    QString sourceAbsoluteCanonicalFilePath(sourceFileInfoWithAbsolutePath.canonicalFilePath());
    QFile sourceFile2CopyTreeAndHash(sourceAbsoluteCanonicalFilePath);
    if(sourceFile2CopyTreeAndHash.open(QFile::ReadOnly))
    {
        QFile destinationFile2Write(destDir.canonicalPath() + m_DirSeparator + getFilenameOrDirnameOnly(sourceAbsoluteCanonicalFilePath));
        if(destinationFile2Write.open(QFile::WriteOnly))
        {
            easyTreeHashItem = new EasyTreeHashItem();
            easyTreeHashItem->setIsDirectory(false);
            easyTreeHashItem->setHasHash(true);
            QString relativeFilePath = sourceAbsoluteCanonicalFilePath.mid(absoluteSourcePathLengthWithTrailingSlash);
            easyTreeHashItem->setRelativeFilePath(relativeFilePath);
            easyTreeHashItem->setFileSize(sourceFileInfoWithAbsolutePath.size());
            easyTreeHashItem->setCreationDateTime(sourceFileInfoWithAbsolutePath.created());
            easyTreeHashItem->setLastModifiedDateTime(sourceFileInfoWithAbsolutePath.lastModified());

            qint64 bytesAvail = sourceFile2CopyTreeAndHash.bytesAvailable();
            qint64 toRead;
            while(bytesAvail > 0)
            {
                toRead = qMin(m_MaxReadSize, bytesAvail);


                //Next 3 lines are bulk of program! Entire fucking reason for existence!

                QByteArray readChunkArray = sourceFile2CopyTreeAndHash.read(toRead); //One read (copy)
                hasher.addData(readChunkArray); //1st Utilization of that read (hash)
                destinationFile2Write.write(readChunkArray); //2nd Utilization of that read, the write (paste)

                //Efficient as fuck... or premature optimization? YOU BE THE JUDGE! 'man tee' would have saved me bundles of time xD. Ahh well fuck it I still hate shell scripting. I even knew about tee before writing this... but I didn't realize it worked on stdout and didn't know about the anonymous pipe stuff. I thought it was mainly for saving/showing "debug [text] output" only, not splitting binary output :-P


                bytesAvail = sourceFile2CopyTreeAndHash.bytesAvailable();
            }

            //TODOoptional: we could check that both file sizes are the same here if we wanted...

            easyTreeHashItem->setHash(hasher.result()); //TODOoptional: mb an #ifdef debug or some shit to see if we want to waste cpu cycles sending the result() to the GUI... for now fuck it, i just want errors~

            destinationFile2Write.flush();
            destinationFile2Write.close();
        }
        else
        {
            //TODOreq: destination file open for writing failed -- error'ing out of the recursion is going to be a bitch :-P
            qWarning("destination file open for writing failed");
        }
        sourceFile2CopyTreeAndHash.close();
    }
    else
    {
        //TODOreq: source file open for reading failed -- error'ing out of the recursion is going to be a bitch :-P
        qWarning("source file open for reading failed");
    }

    return easyTreeHashItem;
}

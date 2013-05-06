#include "easytreehasher.h"

const qint64 EasyTreeHasher::m_MaxReadSize = 4194304; //4mb (max) read buffer

EasyTreeHasher::EasyTreeHasher(QObject *parent)
    : QObject(parent), m_DirSeparator("/"), m_Colon(":"), m_Hasher(0)
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

    deleteHasherIfNotZero(); //multiple runs without closing down application... but changing algorithm in between. QCryptographicHash doesn't provide a way to change the algorithm once instantiated
    m_Hasher = new QCryptographicHash(algorithm);

    //leaving in for lulz, i really wrote it:
    //theFuckingRecursiveFunction(GodDamnitImplicitSharingEitherMakesThisTooEasyOrTooHardICantTell);

    emit d("About to call the recursive function for the first time");

    copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(initialFileInfoList, emptyDestinationDir);

    easyTreeHashOutputIODevice->close();

    emit d("The Recursive Copy/Tree/Hash Function Has Finished");
}
EasyTreeHasher::~EasyTreeHasher()
{
    deleteHasherIfNotZero();
}
void EasyTreeHasher::deleteHasherIfNotZero()
{
    //I thought C++ allowed it to be deleted even if it is null.. but my shit just segfaulted (ANEURISM) so I guess not? Probably need to update code in "other projects" (lol I guess I'm thinking about releasing this) too...
    if(m_Hasher)
    {
        delete m_Hasher;
        m_Hasher = 0;
    }
}
QString EasyTreeHasher::getCurrentSourceFileInfo_RelativePath()
{
    return m_CurrentSourceFileInfo.canonicalFilePath().remove(0, m_SourceDirectoryAbsolutePathLength);
}
QString EasyTreeHasher::getCurrentSourceFileInfo_FilenameOrDirnameOnly()
{
    return m_CurrentSourceFileInfo.canonicalFilePath().split(m_DirSeparator,QString::SkipEmptyParts).last();
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
            copyAndHashSimultaneously(destAlreadyMkdirDAndCDdInto); //m_CurrentSourceFileInfo is member, hash output is member also... so we only need to pass in the [scope-local] dest dir

            addFileEntryToEasyTreeHashOutput();
        }
        else
        {
            //TODOreq: perhaps report that some kind of file (special?) was found? idfk. error out?
            emit e("not a file or a directory");
        }
    }
}
void EasyTreeHasher::addDirectoryEntryToEasyTreeHashOutput()
{
    m_EasyTreeHashTextStream << getCurrentSourceFileInfoPath_ColonEscapedAndRelative() << m_DirSeparator;

    //stream a colon, the file size, another colon, the creation date, another colon, the last modified date, and lastly a newline
    m_EasyTreeHashTextStream << m_Colon << QString::number(m_CurrentSourceFileInfo.created().toMSecsSinceEpoch()/1000) << m_Colon << QString::number(m_CurrentSourceFileInfo.lastModified().toMSecsSinceEpoch()/1000) << endl;

    //Debug:
    emit d("Entering/Making Dir: " + m_CurrentFilenameOrDirnameOnly);
}
void EasyTreeHasher::copyAndHashSimultaneously(const QDir &destDir)
{
    m_Hasher->reset();
    m_SourceFile2CopyTreeAndHash.setFileName(m_CurrentSourceFileInfo.canonicalFilePath());
    if(m_SourceFile2CopyTreeAndHash.open(QFile::ReadOnly))
    {
        m_DestinationFile2Write.setFileName(destDir.canonicalPath() + m_DirSeparator + m_CurrentFilenameOrDirnameOnly);
        if(m_DestinationFile2Write.open(QFile::WriteOnly))
        {
            qint64 bytesAvail = m_SourceFile2CopyTreeAndHash.bytesAvailable();
            qint64 toRead;
            while(bytesAvail > 0)
            {
                toRead = qMin(m_MaxReadSize, bytesAvail);


                //Next 3 lines are bulk of program! Entire fucking reason for existence!

                QByteArray readChunkArray = m_SourceFile2CopyTreeAndHash.read(toRead); //One read (copy)
                m_Hasher->addData(readChunkArray); //1st Utilization of that read (hash)
                m_DestinationFile2Write.write(readChunkArray); //2nd Utilization of that read, the write (paste)

                //Efficient as fuck... or premature optimization? YOU BE THE JUDGE!


                bytesAvail = m_SourceFile2CopyTreeAndHash.bytesAvailable();
            }

            //TODOoptional: we could check that both file sizes are the same here if we wanted...

            m_HashResult = m_Hasher->result(); //TODOoptional: mb an #ifdef debug or some shit to see if we want to waste cpu cycles sending the result() to the GUI... for now fuck it, i just want errors~

            m_DestinationFile2Write.flush();
            m_DestinationFile2Write.close();
        }
        else
        {
            //TODOreq: destination file open for writing failed -- error'ing out of the recursion is going to be a bitch :-P
            emit e("destination file open for writing failed");
        }
        m_SourceFile2CopyTreeAndHash.close();
    }
    else
    {
        //TODOreq: source file open for reading failed -- error'ing out of the recursion is going to be a bitch :-P
        emit e("source file open for reading failed");
    }
}
void EasyTreeHasher::addFileEntryToEasyTreeHashOutput()
{
    m_EasyTreeHashTextStream << getCurrentSourceFileInfoPath_ColonEscapedAndRelative() << m_Colon << m_CurrentSourceFileInfo.size() << m_Colon << QString::number(m_CurrentSourceFileInfo.created().toMSecsSinceEpoch()/1000) << m_Colon << QString::number(m_CurrentSourceFileInfo.lastModified().toMSecsSinceEpoch()/1000) << m_Colon << m_HashResult.toHex() << endl;

    //Debug:
    emit d("Copied/Tree'd/Hashed: " + m_CurrentFilenameOrDirnameOnly);
}

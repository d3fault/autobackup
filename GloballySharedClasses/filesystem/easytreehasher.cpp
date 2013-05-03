#include "easytreehasher.h"

qint64 EasyTreeHasher::m_MaxReadSize = 4000000; //~4mb buffer (should probably make this a power of 2? TODOoptimization)

EasyTreeHasher::EasyTreeHasher(QObject *parent)
    : QObject(parent), m_Colon(":"), m_DirSeparator("/")
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
void EasyTreeHasher::recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(const QDir &sourceDir, const QDir &emptyDestinationDir, const QIODevice *easyTreeHashOutputIODevice, QCryptographicHash::Algorithm algorithm)
{
    if(!sourceDir.exists())
    {
        emit d("Error: The Source Directory Does Not Exist");
        return;
    }

    if(!emptyDestinationDir.exists())
    {
        emit d("Error: The Destination Directory Does Not Exist");
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
    m_SourceDirectoryAbsolutePathLength = sourceDir.canonicalPath().length() + 1; //the extra one is to account for the slash, that canonical path doesn't have here but we will have when calling .remove on some other (child) canonical path

    QDir::Filters liberalFiltersForDetectingIfDirIsEmpty; //symlinks! system files!
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::Dirs;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::Files;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::NoDotAndDotDot;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::System;
    liberalFiltersForDetectingIfDirIsEmpty |= QDir::Hidden;
    emptyDestinationDir.setFilter(liberalFiltersForDetectingIfDirIsEmpty);
    if(emptyDestinationDir.count() > 0)
    {
        emit d("Error: The Destination Directory Is Not Empty");
        return;
    }

    if(!easyTreeHashOutputFilePath.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit d("Error: Failed To Open Easy Tree Hash Output File For Writing");
        return;
    }

    m_EasyTreeHashTextStream.setDevice(easyTreeHashOutputIODevice);
    if(!easyTreeHashOutputIODevice->isSequential())
    {
        m_EasyTreeHashTextStream.seek(0);
    }

    delete m_Hasher; //multiple runs without closing down application... but changing algorithm in between. QCryptographicHash doesn't provide a way to change the algorithm once instantiated
    m_Hasher = new QCryptographicHash(algorithm);

    //leaving in for lulz, i really wrote it:
    //theFuckingRecursiveFunction(GodDamnitImplicitSharingEitherMakesThisTooEasyOrTooHardICantTell);

    copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(initialFileInfoList, emptyDestinationDir);

    easyTreeHashOutputIODevice->close();

    emit d("The Recursive Copy/Tree/Hash Function Has Finished");
}
EasyTreeHasher::~EasyTreeHasher()
{
    delete m_Hasher;
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
void EasyTreeHasher::copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(const QFileInfoList &filesAndFoldersInCurrentDirToCopyAndTreeAndHash, const QDir &destAlreadyMkdirDAndCDdInto)
{
    QListIterator<QFileInfoList> it(filesAndFoldersInCurrentDirToCopyAndTreeAndHash);
    while(it.hasNext())
    {
        m_CurrentSourceFileInfo = it.next();
        m_CurrentFilenameOrDirnameOnly = getCurrentSourceFileInfo_FilenameOrDirnameOnly();
        if(m_CurrentSourceFileInfo.isDir())
        {
            addDirectoryEntryToEasyTreeHashOutput();

            if(destAlreadyMkdirDAndCDdInto.mkdir(m_CurrentFilenameOrDirnameOnly))
            {
                if(destAlreadyMkdirDAndCDdInto.cd(m_CurrentFilenameOrDirnameOnly))
                {
                    //TODOoptimization: can maybe use member variable to lessen heap allocs... but I bet we're IO bound anyways...
                    QDir nextSourceDir(m_CurrentSourceFileInfo.canonicalFilePath());
                    nextSourceDir.setSorting(m_SortFlags);
                    nextSourceDir.setFilter(m_ConservativeFiltersForSeeingIfThereAreFilesToCopy);

                    QFileInfoList nextFileInfoList = nextSourceDir.entryInfoList();
                    copyEachOfTheseFilesToTheDestinationAndRecurseIntoDirsDoingTheSameWhileMkDiringIntoDestinationOhAndAlsoWritingEverythingToEasyTreeHashOutputIODeviceRofl(nextFileInfoList, destAlreadyMkdirDAndCDdInto);
                }
                else
                {
                    //TODOreq: cd failed
                }
                //TODOreq: do i need to cdUp the dest here or does implicit sharing take care of that? wtfz i should just stick with strings etc. At the very least it's confusing to think that the same copy gets mkdir'd/cd'd multiple times in the while(it.hasNext()) loop. That leads me to believe we need to cdUp here...... OR perhaps another solution is to make another QDir on the stack and assign it to destAlreadyMkdirDAndCDdInto... just before the mkdir call (so it (the new one) detaches and we don't have to worry about doing cdUp)... idfk

                //TODOreq: also possibly for nextSourceDir, but i think that one is ok actually because it's scoped?
            }
            else
            {
                //TODOreq: mkdir failed
            }
        }
        else if(m_CurrentSourceFileInfo.isFile())
        {
            copyAndHashSimultaneously(destAlreadyMkdirDAndCDdInto); //m_CurrentSourceFileInfo is member, hash output should be member also

            addFileEntryToEasyTreeHashOutput();
        }
        else
        {
            //TODOreq: perhaps report that some kind of file (special?) was found? idfk. error out?
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

                QByteArray readChunkArray = m_SourceFile2CopyTreeAndHash.read(toRead); //One Read
                m_Hasher->addData(readChunkArray); //1st Utilization of that read
                m_DestinationFile2Write.write(readChunkArray); //2nd Utilization of that read

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
        }
        m_SourceFile2CopyTreeAndHash.close();
    }
    else
    {
        //TODOreq: source file open for reading failed -- error'ing out of the recursion is going to be a bitch :-P
    }
}
void EasyTreeHasher::addFileEntryToEasyTreeHashOutput()
{
    m_EasyTreeHashTextStream << getCurrentSourceFileInfoPath_ColonEscapedAndRelative() << m_Colon << m_CurrentSourceFileInfo.size() << m_Colon << QString::number(m_CurrentSourceFileInfo.created().toMSecsSinceEpoch()/1000) << m_Colon << QString::number(m_CurrentSourceFileInfo.lastModified().toMSecsSinceEpoch()/1000) << m_Colon << m_HashResult.toHex() << endl;

    //Debug:
    emit d("Copied/Tree'd/Hashed: " + m_CurrentFilenameOrDirnameOnly);
}

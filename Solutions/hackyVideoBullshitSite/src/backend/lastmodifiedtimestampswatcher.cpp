#include "lastmodifiedtimestampswatcher.h"

#include <QTimer>
#include <QMapIterator>
#include <QScopedPointer>
#include <QFileSystemWatcher>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QHashIterator>

//git push -> post-update -> git clone/archive -> symlinkSwap -> deleteOldLastModifiedJustToTriggerQfsWatcher (also delete rest of shit) -> [re-]resolveAndWatchSymlink/.lastModified

#define D3FAULT_LAUNCH_BOOK_HACK_SEX_SPITTER_OUTTER_KTHXBAI 0 //completely off-topic hack for book generating :). so much timestamp merging etc going on i don't feel like coding twice

#ifdef D3FAULT_LAUNCH_BOOK_HACK_SEX_SPITTER_OUTTER_KTHXBAI
#include <QTextStream>
#include <QDateTime>
#include <QCryptographicHash>
#include <QSet>
#include <iostream>
using namespace std;
#endif

LastModifiedTimestampsWatcher::LastModifiedTimestampsWatcher(QObject *parent)
    : QObject(parent)
    , m_LastModifiedTimestampsFilesWatcher(0)
    , m_CurrentTimestampsAndPathsAtomicPointer(0) //initialize to value zero (which coincidentally is a null/0 ptr address), not null/0 ptr address [of the member itself]
    , m_DeleteInFiveMinsTimer(new QTimer(this))
    , m_TimestampsAndPathsQueuedForDelete(0)
{
    m_DeleteInFiveMinsTimer->setSingleShot(true);
    m_DeleteInFiveMinsTimer->setInterval(5*(1000*60));
    connect(m_DeleteInFiveMinsTimer, SIGNAL(timeout()), this, SLOT(handleDeleteInFiveMinsTimerTimedOut()));
}
QAtomicPointer<LastModifiedTimestampsAndPaths> *LastModifiedTimestampsWatcher::getTimestampsAndPathsAtomicPointer()
{
    return &m_CurrentTimestampsAndPathsAtomicPointer;
}
LastModifiedTimestampsWatcher::~LastModifiedTimestampsWatcher()
{
    if(m_TimestampsAndPathsQueuedForDelete)
    {
        while(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        {
            deleteOneTimestampAndPathQueuedForDelete();
        }
        delete m_TimestampsAndPathsQueuedForDelete;
    }
    LastModifiedTimestampsAndPaths *currentTimestampsAndPathsMaybe = m_CurrentTimestampsAndPathsAtomicPointer.fetchAndStoreOrdered(0);
    if(currentTimestampsAndPathsMaybe)
    {
        delete currentTimestampsAndPathsMaybe;
    }
    if(m_LastModifiedTimestampsFilesWatcher)
    {
        delete m_LastModifiedTimestampsFilesWatcher;
    }
}
bool LastModifiedTimestampsWatcher::addAndReadLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile)
{
    if(!QFile::exists(lastModifiedTimestampsFile))
    {
        emit e("TOTAL SYSTEM FAILURE: you did not use 'move [symlink] atomics' for last modified timestamp file: " + lastModifiedTimestampsFile); //TODOoptional: error out etcz
        return false;
    }
    m_LastModifiedTimestampsFilesWatcher->addPath(lastModifiedTimestampsFile);

    LastModifiedTimestampsSorter lastModifiedTimestampsSorter(this);
    int totalPathsCount = 0;
    SortedMapOfListsOfPathsPointerType *oldUncombinedMap = m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.value(lastModifiedTimestampsFile);
    delete oldUncombinedMap;
    m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.insert(lastModifiedTimestampsFile, lastModifiedTimestampsSorter.sortLastModifiedTimestamps_ButDontSortPaths(lastModifiedTimestampsFile, &totalPathsCount, getPathPrefixForThisLastModifiedTimestampsFile(lastModifiedTimestampsFile))); //dont sort the paths because it would be a waste of time to sort them before merging with the others

    return true; //TODOoptional: above two ops can/should return false if fail
}
QString LastModifiedTimestampsWatcher::getPathPrefixForThisLastModifiedTimestampsFile(const QString &lastModifiedTimestampsFile)
{
    //TODOoptimization: *might* be worth it to cache this parsing... but eh fuck it pretty cheap anyways...
    QFileInfo lastModifiedTimestampsFilenameChopperOffer(lastModifiedTimestampsFile);
    QString absolutePath = appendSlashIfNeeded(lastModifiedTimestampsFilenameChopperOffer.absolutePath());
    absolutePath.remove(0, m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList.length());
    return absolutePath;
}
void LastModifiedTimestampsWatcher::combineAndPublishLastModifiedTimestampsFiles()
{
    //combine with other maps before converting to flat list and paths index into flat list
    QScopedPointer<SortedMapOfListsOfPathsPointerType> combinedMap(new SortedMapOfListsOfPathsPointerType());
    QHashIterator<QString, SortedMapOfListsOfPathsPointerType*> uncombinedMapsIterator(m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps);
    while(uncombinedMapsIterator.hasNext())
    {
        uncombinedMapsIterator.next();
        QMapIterator<long long, QList<std::string>* > mapCurrentlyBeingInsertedIntoCombinedMapIterator(*uncombinedMapsIterator.value());
        while(mapCurrentlyBeingInsertedIntoCombinedMapIterator.hasNext())
        {
            mapCurrentlyBeingInsertedIntoCombinedMapIterator.next();
            QList<std::string>* maybeExistingPathsForTimestamp = combinedMap->value(mapCurrentlyBeingInsertedIntoCombinedMapIterator.key(), 0);
            if(maybeExistingPathsForTimestamp)
            {
                //key already exists in combined map
                maybeExistingPathsForTimestamp->append(*mapCurrentlyBeingInsertedIntoCombinedMapIterator.value()); //TODOreq: wtf is the memory shit of this? maybe just fuck it and let valgrind tell me :). starting to think i should have used qstring since implicitly (we now have two copies :-/) shared and simplified...
            }
            else
            {
                //key does not yet exist in combined map
                combinedMap->insert(mapCurrentlyBeingInsertedIntoCombinedMapIterator.key(), mapCurrentlyBeingInsertedIntoCombinedMapIterator.value()); //TODOreq: ditto about memory confusion
            }
        }
    }

    //now we convert it to two formats we want
    QList<TimestampAndPath*> *sortedTimestampAndPathFlatList = new QList<TimestampAndPath*>();
    //sortedTimestampAndPathFlatList->reserve(totalPathsCount); //TODOoptimization: maybe re-use these values again
    int currentIndexIntoSortedPathsFlatList = -1;
    PathsIndexIntoFlatListHashType *pathsIndexIntoFlatListHash = new PathsIndexIntoFlatListHashType();
    //pathsIndexIntoFlatListHash->reserve(totalPathsCount);


    //TO DOnereq: skip over directory entries (which may be the only entry for that timestamp), honestly i don't think they're even that necessary (although creating empty directories to indicate a project i want to start is kinda handy), maybe i should just stop keeping track of them instead...

    QMapIterator<long long, QList<std::string>* > combineddMapIterator(*combinedMap.data());
    while(combineddMapIterator.hasNext())
    {
        combineddMapIterator.next();
        QList<std::string> *allPathsWithThisTimestamp = combineddMapIterator.value();

        //a step we skipped earlier was that each of the paths (values of map) weren't being sorted. now that they're all combined, we sort them
        std::sort(allPathsWithThisTimestamp->begin(), allPathsWithThisTimestamp->end());

        QListIterator<std::string> allPathsWithThisTimestampIterator(*allPathsWithThisTimestamp);
        while(allPathsWithThisTimestampIterator.hasNext())
        {
            std::string currentPath = "/" /* convert to internal path, which starts with a slash */ + allPathsWithThisTimestampIterator.next();
            const QString &currentPathQString = QString::fromStdString(currentPath);
            if(currentPathQString.endsWith("/") /*we don't want directories*/ || currentPathQString == "/"/*it was an empty path (error)*/)
                continue;

            TimestampAndPath *timestampAndPath = new TimestampAndPath(combineddMapIterator.key(), currentPath);
            sortedTimestampAndPathFlatList->append(timestampAndPath);
            (*pathsIndexIntoFlatListHash)[currentPath] = ++currentIndexIntoSortedPathsFlatList; //funny, if i use QString then the string is doubled in size (16-bit vs. 8-bit), and yet QString also saves me half the space because of implicit sharing xD...
        }
    }

#if D3FAULT_LAUNCH_BOOK_HACK_SEX_SPITTER_OUTTER_KTHXBAI
    {
        QSet<QByteArray> fileContentsDeDuplicater; //sha1 contents, only ever add the body of a given hash to book once (first occurance)
        QString baseDir = "/home/user/hvbs/web/view";
        QFile bookContentsFile("/run/shm/book.contents.txt");
        QFile dreamsContentsFile("/run/shm/book.dreams.txt");
        QList<QString> wantedExtensionsForBook;
        wantedExtensionsForBook.append(".txt");
        wantedExtensionsForBook.append(".cpp");
        wantedExtensionsForBook.append(".h");
        wantedExtensionsForBook.append(".c");
        wantedExtensionsForBook.append(".cxx");
        wantedExtensionsForBook.append(".cs");
        wantedExtensionsForBook.append(".php");
        wantedExtensionsForBook.append(".sh");
        //java? nahh since it was only ever stupid school projects...
        if(!bookContentsFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text) || !dreamsContentsFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            cout << "error opening book contents file for writing";
        else
        {
            QTextStream bookContentsStream(&bookContentsFile);
            QTextStream dreamsContentsFileStream(&dreamsContentsFile);
            bool firstEntry = true;
            Q_FOREACH(TimestampAndPath *currentTimestampAndPath, *sortedTimestampAndPathFlatList)
            {
                QString pathQ = QString::fromStdString(currentTimestampAndPath->Path);
                bool wantFileForBook = false;
                Q_FOREACH(const QString &currentWantedExtensionForBook, wantedExtensionsForBook)
                {
                    if(pathQ.endsWith(currentWantedExtensionForBook))
                    {
                        wantFileForBook = true;
                        break;
                    }
                }
                if(!wantFileForBook)
                    continue;
                /*if(pathQ.contains("/Projects/") && !pathQ.contains("/Projects/Ideas/")) //don't want various "design" docs, but do want ideas
                    continue;*/
                if(pathQ.contains("/VariousTreeingsDuringHugeArchivePurge/") || pathQ.contains("oldUnversionedArchive/Lists/") || pathQ.contains("oldUnversionedArchive/Media Lists/") || pathQ.contains("downloadMoviesFreeLegallyTutorial") || pathQ.contains("/working/Documents/oldGitLog") || pathQ.contains("/lastFmExports/") || pathQ.contains("/sxml/testxml.txt") || pathQ.contains("/Documents/dmca/") || pathQ.contains("/Voice/property/") || pathQ.contains("preamble.wip.2.txt") || pathQ.contains("license.dpl.txt") || pathQ == "copyright.txt" || pathQ.contains("/copyright.txt"))
                    continue;
                QDateTime timeD = QDateTime::fromMSecsSinceEpoch(currentTimestampAndPath->Timestamp*1000);
                QFile currentFileForBook(baseDir + pathQ);
                if(!currentFileForBook.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    cout << "failed to open: " << baseDir.toStdString() << currentTimestampAndPath->Path;
                    break;
                }
                else
                {
                    QTextStream currentFileForBookStream(&currentFileForBook);
                    QString currentFileContents = currentFileForBookStream.readAll();
                    //custom: "if(char-is-ascii-ascii) { emit empty-str }" replacer
                    QString rebuiltWithNoUnicode;
                    int fileContentsLen = currentFileContents.length();
                    for(int i = 0; i < fileContentsLen; ++i)
                    {
                        QChar currentChar = currentFileContents.at(i);
                        if(currentChar.unicode() > 127)
                            continue;
                        rebuiltWithNoUnicode.append(currentChar);
                    }
                    currentFileContents = rebuiltWithNoUnicode;
                    if(currentFileContents.trimmed().isEmpty())
                        continue;

                    //Begin last-minute dedupe of contents based on sha1sum of contents
                    QByteArray currentFileContentsAsByteArrayForDedupe = currentFileContents.toUtf8();
                    QByteArray currentFileContentsSignature = QCryptographicHash::hash(currentFileContentsAsByteArrayForDedupe, QCryptographicHash::Sha1);
                    if(fileContentsDeDuplicater.contains(currentFileContentsSignature))
                        continue;
                    fileContentsDeDuplicater.insert(currentFileContentsSignature);
                    //End last-minute dedupe code

                    if(firstEntry)
                    {
                        firstEntry = false;
                    }
                    else
                    {
                        bookContentsStream << " ### ";
                    }

                    //remove the first slash, since it is always there (it was added to make internal path for wt)
                    if(pathQ.startsWith("/")) //just in case
                    {
                        pathQ = pathQ.right(pathQ.length()-1);
                    }

                    bookContentsStream << timeD.toString("yy-MM-d_hh:mm:ss") << " ### " << pathQ << " ### ";
                    if(pathQ.contains("/minddump/dreams/"))
                    {
                        //dreams only (retain formatting)
                        dreamsContentsFileStream << timeD.toString("yy-MM-d_hh:mm:ss") << " - " << pathQ << endl << currentFileContents << endl << endl;
                    }

                    //everything (compact formatting)

                    //i see huge gaps of whitespace, so maybe readAll isn't as smart as readLine. ez fix anyways
                    //convert all \r\n into \n until no more \r\n seen
                    QString origString;
                    do
                    {
                        origString = currentFileContents;
                        currentFileContents = currentFileContents.replace("\r\n", "\n");
                    }while(currentFileContents != origString);

                    //compact all multiple \n into a single \n. i wasn't planning on doing this (see below's TODOoptional), but am seeing weird paragraph breaks that are costing me tons of empty space on pages (usually over half the page is blank)
                    do
                    {
                        origString = currentFileContents;
                        currentFileContents = currentFileContents.replace("\n\n", "\n");
                    }while(currentFileContents != origString);

                    //convert all multiple \t into just one \t until no more multiple \t seen

                    //convert all newlines into '\n' until no more newlines seen -- TODOoptional: compact multiple newlines into something like "\n{15}", but only if that would give us a shorter string overall
                    do
                    {
                        origString = currentFileContents;
                        currentFileContents = currentFileContents.replace("\n", "\\n");
                    }while(currentFileContents != origString);

                    //convert all multiple \t into just one \t until no more multiple \t seen
                    do
                    {
                        origString = currentFileContents;
                        currentFileContents = currentFileContents.replace("\t\t", "\t");
                    }while(currentFileContents != origString);
                    currentFileContents.replace("\t", "\\t");

                    //convert all multiple spaces into just one space until no more multiple spaces seen
                    do
                    {
                        origString = currentFileContents;
                        currentFileContents = currentFileContents.replace("  ", " ");
                    }while(currentFileContents != origString);

                    bookContentsStream << currentFileContents;
                }
            }
        }
    }
    cout << "book wroted" << endl;
#endif

    LastModifiedTimestampsAndPaths *newTimestampsAndPaths = new LastModifiedTimestampsAndPaths(sortedTimestampAndPathFlatList, pathsIndexIntoFlatListHash);
    if(sortedTimestampAndPathFlatList->isEmpty())
    {
        delete newTimestampsAndPaths; //chances are the list that they already have is better than an empty list...
        return;
    }
    LastModifiedTimestampsAndPaths *oldTimestampsAndPaths = m_CurrentTimestampsAndPathsAtomicPointer.fetchAndStoreOrdered(newTimestampsAndPaths); //TODOoptimization: might not need ordered, idfk
    if(!oldTimestampsAndPaths)
        return; //first time, nothing to queue for delete
    if(m_TimestampsAndPathsQueuedForDelete->isEmpty()) //don't double start (restart)
        m_DeleteInFiveMinsTimer->start();
    m_TimestampsAndPathsQueuedForDelete->enqueue(oldTimestampsAndPaths);
}
void LastModifiedTimestampsWatcher::deleteOneTimestampAndPathQueuedForDelete()
{
    LastModifiedTimestampsAndPaths *currentToDelete = m_TimestampsAndPathsQueuedForDelete->dequeue();
    delete currentToDelete;
}
//TO DOnereq: we need to keep the unresolved symlinks handy for re-adding. the key entry in QHash should probably be that symlink (since otherwise it'd be changing). HOWEVER, when qfsw tells us that a file is deleted, it is no longer the symlink. we resolved it! so i need to keep track of symlinks and resolvedz... ffff. wait a tick, i don't need to resolve it in the first place and then it's no longer a problem... why was i anyways? i guess it was just to help me with understanding...
void LastModifiedTimestampsWatcher::startWatchingLastModifiedTimestampsFiles(const QString &absolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList, const QStringList &lastModifiedTimestampsFiles)
{
    m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList = appendSlashIfNeeded(absolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList);
    if(m_LastModifiedTimestampsFilesWatcher)
        delete m_LastModifiedTimestampsFilesWatcher;
    m_LastModifiedTimestampsFilesWatcher = new QFileSystemWatcher(this);
    connect(m_LastModifiedTimestampsFilesWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleLastModifiedTimestampsChanged(QString)));
    if(!m_TimestampsAndPathsQueuedForDelete)
    {
        m_TimestampsAndPathsQueuedForDelete = new QQueue<LastModifiedTimestampsAndPaths*>();
    }
    foreach(const QString &lastModifiedTimestampsFile, lastModifiedTimestampsFiles)
    {
        if(!lastModifiedTimestampsFile.startsWith(m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList))
        {
            emit e("error: '" + lastModifiedTimestampsFile + "' is not in a sub-folder of: '" + m_AbsolutePathToBaseViewDirSoTheFoldersInBetweenThereAndLastModifiedTimestampsFilesCanBeUsedAsPrefixInCombinedSortedList + "'");
            return; //TODOoptional: this and the below return should return false, and that false should be emitted back to caller and exit cleanly etc (whereas now hvbs just.. err... stops mid-startup without the startedWatchingLastModifiedTimestampsFile signal..)
        }
        m_LastModifiedTimestampsFilesAndPreCombinedSemiSortedTimestampsAndPathsMaps.insert(lastModifiedTimestampsFile, new SortedMapOfListsOfPathsPointerType());
        if(!addAndReadLastModifiedTimestampsFile(lastModifiedTimestampsFile)) //populate at startup
            return;
    }
    combineAndPublishLastModifiedTimestampsFiles();
    emit startedWatchingLastModifiedTimestampsFile();
}
void LastModifiedTimestampsWatcher::handleLastModifiedTimestampsChanged(const QString &lastModifiedTimestampsFileThatChanged)
{
    if(!addAndReadLastModifiedTimestampsFile(lastModifiedTimestampsFileThatChanged))
        return;
    combineAndPublishLastModifiedTimestampsFiles(); //TODOoptimization: maybe another one needs reading (maybe they all changed at, or around, the same time)... so it would be an optimization to not combine yet if that were the case and, more importantly, it is detectable. perhaps clever queued combine invocation (0 length timer) hackery with some kind of boolean flag that somehow cancels it (stop the timer?). not sure if canceling a timer that has "already fired" will stop the slot... could test that though..
}
void LastModifiedTimestampsWatcher::handleDeleteInFiveMinsTimerTimedOut()
{
    if(m_TimestampsAndPathsQueuedForDelete->isEmpty())
        return;

    deleteOneTimestampAndPathQueuedForDelete();

    if(!m_TimestampsAndPathsQueuedForDelete->isEmpty())
        m_DeleteInFiveMinsTimer->start();
}

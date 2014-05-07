#include "videosegmentsimporterfolderwatcher.h"

#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QListIterator>

VideoSegmentsImporterFolderWatcher::VideoSegmentsImporterFolderWatcher(QObject *parent) :
    QObject(parent)
  , m_DirectoryWatcher(new QFileSystemWatcher(this))
  //, m_CurrentYearFolder(-1)
  //, m_CurrentDayOfYearFolder(-1)
  , m_SftpUploaderAndRenamerQueue(0)
{
    connect(m_DirectoryWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged()));
}
bool VideoSegmentsImporterFolderWatcher::jitEnsureFolderExists(const QString &absoluteFolderPathToMaybeJitCreate)
{
    if(!QFile::exists(absoluteFolderPathToMaybeJitCreate))
    {
        QDir blahDir(absoluteFolderPathToMaybeJitCreate);
        if(!blahDir.mkpath(absoluteFolderPathToMaybeJitCreate))
        {
            emit e("failed to jit create folder: " + absoluteFolderPathToMaybeJitCreate);
            return false; //don't stop, but do tell
        }
    }
    return true;
}
void VideoSegmentsImporterFolderWatcher::beginStoppingVideoNeighborPropagation(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState)
{
    //uncomment next line if you don't want files received after Q or QQ is pressed to MAYBE have to finish before considered finished (MAYBE because if the upload bandwidth is fast enough and the queue is empty right when Q or QQ is pressed, it will still disregard newly received ones). Does not affect QQQ. If uncommenting, probably need to find a place to reconnect as well (but idk maybe not)...
    //disconnect(m_DirectoryWatcher, SIGNAL(directoryChanged(QString)));

    emit sftpUploaderAndRenamerQueueStateChangedRequested(newSftpUploaderAndRenamerQueueState);
}
void VideoSegmentsImporterFolderWatcher::initializeAndStart(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo, const QString &neighborPropagationRemoteDestinationToUploadTo, const QString &neighborPropagationRemoteDestinationToMoveTo, const QString &neighborPropagationUserHostPathComboSftpArg, const QString &sftpProcessPath)
{
    m_VideoSegmentsImporterFolderToWatchWithSlashAppended = appendSlashIfNeeded(videoSegmentsImporterFolderToWatch);
    m_VideoSegmentsImporterFolderScratchSpacePathWithSlashAppended = appendSlashIfNeeded(videoSegmentsImporterFolderScratchSpace);
    //m_VideoSegmentsImporterFolderScratchSpace.setPath(videoSegmentsImporterFolderScratchSpace); //because using folder to watch would trigger more directory changed stuffs (already does when leaving, but that is fine. creating folders in it though would create problems), and the folders need to already have a file in them before they are put in videoSegmentsImporterFolderToMoveTo
    m_VideoSegmentsImporterFolderToMoveToWithSlashAppended = appendSlashIfNeeded(videoSegmentsImporterFolderToMoveTo);

    if(m_SftpUploaderAndRenamerQueue)
        delete m_SftpUploaderAndRenamerQueue;
    m_SftpUploaderAndRenamerQueue = new SftpUploaderAndRenamerQueue(this); //the rename is useless in this case (but the move ISN'T), but i'm too lazy to un-over-engineer it :-P
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStarted()), this, SLOT(handleSftpUploaderAndRenamerQueueStarted()));
    connect(this, SIGNAL(tellNeighborPropagationInformationRequested()), m_SftpUploaderAndRenamerQueue, SLOT(tellStatus()));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(statusGenerated(QString)), this, SIGNAL(o(QString)));
    connect(this, SIGNAL(sftpUploaderAndRenamerQueueStateChangedRequested(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum)), m_SftpUploaderAndRenamerQueue, SLOT(changeSftpUploaderAndRenamerQueueState(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStopped()), this, SIGNAL(videoSegmentsImporterFolderWatcherFinishedPropagatingToNeighbors())); //whether waited or killed, shit is done yo

    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "startSftpUploaderAndRenamerQueue", Q_ARG(QString, neighborPropagationRemoteDestinationToUploadTo), Q_ARG(QString, neighborPropagationRemoteDestinationToMoveTo), Q_ARG(QString, neighborPropagationUserHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
}
void VideoSegmentsImporterFolderWatcher::stopCleanlyOnceVideoSegmentNeighborPropagatationFinishes()
{
    beginStoppingVideoNeighborPropagation(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully);
}
void VideoSegmentsImporterFolderWatcher::stopCleanlyOnceVideoSegmentNeighborPropagatationFinishesUnlessDc()
{
    beginStoppingVideoNeighborPropagation(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc);
}
void VideoSegmentsImporterFolderWatcher::stopNow()
{
    beginStoppingVideoNeighborPropagation(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueState_StopNow);
}
//moves file added to watch directory to <year>/<day>/
//creates both year/day folders if needed (moves file into them before moving the folder to the destination (so it's atomic))
void VideoSegmentsImporterFolderWatcher::handleDirectoryChanged()
{
    QDir theDir(m_VideoSegmentsImporterFolderToWatchWithSlashAppended);
    const QStringList &theDirEntryList = theDir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name /* | QDir::Reversed*/); //could sort by time instead, but i'm naming them using unix timestamps so...
    QListIterator<QString> it(theDirEntryList);
    while(it.hasNext())
    {
        const QString &currentEntry = it.next();
        //move entry from watch folder to permanent folder. TODOoptimization: folder-ize videos maybe. isn't there some maximum files in directory limit?
        const QString &filenameMinusExt = currentEntry.left(currentEntry.lastIndexOf("."));
        bool convertOk = false;
        long long filenameMinusExtAsLong = filenameMinusExt.toLongLong(&convertOk);
        if(!convertOk)
        {
            emit e("invalid filename: " + currentEntry);
            continue;
        }
        const QDateTime &newFilenamesDateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(filenameMinusExtAsLong)*1000).toUTC();
        const int newFilenamesYear = newFilenamesDateTime.date().year();
        const int newFilenamesDayOfYear = newFilenamesDateTime.date().dayOfYear();
        const QString &currentEntryAbsoluteFilePath = m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry;

        //these two must be created with the video already in them (so scratch must be used)
        const QString &yearFolderFinalDest = m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(newFilenamesYear) + QDir::separator();
        const QString &dayFolderFinalDest = yearFolderFinalDest + QString::number(newFilenamesDayOfYear) + QDir::separator();

        //most common case, day folder already exists
        if(QFile::exists(dayFolderFinalDest))
        {
            const QString &moveToDestinationFilename = dayFolderFinalDest + currentEntry;
            if(!QFile::rename(currentEntryAbsoluteFilePath, moveToDestinationFilename))
            {
                emit e("failed to move '" + currentEntryAbsoluteFilePath + "' to '" + moveToDestinationFilename + "'");
                continue; //don't stop, but do tell
            }

            //enqueue for propagation to sibling
            SftpUploaderAndRenamerQueueTimestampAndFilenameType enqueuForUploadType;
            enqueuForUploadType.first = filenameMinusExt;
            enqueuForUploadType.second = moveToDestinationFilename;
            QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, enqueuForUploadType));

            continue;
        }

        //second most common case, year folder already exists, day folder does not
        if(QFile::exists(yearFolderFinalDest))
        {
            //day folder needs creating, but video needs to already be in it ("move to atomicity"), so we need to use scratch space
            const QString &dayFolderInScratchSpace = m_VideoSegmentsImporterFolderScratchSpacePathWithSlashAppended + QString::number(newFilenamesYear) + QDir::separator() + QString::number(newFilenamesDayOfYear) + QDir::separator();
            if(!jitEnsureFolderExists(dayFolderInScratchSpace))
                continue;
            const QString &absoluteFilePathInScratchSpace = dayFolderInScratchSpace + currentEntry;
            //move to scratch
            if(!QFile::rename(currentEntryAbsoluteFilePath, absoluteFilePathInScratchSpace))
            {
                emit e("failed to move '" + currentEntryAbsoluteFilePath + "' to '" + absoluteFilePathInScratchSpace + "'");
                continue; //don't stop, but do tell
            }
            //move to final
            if(!QFile::rename(dayFolderInScratchSpace, dayFolderFinalDest))
            {
                emit e("failed to move '" + dayFolderInScratchSpace + "' to '" + dayFolderFinalDest + "'");
                continue; //don't stop, but do tell
            }

            //enqueue for propagation to sibling
            SftpUploaderAndRenamerQueueTimestampAndFilenameType enqueuForUploadType;
            enqueuForUploadType.first = filenameMinusExt;
            enqueuForUploadType.second = dayFolderFinalDest + currentEntry;
            QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, enqueuForUploadType));

            continue;
        }

        //least common case, year folder does not exist

        const QString &yearFolderInScratchSpace = m_VideoSegmentsImporterFolderScratchSpacePathWithSlashAppended + QString::number(newFilenamesYear) + QDir::separator(); //source of the move/rename
        const QString &dayFolderInScratchSpace = yearFolderInScratchSpace + QString::number(newFilenamesDayOfYear) + QDir::separator();
        if(!jitEnsureFolderExists(dayFolderInScratchSpace))
            continue;
        const QString &absoluteFilePathInScratchSpace = dayFolderInScratchSpace + currentEntry;
        //move to scratch
        if(!QFile::rename(currentEntryAbsoluteFilePath, absoluteFilePathInScratchSpace))
        {
            emit e("failed to move '" + currentEntryAbsoluteFilePath + "' to '" + absoluteFilePathInScratchSpace + "'");
            continue; //don't stop, but do tell
        }
        //move to final
        if(!QFile::rename(yearFolderInScratchSpace, yearFolderFinalDest))
        {
            emit e("failed to move '" + yearFolderInScratchSpace + "' to '" + yearFolderFinalDest + "'");
            continue; //don't stop, but do tell
        }

        //enqueue for propagation to sibling
        SftpUploaderAndRenamerQueueTimestampAndFilenameType enqueuForUploadType;
        enqueuForUploadType.first = filenameMinusExt;
        enqueuForUploadType.second = dayFolderFinalDest + currentEntry;
        QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, enqueuForUploadType));

        continue;

#if 0 //depcrecated: overcomplicated and less robust (quit->restarts failed)
        //BUG (solved): creating *second* new day folder does not have a year folder in scratch space, because it was moved to moveTo and we currently don't think one needs to be created if one exists in moveTo already

        /*
         * PSEUDO-CODE of following
         *
         * on(new year) -> delete last year scratch space (empty folder) -> make new year folder in scratch space -> make new day folder in scratch space -> move to new day folder -> since new year, move new year to moveTo and remake new year
         * on(new day) -> make new day folder in scratch space -> move to new day folder -> since new day, move new day to moveTo/year
         * on(notNewYearOrNewDay) -> move to moveTo/year/day
         */

        //on(new year)
        if((m_CurrentYearFolder == -1) || (newFilenamesYear != m_CurrentYearFolder))
        {
            //delete last year's folder in scratch space if it exists. should be empty and will fail if it isn't
            if(m_CurrentYearFolder != -1)
                m_VideoSegmentsImporterFolderScratchSpace.remove(QString::number(m_CurrentYearFolder));
            m_CurrentYearFolder = newFilenamesYear;
            m_CurrentDayOfYearFolder = newFilenamesDayOfYear;
            //make new year folder in scratch space
            const QString &yearFolderAboutToMake = m_VideoSegmentsImporterFolderScratchSpace.absolutePath() + QDir::separator() + QString::number(m_CurrentYearFolder) + QDir::separator();
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkpath(yearFolderAboutToMake))
            {
                emit e("failed to make year directory: '" + yearFolderAboutToMake + "'");
                continue; //don't stop, but do tell
            }
            //make new day folder in scratch space
            QString currentDayOfYearFolderString = QString::number(m_CurrentDayOfYearFolder);
            const QString &dayFolderAboutToMake = yearFolderAboutToMake + currentDayOfYearFolderString + QDir::separator();
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkpath(dayFolderAboutToMake))
            {
                emit e("failed to make day directory: '" + dayFolderAboutToMake + "'");
                continue; //don't stop, but do tell
            }
            //move to new day folder
            const QString &newFileScratchSpaceAbsoluteFilePath = dayFolderAboutToMake + currentEntry;
            if(!QFile::rename(currentEntryAbsoluteFilePath, newFileScratchSpaceAbsoluteFilePath))
            {
                emit e("failed to move: '" + currentEntryAbsoluteFilePath + "' to '" + newFileScratchSpaceAbsoluteFilePath + "'");
                continue; //don't stop, but do tell
            }
            //move new year to moveTo
            const QString &moveToDestinationYearFolder = m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(m_CurrentYearFolder);
            if(!QFile::rename(yearFolderAboutToMake, moveToDestinationYearFolder)) //jitCreateFolder not necessary since moving to root of final destination
            {
                emit e("failed to move '" + yearFolderAboutToMake + "' to '" + moveToDestinationYearFolder + "'");
                continue; //don't stop, but do tell
            }

            //enqueue for propagation to sibling
            SftpUploaderAndRenamerQueueTimestampAndFilenameType enqueuForUploadType;
            enqueuForUploadType.first = filenameMinusExt;
            enqueuForUploadType.second = moveToDestinationYearFolder + QDir::separator() + currentDayOfYearFolderString + QDir::separator() + currentEntry;
            QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, enqueuForUploadType));

            //remake new year (since it disappeared with above move, yet we still need it for new days)
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkpath(yearFolderAboutToMake))
            {
                emit e("failed to re-make year directory: '" + yearFolderAboutToMake + "'");
                continue; //don't stop, but do tell
            }
            continue;
        }
        //on(new day)
        if((m_CurrentDayOfYearFolder == -1) || (newFilenamesDayOfYear != m_CurrentDayOfYearFolder))
        {
            m_CurrentDayOfYearFolder = newFilenamesDayOfYear;
            //make new day folder
            const QString &dayFolderAboutToMake = m_VideoSegmentsImporterFolderScratchSpace.absolutePath() + QDir::separator() + QString::number(m_CurrentYearFolder) + QDir::separator() + QString::number(m_CurrentDayOfYearFolder) + QDir::separator();
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkpath(dayFolderAboutToMake))
            {
                emit e("failed to make day directory: '" + dayFolderAboutToMake + "'");
                continue; //don't stop, but do tell
            }
            //move to new day folder
            const QString &newFileScratchSpaceAbsoluteFilePath = dayFolderAboutToMake + currentEntry;
            if(!QFile::rename(currentEntryAbsoluteFilePath, newFileScratchSpaceAbsoluteFilePath))
            {
                emit e("failed to move: '" + currentEntryAbsoluteFilePath + "' to '" + newFileScratchSpaceAbsoluteFilePath + "'");
                continue; //don't stop, but do tell
            }
            //move new day to moveTo/year
            const QString &moveToDestinationYearFolder = m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(m_CurrentYearFolder) + QDir::separator();
            if(!jitEnsureFolderExists(moveToDestinationYearFolder))
                continue;
            const QString &moveToDestinationDayFolder = moveToDestinationYearFolder + QString::number(m_CurrentDayOfYearFolder);
            if(!QFile::rename(dayFolderAboutToMake, moveToDestinationDayFolder))
            {
                emit e("failed to move '" + dayFolderAboutToMake + "' to '" + moveToDestinationDayFolder + "'");
                continue; //don't stop, but do tell
            }

            //enqueue for propagation to sibling
            SftpUploaderAndRenamerQueueTimestampAndFilenameType enqueuForUploadType;
            enqueuForUploadType.first = filenameMinusExt;
            enqueuForUploadType.second = moveToDestinationDayFolder + QDir::separator() + currentEntry;
            QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, enqueuForUploadType));

            //no remake of day folder that just disappeared necessary, because the rest of this days entries will skip scratch space altogether
            continue;
        }
        //on(not new day or new year)  -- COMMON CASE
        //move to moveTo/year/day

        const QString &moveToDestinaationFolder = m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(m_CurrentYearFolder) + QDir::separator() + QString::number(m_CurrentDayOfYearFolder) + QDir::separator();
        if(!jitEnsureFolderExists(moveToDestinaationFolder))
            continue;
        const QString &moveToDestinationFilename = moveToDestinaationFolder + currentEntry;

        if(!QFile::rename(currentEntryAbsoluteFilePath, moveToDestinationFilename))
        {
            emit e("failed to move '" + currentEntryAbsoluteFilePath + "' to '" + moveToDestinationFilename + "'");
            continue; //don't stop, but do tell
        }

        //enqueue for propagation to sibling
        SftpUploaderAndRenamerQueueTimestampAndFilenameType enqueuForUploadType;
        enqueuForUploadType.first = filenameMinusExt;
        enqueuForUploadType.second = moveToDestinationFilename;
        QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueueTimestampAndFilenameType, enqueuForUploadType));
#endif
        //TO DOnereq: there are 3 renames/moves above that each need to trigger a "start propagating to sibling server" [via sftp i guess]. whenever a video segment is moved into "VideoSegmentsImporterFolderToMoveTo", it should also be added to a queue to propagate it
        //^also worth noting is that the very "last" server in the daisy chain of that shit (if not using bit-torrent but using sftp) should NOT queue the file, since nobody else needs it
        //^^this needs to be done before i can scale to 2+ servers
    }
}
void VideoSegmentsImporterFolderWatcher::handleSftpUploaderAndRenamerQueueStarted()
{
    QStringList oldDirectories = m_DirectoryWatcher->directories();
    if(!oldDirectories.isEmpty())
        m_DirectoryWatcher->removePaths(m_DirectoryWatcher->directories()); //clear out old stuffz
    if(!m_DirectoryWatcher->addPath(m_VideoSegmentsImporterFolderToWatchWithSlashAppended))
    {
        emit e("VideoSegmentsImporterFolderWatcher: failed to add '" + m_VideoSegmentsImporterFolderToWatchWithSlashAppended + "' to filesystem watcher");
        return;
    }
    emit o("VideoSegmentsImporterFolderWatcher started");

    //check for any segments that were uploaded while this app wasn't running (if ffmpeg segment uploader was started first). this isn't a big deal, just an optimization really. it would still handle all the old ones once a new one is seen
    handleDirectoryChanged(); //comment out this line if you don't want hvbs to "catch up" right at startup (it will instead catch up when next segment is moved to watched folder)
}

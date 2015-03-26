#include "videosegmentsimporterfolderwatcher.h"

#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QListIterator>

VideoSegmentsImporterFolderWatcher::VideoSegmentsImporterFolderWatcher(QObject *parent) :
    QObject(parent)
  , m_DirectoryWatcher(new QFileSystemWatcher(this))
  , m_PropagateToNeighbor(false)
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
QString VideoSegmentsImporterFolderWatcher::ensureMoveWontOverwrite_ByAddingSecondsIfItWouldHave(const QString &folderToMoveTo_WithSlashAppended, long long desiredFilename)
{
    QString ret;
    do
    {
        ret = folderToMoveTo_WithSlashAppended + QString::number(desiredFilename) + ".ogg";
        ++desiredFilename;
    }while(QFile::exists(ret));
    return ret;
}
void VideoSegmentsImporterFolderWatcher::maybePropagateToNeighbor(const QString &timestampUsedInRename, const QString &localFilenameToPropagate)
{
    if(!m_PropagateToNeighbor)
        return;

    SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type enqueuForUploadType;
    enqueuForUploadType.first = timestampUsedInRename + ".ogg";
    enqueuForUploadType.second = localFilenameToPropagate;
    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "enqueueFileForUploadAndRename", Q_ARG(SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type, enqueuForUploadType));
}
void VideoSegmentsImporterFolderWatcher::beginStoppingVideoNeighborPropagation(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState)
{
    if(!m_PropagateToNeighbor)
    {
        emit videoSegmentsImporterFolderWatcherFinishedPropagatingToNeighbors(); //signal name isn't exactly accurate here, but whatever
        return;
    }

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
    {
        delete m_SftpUploaderAndRenamerQueue;
        m_SftpUploaderAndRenamerQueue = 0;
    }

    m_PropagateToNeighbor = (neighborPropagationUserHostPathComboSftpArg == VideoSegmentsImporterFolderWatcher_DONT_PROPAGATE_TO_NEIGHBOR ? false : true);

    if(m_PropagateToNeighbor)
    {
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
    else
    {
        handleSftpUploaderAndRenamerQueueStarted();
    }
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
    const QStringList &theDirEntryList = theDir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Time | QDir::Reversed);
    QListIterator<QString> it(theDirEntryList);
    while(it.hasNext())
    {
        const QString &currentEntry = it.next();
        //move entry from watch folder to permanent folder. TODOoptimization: folder-ize videos maybe. isn't there some maximum files in directory limit?
        const QString &filenameMinusExt_AkaUnixTimestamp = currentEntry.left(currentEntry.lastIndexOf("."));
        bool convertOk = false;
        long long filenameMinusExtAsLong_AkaUnixTimestamp = filenameMinusExt_AkaUnixTimestamp.toLongLong(&convertOk);
        if(!convertOk)
        {
            emit e("invalid filename: " + currentEntry);
            continue;
        }
        const QDateTime &newFilenamesDateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(filenameMinusExtAsLong_AkaUnixTimestamp)*1000).toUTC();
        const int newFilenamesYear = newFilenamesDateTime.date().year();
        const int newFilenamesDayOfYear = newFilenamesDateTime.date().dayOfYear();
        const QString &currentEntryAbsoluteFilePath = m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry;

        //these two must be created with the video already in them (so scratch must be used)
        const QString &yearFolderFinalDest = m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(newFilenamesYear) + QDir::separator();
        const QString &dayFolderFinalDest_WithSlashAppended = yearFolderFinalDest + QString::number(newFilenamesDayOfYear) + QDir::separator();

        //most common case, day folder already exists
        if(QFile::exists(dayFolderFinalDest_WithSlashAppended))
        {
            const QString &moveToDestinationFilename = ensureMoveWontOverwrite_ByAddingSecondsIfItWouldHave(dayFolderFinalDest_WithSlashAppended, filenameMinusExtAsLong_AkaUnixTimestamp); //this is the only rename that needs this check, because the other ones below are to brand new folders (obviously empty). the reason for this check: my morning (hat) script is now using this mechanism, so there's a decent (1/3) chance of filename collision if I'm wearing the hat while using my comp (and I probably won't, but might). this is an ugly as fuck hack, but it's better than silently overwriting :-P (oh turns out rename fails instead of overwriting... but still the mechanism is already coded this way so fuggit (the files would just stay in 'watched' forever in that case lmfao)
            if(!QFile::rename(currentEntryAbsoluteFilePath, moveToDestinationFilename))
            {
                emit e("failed to move '" + currentEntryAbsoluteFilePath + "' to '" + moveToDestinationFilename + "'");
                continue; //don't stop, but do tell
            }

            //enqueue for propagation to sibling
            maybePropagateToNeighbor(filenameMinusExt_AkaUnixTimestamp, moveToDestinationFilename);
            continue;
        }

        //second most common case, year folder already exists, day folder does not
        if(QFile::exists(yearFolderFinalDest))
        {
            //day folder needs creating, but video needs to already be in it ("move to atomicity"), so we need to use scratch space
            const QString &dayFolderInScratchSpace_WithSlashAppended = m_VideoSegmentsImporterFolderScratchSpacePathWithSlashAppended + QString::number(newFilenamesYear) + QDir::separator() + QString::number(newFilenamesDayOfYear) + QDir::separator();
            if(!jitEnsureFolderExists(dayFolderInScratchSpace_WithSlashAppended))
                continue;
            const QString &absoluteFilePathInScratchSpace = dayFolderInScratchSpace_WithSlashAppended + currentEntry;
            //move to scratch
            if(!QFile::rename(currentEntryAbsoluteFilePath, absoluteFilePathInScratchSpace))
            {
                emit e("failed to move '" + currentEntryAbsoluteFilePath + "' to '" + absoluteFilePathInScratchSpace + "'");
                continue; //don't stop, but do tell
            }
            //move to final
            if(!QFile::rename(dayFolderInScratchSpace_WithSlashAppended, dayFolderFinalDest_WithSlashAppended))
            {
                emit e("failed to move '" + dayFolderInScratchSpace_WithSlashAppended + "' to '" + dayFolderFinalDest_WithSlashAppended + "'");
                continue; //don't stop, but do tell
            }

            //enqueue for propagation to sibling
            maybePropagateToNeighbor(filenameMinusExt_AkaUnixTimestamp, dayFolderFinalDest_WithSlashAppended + currentEntry);
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
        maybePropagateToNeighbor(filenameMinusExt_AkaUnixTimestamp, dayFolderFinalDest_WithSlashAppended + currentEntry);
        continue;
    }
}
void VideoSegmentsImporterFolderWatcher::handleSftpUploaderAndRenamerQueueStarted()
{
    QStringList oldDirectories = m_DirectoryWatcher->directories();
    if(!oldDirectories.isEmpty())
        m_DirectoryWatcher->removePaths(m_DirectoryWatcher->directories()); //clear out old stuffz

//addPath returns void in 4.8, bool in 5.x
#if !(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    if(!
#endif
            m_DirectoryWatcher->addPath(m_VideoSegmentsImporterFolderToWatchWithSlashAppended)
#if !(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
            )
#else
            ;
#endif

#if !(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    {
        emit e("VideoSegmentsImporterFolderWatcher: failed to add '" + m_VideoSegmentsImporterFolderToWatchWithSlashAppended + "' to filesystem watcher");
        return;
    }
#endif

    emit o("VideoSegmentsImporterFolderWatcher started");

    //check for any segments that were uploaded while this app wasn't running (if ffmpeg segment uploader was started first). this isn't a big deal, just an optimization really. it would still handle all the old ones once a new one is seen
    handleDirectoryChanged(); //comment out this line if you don't want hvbs to "catch up" right at startup (it will instead catch up when next segment is moved to watched folder)
}

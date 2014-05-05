#include "videosegmentsimporterfolderwatcher.h"

#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QListIterator>

#include "sftpuploaderandrenamerqueue.h"

VideoSegmentsImporterFolderWatcher::VideoSegmentsImporterFolderWatcher(QObject *parent) :
    QObject(parent)
  , m_DirectoryWatcher(new QFileSystemWatcher(this))
  , m_CurrentYearFolder(-1)
  , m_CurrentDayOfYearFolder(-1)
  , m_SftpUploaderAndRenamerQueue(0)
{
    connect(m_DirectoryWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged(QString)));
}
void VideoSegmentsImporterFolderWatcher::initializeAndStart(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo, const QString &neighborPropagationRemoteDestinationToUploadTo, const QString &neighborPropagationRemoteDestinationToMoveTo, const QString &neighborPropagationUserHostPathComboSftpArg, const QString &sftpProcessPath)
{
    m_VideoSegmentsImporterFolderToWatchWithSlashAppended = appendSlashIfNeeded(videoSegmentsImporterFolderToWatch);
    m_VideoSegmentsImporterFolderScratchSpace.setPath(videoSegmentsImporterFolderScratchSpace); //because using folder to watch would trigger more directory changed stuffs (already does when leaving, but that is fine. creating folders in it though would create problems), and the folders need to already have a file in them before they are put in videoSegmentsImporterFolderToMoveTo
    m_VideoSegmentsImporterFolderToMoveToWithSlashAppended = appendSlashIfNeeded(videoSegmentsImporterFolderToMoveTo);

    if(m_SftpUploaderAndRenamerQueue)
        delete m_SftpUploaderAndRenamerQueue;
    m_SftpUploaderAndRenamerQueue = new SftpUploaderAndRenamerQueue(this); //the rename is useless in this case (but the move ISN'T), but i'm too lazy to un-over-engineer it :-P
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(o(QString)), this, SIGNAL(o(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStarted()), this, SLOT(handleSftpUploaderAndRenamerQueueStarted()));
    //connect(m_SftpUploaderAndRenamerQueue, SIGNAL(statusGenerated(QString)), this, SIGNAL(o(QString)));
    //connect(m_SftpUploaderAndRenamerQueue, SIGNAL(sftpUploaderAndRenamerQueueStopped()), this, SIGNAL(stoppedUploadingFfmpegSegments())); //TODOreq: "q"[uit] should wait for upload queue to finish, which probably means more signal synchronization in HackyVideoBullshitSite

    QMetaObject::invokeMethod(m_SftpUploaderAndRenamerQueue, "startSftpUploaderAndRenamerQueue", Q_ARG(QString, neighborPropagationRemoteDestinationToUploadTo), Q_ARG(QString, neighborPropagationRemoteDestinationToMoveTo), Q_ARG(QString, neighborPropagationUserHostPathComboSftpArg), Q_ARG(QString, sftpProcessPath));
}
//moves file added to watch directory to <year>/<day>/
//creates both year/day folders if needed (moves file into them before moving the folder to the destination (so it's atomic))
void VideoSegmentsImporterFolderWatcher::handleDirectoryChanged(const QString &path)
{
    QDir theDir(path);
    const QStringList &theDirEntryList = theDir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name | QDir::Reversed); //could sort by time instead, but i'm naming them using unix timestamps so...
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
            continue; //silently discard invalid filenames
        }
        const QDateTime &newFilenamesDateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(filenameMinusExtAsLong)*1000);
        const int newFilenamesYear = newFilenamesDateTime.date().year();
        const int newFilenamesDayOfYear = newFilenamesDateTime.date().dayOfYear();

        //BUG (solved): creating *second* new day folder does not have a year folder in scratch space, because it was moved to moveTo and we currently don't think one needs to be created if one exists in moveTo already

        //TODOoptional: possible bug, unsure: if starting up and moveTo/year/day is already made, that doesn't necessarily mean scratchSpace/year is too (i mean it SHOULD, but not necessarily. this would be a rare crash case most likely)
        //^TODOreq: the opposite holds true and is much more likely (regular shutdown -> restart). scratch/year/day is made but moveTo/year/day might not be

        /*
         * PSEUDO-CODE of following
         *
         * on(new year) -> delete last year scratch space (empty folder) -> make new year folder in scratch space -> make new day folder in scratch space -> move to new day folder -> since new year, move new year to moveTo and remake new year
         * on(new day) -> make new day folder in scratch space -> move to new day folder -> since new day, move new day to moveTo/year
         * on(notNewYearOrNewDay) -> move to moveTo/year/day
         */

        const QString &currentEntryAbsoluteFilePath = m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry;

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
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkdir(yearFolderAboutToMake))
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
            if(!QFile::rename(yearFolderAboutToMake, moveToDestinationYearFolder))
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
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkdir(yearFolderAboutToMake))
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
            const QString &moveToDestinationDayFolder = m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(m_CurrentYearFolder) + QDir::separator() + QString::number(m_CurrentDayOfYearFolder);
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
        const QString &moveToDestinationFilename = m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(m_CurrentYearFolder) + QDir::separator() + QString::number(m_CurrentDayOfYearFolder) + QDir::separator() + currentEntry;
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
}

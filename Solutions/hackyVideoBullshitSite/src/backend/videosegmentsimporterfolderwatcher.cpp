#include "videosegmentsimporterfolderwatcher.h"

#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QListIterator>

VideoSegmentsImporterFolderWatcher::VideoSegmentsImporterFolderWatcher(QObject *parent) :
    QObject(parent)
    , m_DirectoryWatcher(0)
{ }
void VideoSegmentsImporterFolderWatcher::initializeAndStart(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo)
{
    m_VideoSegmentsImporterFolderToWatchWithSlashAppended = appendSlashIfNeeded(videoSegmentsImporterFolderToWatch);
    m_VideoSegmentsImporterFolderScratchSpace.setPath(videoSegmentsImporterFolderScratchSpace); //because using folder to watch would trigger more directory changed stuffs (already does when leaving, but that is fine. creating folders in it though would create problems), and the folders need to already have a file in them before they are put in videoSegmentsImporterFolderToMoveTo
    m_VideoSegmentsImporterFolderToMoveToWithSlashAppended = appendSlashIfNeeded(videoSegmentsImporterFolderToMoveTo);
    if(m_DirectoryWatcher)
        delete m_DirectoryWatcher;
    m_DirectoryWatcher = new QFileSystemWatcher(this);
    m_DirectoryWatcher->addPath(videoSegmentsImporterFolderToWatch);
    connect(m_DirectoryWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged(QString)));
    m_CurrentYearFolder = -1;
    m_CurrentDayOfYearFolder = -1;
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
        const QString &filenameMinusExt = currentEntry.left(currentEntry.indexOf("."));
        bool convertOk = false;
        long filenameMinusExtAsLong = filenameMinusExt.toLong(&convertOk);
        if(!convertOk)
        {
            continue; //silently discard invalid filenames
        }
        const QDateTime &newFilenamesDateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(filenameMinusExtAsLong)*1000);
        const int newFilenamesYear = newFilenamesDateTime.date().year();
        bool newYearFolderCreated = false;
        if((m_CurrentYearFolder == -1) || (newFilenamesYear != m_CurrentYearFolder))
        {
            //year folder needs to be created
            newYearFolderCreated = true;
            m_CurrentDayOfYearFolder = -1; //so does day folder
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkdir(QString::number(newFilenamesYear)))
            {
                emit d("failed to make directory: " + m_VideoSegmentsImporterFolderScratchSpace.path() + QDir::separator() + QString::number(newFilenamesYear));
                continue; //don't stop, but do tell
            }
            m_CurrentYearFolder = newFilenamesYear;
        }
        //actually would be trivial and a TODOoptimization to split up by month/week as well, but actually would probably be a better optimization to split the day up further... fuck it though, my main concern is too many files in a directory
        const int newFilenamesDayOfYear = newFilenamesDateTime.date().dayOfYear();
        bool newDayFolderCreated = false;
        if((m_CurrentDayOfYearFolder == -1) || (newFilenamesDayOfYear != m_CurrentDayOfYearFolder))
        {
            //day folder needs to be created
            newDayFolderCreated = true;
            if(!m_VideoSegmentsImporterFolderScratchSpace.mkpath(QString::number(newFilenamesYear) + QDir::separator() + QString::number(newFilenamesDayOfYear)))
            {
                emit d("failed to make directory: " + m_VideoSegmentsImporterFolderScratchSpace.path() + QDir::separator() + QString::number(newFilenamesYear) + QDir::separator() + QString::number(newFilenamesDayOfYear));
                continue; //don't stop, but do tell
            }
            m_CurrentDayOfYearFolder = newFilenamesDayOfYear;
        }
        if(newYearFolderCreated)
        {
            QString scratchFolderName = (m_VideoSegmentsImporterFolderScratchSpace.absolutePath() + QDir::separator() + QString::number(newFilenamesYear) + QDir::separator());
            //move file to scratch space first
            if(!QFile::rename((m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry), (scratchFolderName + currentEntry)))
            {
                emit d("failed to rename '" + (m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry) + "' to '" + (scratchFolderName + currentEntry) + "'");
                continue; //don't stop, but do tell
            }
            QString targetFileDestinationName = (m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(newFilenamesYear));
            if(!QFile::rename(scratchFolderName, targetFileDestinationName))
            {
                emit d("failed to rename '" + scratchFolderName + "' to '" + targetFileDestinationName + "'"); //don't stop, but do tell
            }
            continue;
        }
        else if(newDayFolderCreated)
        {
            QString scratchFolderName = (m_VideoSegmentsImporterFolderScratchSpace.absolutePath() + QDir::separator() + QString::number(newFilenamesYear) + QDir::separator() + QString::number(newFilenamesDayOfYear) + QDir::separator());
            //move file to scratch space first
            if(!QFile::rename((m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry), (scratchFolderName + currentEntry)))
            {
                emit d("failed to rename '" + (m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry) + "' to '" + (scratchFolderName + currentEntry) + "'");
                continue; //don't stop, but do tell
            }
            QString targetFileDestinationName = (m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + QString::number(newFilenamesYear) + QDir::separator() + QString::number(newFilenamesDayOfYear));
            if(!QFile::rename(scratchFolderName, targetFileDestinationName))
            {
                emit d("failed to rename '" + scratchFolderName + "' to '" + targetFileDestinationName + "'"); //don't stop, but do tell
            }
            continue;
        }
        //else: typical use case, year/day folder already created, so just move file to the final destination
        QString targetFileDestinationName = (m_VideoSegmentsImporterFolderToMoveToWithSlashAppended + newFilenamesYear + QDir::separator() + newFilenamesDayOfYear + QDir::separator() + currentEntry);
        if(!QFile::rename((m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry), targetFileDestinationName))
        {
            emit d("failed to rename: '" + (m_VideoSegmentsImporterFolderToWatchWithSlashAppended + currentEntry) + "' to '" + targetFileDestinationName + "'"); //don't stop, but do tell
        }
    }
}

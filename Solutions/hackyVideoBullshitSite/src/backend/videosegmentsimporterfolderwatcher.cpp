#include "videosegmentsimporterfolderwatcher.h"

#include <QFileSystemWatcher>
#include <QDir>
#include <QStringList>
#include <QListIterator>

VideoSegmentsImporterFolderWatcher::VideoSegmentsImporterFolderWatcher(QAtomicInt *sharedVideoSegmentsArrayIndex, QObject *parent) :
    QObject(parent)
    , m_DirectoryWatcher(0)
    , m_SharedVideoSegmentsArrayIndex(sharedVideoSegmentsArrayIndex) //should be noted that this is not "size" or "count" of video segments, it's a 0-based array offset! so size is (the value + 1). it's the reason we initialize it's value to -1
{ }
void VideoSegmentsImporterFolderWatcher::initializeAndStart()
{
    if(m_DirectoryWatcher)
        delete m_DirectoryWatcher;
    m_DirectoryWatcher = new QFileSystemWatcher(this);
    m_DirectoryWatcher->addPath("/run/shm/hackyVideoBullshitSiteVideoImportFolder"); //TODOreq: arg or qsettings
    connect(m_DirectoryWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged()));
}
void VideoSegmentsImporterFolderWatcher::handleDirectoryChanged(const QString &path)
{
    QDir theDir(path);
    const QStringList &theDirEntryList = theDir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name); //could sort by time instead, but i'm naming them using unix timestamps so...
    QListIterator<QString> it(theDirEntryList);
    while(it.hasNext())
    {
        const QString &currentEntry = it.next();
        //move entry from watch folder to permanent folder. TODOoptimization: folder-ize videos maybe. isn't there some maximum files in directory limit?

        //using atomics, let wt know that we've added a segment
        m_SharedVideoSegmentsArrayIndex->fetchAndAddOrdered(1); //yolo. TODOoptimization: find out if i can use relaxed or aquired, too lazy to figure it out. my guess is that i can use the one that doesn't re-arrange memory before the call, but i need to be sure
    }
}

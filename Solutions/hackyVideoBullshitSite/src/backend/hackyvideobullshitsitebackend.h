#ifndef HACKYVIDEOBULLSHITSITEBACKEND_H
#define HACKYVIDEOBULLSHITSITEBACKEND_H

#include <Wt/WServer>

#include <QObject>
#include <QAtomicPointer>

#include "objectonthreadhelper.h"
#include "adimagegetandsubscribemanager.h"
#include "videosegmentsimporterfolderwatcher.h"
#include "lastmodifiedtimestampswatcher.h"

class HackyVideoBullshitSiteBackend : public QObject
{
    Q_OBJECT
public:
    explicit HackyVideoBullshitSiteBackend(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo, const QString &lastModifiedTimestampsFile, QAtomicPointer<LastModifiedTimestampsAndPaths> *timestampsAndPathsSharedAtomicPointer, QObject *parent = 0);
    ~HackyVideoBullshitSiteBackend();
    AdImageGetAndSubscribeManager *adImageGetAndSubscribeManager();
private:
    AdImageGetAndSubscribeManager *m_AdImageGetAndSubscribeManager;
    ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher> *m_VideoSegmentsImporterFolderWatcherThread;
    ObjectOnThreadHelper<LastModifiedTimestampsWatcher> *m_LastModifiedTimestampsWatcherThread;

    //Pass through for VideoSegmentsImporterFolderWatcher
    QString m_VideoSegmentsImporterFolderToWatch; //sftp user writeable
    QString m_VideoSegmentsImporterFolderScratchSpace; //should be the same physical filesystem as the "ToWatch" or "MoveTo", otherwise would incur a stupid cost
    QString m_VideoSegmentsImporterFolderToMoveTo; //not sftp user writeable

    //Pass through for LastModifiedTimestampsWatcher
    QString m_LastModifiedTimestampsFile;
    QAtomicPointer<LastModifiedTimestampsAndPaths> *m_TimestampsAndPathsSharedAtomicPointer;

signals:
    void d(const QString &);
    void finishStoppingRequested();
public slots:
    void initializeAndStart();
    void handleVideoSegmentsImporterFolderWatcherReadyForConnections();
    void handleLastModifiedTimestampsWatcherReadyForConnections();
    void beginStopping();
    void finishStopping();
};

#endif // HACKYVIDEOBULLSHITSITEBACKEND_H

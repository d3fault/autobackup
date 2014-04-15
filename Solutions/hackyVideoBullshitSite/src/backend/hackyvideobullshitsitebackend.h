#ifndef HACKYVIDEOBULLSHITSITEBACKEND_H
#define HACKYVIDEOBULLSHITSITEBACKEND_H

#include <Wt/WServer>
#include <QObject>

#include "objectonthreadhelper.h"
#include "adimagegetandsubscribemanager.h"
#include "videosegmentsimporterfolderwatcher.h"

class AdImageGetAndSubscribeManager;

class HackyVideoBullshitSiteBackend : public QObject
{
    Q_OBJECT
public:
    explicit HackyVideoBullshitSiteBackend(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &videoSegmentsImporterFolderToMoveTo, QObject *parent = 0);
    ~HackyVideoBullshitSiteBackend();
    AdImageGetAndSubscribeManager *adImageGetAndSubscribeManager();
private:
    AdImageGetAndSubscribeManager *m_AdImageGetAndSubscribeManager;
    ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher> *m_VideoSegmentsImporterFolderWatcherThread;

    //Pass through
    QString m_VideoSegmentsImporterFolderToWatch; //sftp user writeable
    QString m_VideoSegmentsImporterFolderScratchSpace; //should be the same physical filesystem as the "ToWatch" or "MoveTo"
    QString m_VideoSegmentsImporterFolderToMoveTo; //not sftp user writeable

    void stopVideoSegmentsImporterFolderWatcherThreadIfNeeded();
signals:
    void d(const QString &);
public slots:
    void initializeAndStart();
    void handleVideoSegmentsImporterFolderWatcherReadyForConnections();
    void beginStopping();
    void finishStopping();
};

#endif // HACKYVIDEOBULLSHITSITEBACKEND_H

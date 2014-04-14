#ifndef HACKYVIDEOBULLSHITSITEBACKEND_H
#define HACKYVIDEOBULLSHITSITEBACKEND_H

#include <QObject>

#include "objectonthreadhelper.h"
#include "adimagegetandsubscribemanager.h"
#include "videosegmentsimporterfolderwatcher.h"

class AdImageGetAndSubscribeManager;
class QAtomicInt;

class HackyVideoBullshitSiteBackend : public QObject
{
    Q_OBJECT
public:
    explicit HackyVideoBullshitSiteBackend(QAtomicInt *sharedVideoSegmentsArrayIndex, QObject *parent = 0);
    ~HackyVideoBullshitSiteBackend();
    AdImageGetAndSubscribeManager *adImageGetAndSubscribeManager();
private:
    AdImageGetAndSubscribeManager *m_AdImageGetAndSubscribeManager;
    ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher> *m_VideoSegmentsImporterFolderWatcherThread;

    //pass through
    QAtomicInt *m_SharedVideoSegmentsArrayIndex;

    void endVideoSegmentsImporterFolderWatcherThreadIfNeeded():
public Q_SLOTS:
    void initializeAndStart();
    void handleVideoSegmentsImporterFolderWatcherReadyForConnections();
    void beginStopping();
    void finishStopping();
};

#endif // HACKYVIDEOBULLSHITSITEBACKEND_H

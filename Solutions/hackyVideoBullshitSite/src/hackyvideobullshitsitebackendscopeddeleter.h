#ifndef HACKYVIDEOBULLSHITSITEBACKENDSCOPEDDELETER_H
#define HACKYVIDEOBULLSHITSITEBACKENDSCOPEDDELETER_H

#include <Wt/WServer> //just so i don't have to use no_keywords

#include <QObject>
#include "lastmodifiedtimestampsandpaths.h"
#include "backend/hackyvideobullshitsitebackend.h"

class HackyVideoBullshitSiteBackendScopedDeleter : public QObject
{
    Q_OBJECT
public:
    HackyVideoBullshitSiteBackend m_HackyVideoBullshitSiteBackend;
    QThread m_HackyVideoBullshitSiteBackendThread;
    QAtomicPointer<LastModifiedTimestampsAndPaths> m_TimestampsAndPathsSharedAtomicPointer;
    HackyVideoBullshitSiteBackendScopedDeleter(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, const QString &lastModifiedTimestampsFile)
            : m_HackyVideoBullshitSiteBackend(videoSegmentsImporterFolderToWatch, videoSegmentsImporterFolderScratchSpace, airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo, lastModifiedTimestampsFile, &m_TimestampsAndPathsSharedAtomicPointer)
    {
        //using a style i dislike (object not instantiating on thread that 'owns' it, but oh well)
        m_HackyVideoBullshitSiteBackendThread.start();
        m_HackyVideoBullshitSiteBackend.moveToThread(&m_HackyVideoBullshitSiteBackendThread);
        connect(&m_HackyVideoBullshitSiteBackend, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    }
    ~HackyVideoBullshitSiteBackendScopedDeleter()
    {
        m_HackyVideoBullshitSiteBackendThread.quit();
        m_HackyVideoBullshitSiteBackendThread.wait();
    }
private slots:
    void handleD(const QString &msg)
    {
        cout << msg.toStdString() << endl;
    }
};

#endif // HACKYVIDEOBULLSHITSITEBACKENDSCOPEDDELETER_H

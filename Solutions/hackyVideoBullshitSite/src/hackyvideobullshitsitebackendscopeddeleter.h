#ifndef HACKYVIDEOBULLSHITSITEBACKENDSCOPEDDELETER_H
#define HACKYVIDEOBULLSHITSITEBACKENDSCOPEDDELETER_H

#include <Wt/WServer> //just so i don't have to use no_keywords

#include <QObject>
#include "backend/hackyvideobullshitsitebackend.h"

class HackyVideoBullshitSiteBackendScopedDeleter : public QObject
{
    Q_OBJECT
public:
    HackyVideoBullshitSiteBackend m_HackyVideoBullshitSiteBackend;
    QThread m_HackyVideoBullshitSiteBackendThread;
    HackyVideoBullshitSiteBackendScopedDeleter(const QString &videoSegmentsImporterFolderToWatch, const QString &videoSegmentsImporterFolderScratchSpace, const QString &airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo)
            : m_HackyVideoBullshitSiteBackend(videoSegmentsImporterFolderToWatch, videoSegmentsImporterFolderScratchSpace, airborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo)
    {
        //using a style i dislike (object not instantiating on thread that 'owns' it, but oh well)
        connect(&m_HackyVideoBullshitSiteBackend, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
        m_HackyVideoBullshitSiteBackendThread.start();
        m_HackyVideoBullshitSiteBackend.moveToThread(&m_HackyVideoBullshitSiteBackendThread);
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

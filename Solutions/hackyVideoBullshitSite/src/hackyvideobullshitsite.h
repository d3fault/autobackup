#ifndef HACKYVIDEOBULLSHITSITE_H
#define HACKYVIDEOBULLSHITSITE_H

#include <Wt/WServer>
#include <Wt/WApplication>
using namespace Wt;

#include <QObject>
#include <QStringList>

#include "objectonthreadhelper.h"
#include "wtcontrollerandstdoutowner.h"
#include "backend/adimagegetandsubscribemanager.h"
#include "backend/videosegmentsimporterfolderwatcher.h"
#include "backend/lastmodifiedtimestampswatcher.h"

class StandardInputNotifier;

class HackyVideoBullshitSite : public QObject
{
    Q_OBJECT
public:
    explicit HackyVideoBullshitSite(int argc, char* argv[], QObject *parent = 0);
private:
    /*struct FoldersWithCorrespondingLastModifiedTimestampFiles
    {
        QString AbsoluteDirectoryPath;
        QString AbsoluteFilePathOfLastModifiedTimestampsFile;
    };*/

    int m_ArgC;
    char **m_ArgV;

    ObjectOnThreadHelper<WtControllerAndStdOutOwner> *m_WtControllerAndStdOutOwnerThread;
    ObjectOnThreadHelper<AdImageGetAndSubscribeManager> *m_AdImageGetAndSubscribeManagerThread;
    ObjectOnThreadHelper<VideoSegmentsImporterFolderWatcher> *m_VideoSegmentsImporterFolderWatcherThread;
    ObjectOnThreadHelper<LastModifiedTimestampsWatcher> *m_LastModifiedTimestampsWatcherThread;

    ObjectOnThreadSynchronizer *m_ObjectOnThreadSynchronizer; //OLDnvm critical: MUST be 'after' any object on threads it synchronizes so that it is destructed before them. at destruction, it calls QThread::quit on them giving us easy async quitting too :)

    QStringList m_VideoSegmentImporterFoldersToWatch;
    QString m_VideoSegmentsImporterFolderScratchSpace;
    QString m_AirborneVideoSegmentsBaseDir_aka_VideoSegmentsImporterFolderToMoveTo;
    QString m_HvbsWebBaseDir_NoSlashAppended;
    QStringList m_LastModifiedTimestampsFiles;
    QString m_NeighborPropagationRemoteSftpUploadScratchSpace;
    QString m_NeighborPropagationRemoteDestinationToMoveTo;
    QString m_NeighborPropagationUserHostPathComboSftpArg;
    QString m_SftpProcessPath;

    StandardInputNotifier *m_StdIn;

    inline QString appendSlashIfNeeded(const QString &inputString)
    {
        return inputString.endsWith("/") ? inputString : (inputString + "/");
    }
    inline QString removeTrailingSlashIfNeeded(const QString &inputString)
    {
        return inputString.endsWith("/") ? inputString.left(inputString.length()-1) : inputString;
    }
signals:
    void o(const QString &);
    void e(const QString &);
    void tellVideoSegmentNeighborPropagationInformationRequested();

    //stop video segment propagation in one of 3 ways before actually stopping
    void stopHackyVideoBullshitSiteCleanlyOnceVideoSegmentNeighborPropagatationFinishesRequested();
    void stopHackyVideoBullshitSiteCleanlyOnceVideoSegmentNeighborPropagatationFinishesUnlessDcRequested();
    void stopHackyVideoBullshitSiteNowRequested();

    void videoSegmentNeighborPropagationFinishedStopContinueStoppingRequested();
private slots:
    void handleWtControllerAndStdOutOwnerIsReadyForConnections();
    //void handleAdImageGetAndSubscribeManagerIsReadyForConnections();
    void handleVideoSegmentsImporterFolderWatcherReadyForConnections();
    void handleLastModifiedTimestampsWatcherReadyForConnections();
    void handleWatchingLastModifiedTimestampsFileStarted();

    void handleAllBackendObjectsOnThreadsReadyForConnections();

    void cliUsage();

    void handleStandardInput(const QString &line);
    void handleAboutToQuit();
    void handleFatalError();
};

#endif // HACKYVIDEOBULLSHITSITE_H

#include "sftpuploaderandrenamerqueue.h"

#include <QTextStream>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>

//TODOoptional: take out 5 second wait thing since it's stupid and does nothing anyways (no connection = still passes). really the only way i can find out if it's connected is by seeing if an upload/rename completes. HOWEVER this is only optional because it's only visual (i mean aside from the stupid 5 second wait itself) and doesn't break anything else

bool SftpUploaderAndRenamerQueue::m_HaveRunConstructorOncePerApp = false; //TODOblah: not thread-safe. OWNED

//at first i was amped to learn about sftp, but now after trying to use it in automation it's a freaking pain in the ass. provides very little feedback (whereas scp i'd just check return code == 0). hmm, *tries cranking up verbosity*. cool, increasing verbosity does NOTHING (except a bunch of shit i don't care about on stderr). there's no "upload complete" message... pos...
SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueue(QObject *parent)
    : QObject(parent)
    , m_FiveSecondRetryDequeueAndUploadTimer(new QTimer(this))
    , m_Sftp30secondKillerTimer(new QTimer(this))
    , m_SftpProcess(0)
    , m_SftpProcessTextStream(0)
    , m_SftpIsReadyForCommands(false)
    , m_SftpPutInProgressSoWatchForRenameCommandEcho(false)
    , m_SftpUploaderAndRenamerQueueState(SftpUploaderAndRenamerQueueState_NotYetStarted)
{
    if(!m_HaveRunConstructorOncePerApp)
    {
        qRegisterMetaType<SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum>("SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum");
        m_HaveRunConstructorOncePerApp = true;
    }


    m_FiveSecondRetryDequeueAndUploadTimer->setInterval(5000);
    m_FiveSecondRetryDequeueAndUploadTimer->setSingleShot(true);
    connect(m_FiveSecondRetryDequeueAndUploadTimer, SIGNAL(timeout()), this, SLOT(tryDequeueAndUploadSingleSegment()));

    m_Sftp30secondKillerTimer->setInterval(29000);
    m_Sftp30secondKillerTimer->setSingleShot(true);
    connect(m_Sftp30secondKillerTimer, SIGNAL(timeout()), this, SLOT(killSftpIfStillRunning()));
}
SftpUploaderAndRenamerQueue::~SftpUploaderAndRenamerQueue()
{
    if(m_SftpProcess && m_SftpProcess->state() != QProcess::NotRunning)
    {
        emit e("sending sftp terminate signal from SftpUploaderAndRenamerQueue destructor. bug in stop code?");
        m_SftpProcess->terminate();
        if(!m_SftpProcess->waitForFinished())
        {
            emit e("killing sftp from SftpUploaderAndRenamerQueue destructor. bug in stop code?");
            m_SftpProcess->kill();
        }
    }
    if(m_SftpProcessTextStream)
        delete m_SftpProcessTextStream;
}
void SftpUploaderAndRenamerQueue::setTheStateEnumValueUpgradingStopSpeedOnly(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState)
{
    //ignore
    if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_NotYetStarted || newSftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_Started)
        return;

    //typical stop [type] requested
    if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_Started)
    {
        if(newSftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_NotYetStarted/*bug, but handle it*/ || newSftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully)
        {
            m_SftpUploaderAndRenamerQueueState = SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully;
            return;
        }

        m_SftpUploaderAndRenamerQueueState = newSftpUploaderAndRenamerQueueState;
        return;
    }

    //upgrade to sooner or now
    if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully && (newSftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc || newSftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopNow))
    {
        m_SftpUploaderAndRenamerQueueState = newSftpUploaderAndRenamerQueueState;
        return;
    }

    //upgrade to now
    if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc && newSftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopNow)
    {
        m_SftpUploaderAndRenamerQueueState = newSftpUploaderAndRenamerQueueState;
        return;
    }
}
void SftpUploaderAndRenamerQueue::stopSftpProcess()
{
    if(m_SftpIsReadyForCommands)
    {
        emit o("telling sftp bye bye");
        *m_SftpProcessTextStream << "bye" << endl << "bye" << endl; //idfk why, but when i was testing this it needed two byes. since the second one [probably?] won't hurt, fuck it :-P. i think it's because the first bye causes a disconnect, and for some retarded reason sftp hangs there until it receives another command, at which case that command makes stfp error-out/quit (because disconnect)
        if(!m_Sftp30secondKillerTimer->isActive())
        {
            m_Sftp30secondKillerTimer->start();
        }
    }
    else if(m_SftpProcess->state() != QProcess::NotRunning)
    {
        emit o("sending sftp terminate signal");
        m_SftpProcess->terminate(); //hmmm idk lol
        if(!m_Sftp30secondKillerTimer->isActive())
        {
            m_Sftp30secondKillerTimer->start();
        }
    }
}
void SftpUploaderAndRenamerQueue::startSftpUploaderAndRenamerQueue(const QString &remoteDestinationToUploadTo, const QString &remoteDestinationToMoveTo, const QString &userHostPathComboSftpArg, const QString &sftpProcessPath)
{
    if(remoteDestinationToMoveTo.isEmpty() || remoteDestinationToUploadTo.isEmpty())
    {
        emit e("can't have empty remote destinations");
        return;
    }
    if(userHostPathComboSftpArg.isEmpty())
    {
        emit e("can't have empty user host path (combo) sftp arg. should look like this: 'username@host' -- or just 'host' at a minimum");
        return;
    }
    m_UserHostPathComboSftpArg = userHostPathComboSftpArg;
    m_RemoteDestinationToUploadToWithSlashAppended = appendSlashIfMissing(remoteDestinationToUploadTo);
    m_RemoteDestinationToMoveToWithSlashAppended = appendSlashIfMissing(remoteDestinationToMoveTo);
    m_SftpProcessPath = sftpProcessPath;
    if(sftpProcessPath.isEmpty())
    {
        m_SftpProcessPath = "/usr/bin/sftp";
    }
    if(m_SftpProcess)
        delete m_SftpProcess;
    m_SftpProcess = new QProcess(this);
    if(m_SftpProcessTextStream)
        delete m_SftpProcessTextStream;
    m_SftpProcessTextStream = new QTextStream(m_SftpProcess);
    m_SftpProcess->setReadChannel(QProcess::StandardOutput);
    connect(m_SftpProcess, SIGNAL(started()), this, SLOT(handleSftpProcessStarted()));
    connect(m_SftpProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleSftpProcessFinished(int,QProcess::ExitStatus)));
    connect(m_SftpProcess, SIGNAL(readyReadStandardError()), this, SLOT(handleSftpProcessReadyReadStandardError()));
    connect(m_SftpProcess, SIGNAL(readyRead()), this, SLOT(handleSftpProcessReadyReadStandardOut()));
    m_SftpIsReadyForCommands = false;
    m_SftpPutInProgressSoWatchForRenameCommandEcho = false;
    m_SftpUploaderAndRenamerQueueState = SftpUploaderAndRenamerQueueState_NotYetStarted;
    startSftpProcessInBatchMode();
}
void SftpUploaderAndRenamerQueue::enqueueFileForUploadAndRename(SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type timestampAndFilenameToEnqueueForUpload)
{
    m_SegmentsQueuedForUpload.enqueue(timestampAndFilenameToEnqueueForUpload);
    tryDequeueAndUploadSingleSegment();
}
void SftpUploaderAndRenamerQueue::tellStatus()
{
    int queueSize = m_SegmentsQueuedForUpload.size();
    QString headOfUploadQueue = (queueSize > 0 ?  m_SegmentsQueuedForUpload.head().second : "");
    QString segmentInformations = "==Sftp Uploader and Renamer Queue Status==\nSize of upload queue: " + QString::number(queueSize) + "\n'Head' of the upload queue: " + headOfUploadQueue + "\n'Tail' of upload queue: " + (queueSize > 0 ?  m_SegmentsQueuedForUpload.back().second : "") + "\nsftp connection status: " + (m_SftpIsReadyForCommands ? "" : "NOT ") + "ready for commands";
    emit statusGenerated(segmentInformations);
}
void SftpUploaderAndRenamerQueue::changeSftpUploaderAndRenamerQueueState(SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueueStateEnum newSftpUploaderAndRenamerQueueState)
{
    setTheStateEnumValueUpgradingStopSpeedOnly(newSftpUploaderAndRenamerQueueState);

    //TODOreq: this might not be right because ffmpeg might not have finished stopping (and therefore the queue MIGHT be empty but still another one could come in the not too distant future...)
    if(m_SegmentsQueuedForUpload.isEmpty() || m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopNow)
    {
        stopSftpProcess();
#if 0
        m_Sftp30secondKillerTimer->stop();
        if(!m_SftpProcess->waitForFinished(3000))
        {
            m_SftpProcess->kill();
        }
#endif
        return;
    }

    if(!m_SftpIsReadyForCommands && m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc)
    {
        stopSftpProcess();
        return;
    }
}
void SftpUploaderAndRenamerQueue::startSftpProcessInBatchMode()
{
    QStringList sftpArgs;
    sftpArgs << "-b" << "-" << m_UserHostPathComboSftpArg;
    m_SftpProcess->start(m_SftpProcessPath, sftpArgs, QIODevice::ReadWrite); //TODOidgaf: does windows use \r\n for stdin delimiting also? if so, QIODevice::Text flag should be passed in too
}
void SftpUploaderAndRenamerQueue::tryDequeueAndUploadSingleSegment()
{
    if(m_FiveSecondRetryDequeueAndUploadTimer->isActive())
    {
        m_FiveSecondRetryDequeueAndUploadTimer->stop();
    }

    if(m_SegmentsQueuedForUpload.isEmpty())
    {
        if (m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc || m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopNow)
        {
            stopSftpProcess();
        }
        return;
    }

    if(m_SftpPutInProgressSoWatchForRenameCommandEcho)
        return;
#if 0 //pseudo-code
     bool uploadedOne = false;
    if(scpUpload(queuedPair.second, m_RemoteDestinationToUploadToWithSlashAppended)) //sftp >
    {
        if(persistentSshConnectionMoveCmd(m_RemoteDestinationToUploadToWithSlashAppended + queuedPair.second, m_RemoteDestinationToMoveToWithSlashAppended + queuedPair.first + ".ogg" /*TODOoptional: argv or auto-detect extention*/))
        {
            uploadedOne = true;
            m_SegmentsQueuedForUpload.dequeue();
        }
        //else: how would this fail? if the ssh connection drops?
    }
    else
    {
        //perhaps delete partially written file? probably not but honestly no idea. depends on scp's implementation guh
    }
    if(!uploadedOne)
    {
        emit d("ffmpeg segment uploader failed to upload a segment, check network status etc");
        QTimer::singleShot(5000, Qt::VeryCoarseTimer, this, SLOT(tryDequeueAndUploadSingleSegment())); //TODOoptional: exponential backoff with max maybe, but eh tbh the more i see this, the more lag my videos will have "overall", so 5 seconds is good/fine for now...
    }
    if(!m_SegmentsQueuedForUpload.isEmpty())
    {
        QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection);
    }
#endif
    if(m_SftpIsReadyForCommands)
    {
        const QPair<QString,QString> &queuedPair = m_SegmentsQueuedForUpload.head();
        QFileInfo fileInfoToGetFilenamePortion(queuedPair.second);

        QString remoteUploadPath = "\"" + m_RemoteDestinationToUploadToWithSlashAppended + fileInfoToGetFilenamePortion.fileName() + "\"";
        QString putCommand = "put \"" + queuedPair.second + "\" " + remoteUploadPath;
        QString renameCommand = "rename \"" + remoteUploadPath + "\" \"" + m_RemoteDestinationToMoveToWithSlashAppended + queuedPair.first + "\"";
        //queue upload then rename to sftp
        *m_SftpProcessTextStream << putCommand << endl << renameCommand << endl;
        m_SftpPutInProgressSoWatchForRenameCommandEcho = true;
    }
    else
    {
        if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_Started || m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully)
        {
            emit e("segment queue couldn't upload because sftp is not ready for commands, trying again in 5 sec");
            if(!m_FiveSecondRetryDequeueAndUploadTimer->isActive())
            {
                m_FiveSecondRetryDequeueAndUploadTimer->start();
            }
        }
    }
}
void SftpUploaderAndRenamerQueue::handleSftpProcessStarted()
{
    //m_SftpProcess->waitForStarted(-1);
    //how the fuck do i know when the sftp session is READY? surely it isn't just when the process is started, guh
    //the only hacky solution i can think of is to see if the process DOESN'T finish in a few seconds. if auth fail or whatever, it will finish soon. lame but whatever...
    emit o("sftp connecting (5 sec wait)...");
    if(m_SftpProcess->waitForFinished(5000)) //TODOoptional: 5 second startup time unnecessarily added to my app, fuck you sftp
    {
        m_SftpIsReadyForCommands = false;
        m_SftpUploaderAndRenamerQueueState = SftpUploaderAndRenamerQueueState_StopNow;
        emit e("sftp started and then finished pretty quickly, so auth probably failed or something?");
        emit quitRequested();
    }
    else
    {
        emit o("sftp MAYBE connected (hack/lazy)");
        m_SftpIsReadyForCommands = true;
        QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection); //might use this code path for initial connection (race condition), but if this is a reconnect, we might want to resume uploading if queue isn't empty
    }

    if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_NotYetStarted) //don't re-emit signal on reconnect, and also don't change the stop type derp!
    {
        m_SftpUploaderAndRenamerQueueState = SftpUploaderAndRenamerQueueState_Started;
        emit sftpUploaderAndRenamerQueueStarted();
    }
}
void SftpUploaderAndRenamerQueue::handleSftpProcessReadyReadStandardOut()
{
    while(!m_SftpProcessTextStream->atEnd())
    {
        QString currentLine = m_SftpProcessTextStream->readLine();
#if 0
        if(!m_SftpIsReadyForCommands)
        {
            if(currentLine.startsWith("sftp>"))
            {
                m_SftpIsReadyForCommands = true;
                QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection); //might use this code path for initial connection (race condition), but if this is a reconnect, we might want to resume uploading if queue isn't empty
            }
        }
        else
#endif
        if(m_SftpIsReadyForCommands)
        {
            //process or disregard sftp's standard out
            //seeing the "rename" command being echo'd back to us means that the upload that preceeded it finished [successfully if the process didn't abort], which means we can now start uploading the next segment in the queue (if any)
            if(m_SftpPutInProgressSoWatchForRenameCommandEcho && currentLine.startsWith("sftp> rename "))
            {
                m_SftpPutInProgressSoWatchForRenameCommandEcho = false;
                SftpUploaderAndRenamerQueue_DestFilenameToRenameToAfterUpload_and_SrcFilenameToUpload_Type fileJustUploadedAndRenamed = m_SegmentsQueuedForUpload.dequeue();
                emit fileUploadAndRenameSuccess(fileJustUploadedAndRenamed.first, fileJustUploadedAndRenamed.second);
                if(m_SegmentsQueuedForUpload.isEmpty() && m_SftpUploaderAndRenamerQueueState != SftpUploaderAndRenamerQueueState_Started) //TODOoptional: the last segment PROBABLY won't be segmentLengthSeconds long, so the timestamp we calculate when it's added to the segment entry list will be early and 'pointing to a time in the previous segment'. i could solve this by keeping track of time elapsed or whatever and then only using that for the very last segment added when quitting, but i'm too lazy for now :-P
                {
                    emit o("FfmpegSegmentUploader now cleanly stopping since upload queue is empty...");
                    stopSftpProcess();
                    return;
                }
                //now onto the next one
                QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection);
            }
            //disregard the "uploading ..." lines. worthless
        }
        else //it shouldn't say anything unless we give it a command
        {
            emit e("sftp is not ready for commands and said: '" + currentLine + "'");
        }
    }
}
void SftpUploaderAndRenamerQueue::handleSftpProcessReadyReadStandardError()
{
    QByteArray allStandardErrorBA = m_SftpProcess->readAllStandardError();
    QString allStandardErrorString(allStandardErrorBA);
    emit e("sftp's standard error wrote: "+ allStandardErrorString);
}
void SftpUploaderAndRenamerQueue::killSftpIfStillRunning()
{
    if(m_SftpProcess->state() != QProcess::NotRunning)
    {
        if(!m_SftpProcess->waitForFinished(1000))
        {
            emit e("killing sftp");
            m_SftpProcess->kill();
        }
    }
}
void SftpUploaderAndRenamerQueue::handleSftpProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_SftpIsReadyForCommands = false;
    m_SftpPutInProgressSoWatchForRenameCommandEcho = false;

    if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_NotYetStarted)
    {
        //wtf?
        emit e("sftp process finished, but SftpUploaderAndRenamerQueue's state is NotYetStarted. probably a bug");
        return;
    }

    if(m_SegmentsQueuedForUpload.isEmpty() && m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully/*TODOreq: ensure that the final segment is enqueued before switching to SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully*/)
    {
        emit o("...SftpUploaderAndRenamerQueue has cleanly stopped");
        emit sftpUploaderAndRenamerQueueStopped();
    }
    else if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_Started || m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfully) //TODOreq: should maybe rely on exitCode/exitStatus additionally? it might be zero on a connection drop though idfk
    {
        //sftp -b mode will quit with exitCode == 1 any time an operation does not succeed, and I'd imagine also if/when the connection is 'lost' (?)... though perhaps that isn't noticed until an operation fails? idk lol

        //reconnect by starting it again
        emit e("sftp will try to start again in 5 seconds...");
        QTimer::singleShot(5000
#if !(QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
                           , Qt::VeryCoarseTimer
#endif
                           , this, SLOT(startSftpProcessInBatchMode()));
    }
    else if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopWhenAllUploadsFinishSuccessfullyUnlessDc)
    {
        emit o("...SftpUploaderAndRenamerQueue has stopped because the sftp process finished");
        emit sftpUploaderAndRenamerQueueStopped();
    }
    else if(m_SftpUploaderAndRenamerQueueState == SftpUploaderAndRenamerQueueState_StopNow)
    {
        emit o("...SftpUploaderAndRenamerQueue has stopped");
        emit sftpUploaderAndRenamerQueueStopped();
    }


    if(exitCode != 0)
    {
        emit e("sftp exitted with non-zero code: " + QString::number(exitCode) + " (and is probably being restarted)");
    }
    if(exitStatus != QProcess::NormalExit)
    {
        emit e("sftp crashed (and is probably being restarted)");
    }
}

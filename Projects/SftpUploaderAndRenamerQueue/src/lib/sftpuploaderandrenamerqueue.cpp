#include "sftpuploaderandrenamerqueue.h"

#include <QTextStream>
#include <QTimer>
#include <QFileInfo>

//at first i was amped to learn about sftp, but now after trying to use it in automation it's a freaking pain in the ass. provides very little feedback (whereas scp i'd just check return code == 0). hmm, *tries cranking up verbosity*. cool, increasing verbosity does NOTHING (except a bunch of shit i don't care about on stderr). there's no "upload complete" message... pos...
SftpUploaderAndRenamerQueue::SftpUploaderAndRenamerQueue(QObject *parent)
    : QObject(parent)
    , m_FiveSecondRetryTimer(new QTimer(this))
    , m_SftpProcess(0)
    , m_SftpProcessTextStream(0)
    , m_SftpIsReadyForCommands(false)
    , m_SftpPutInProgressSoWatchForRenameCommandEcho(false)
    , m_SftpWasToldToQuit(false)
{
    m_FiveSecondRetryTimer->setInterval(5000);
    m_FiveSecondRetryTimer->setSingleShot(true);
    connect(m_FiveSecondRetryTimer, SIGNAL(timeout()), this, SLOT(tryDequeueAndUploadSingleSegment()));
}
SftpUploaderAndRenamerQueue::~SftpUploaderAndRenamerQueue()
{
    if(m_SftpProcess && m_SftpProcess->state() != QProcess::NotRunning)
    {
        if(m_SftpProcessTextStream)
        {
            stopSftpProcess();
        }
        if(m_SftpProcess->state() != QProcess::NotRunning)
        {
            m_SftpProcess->terminate();
            if(!m_SftpProcess->waitForFinished(5000))
            {
                m_SftpProcess->kill();
            }
        }
    }
    if(m_SftpProcessTextStream)
        delete m_SftpProcessTextStream;
}
void SftpUploaderAndRenamerQueue::stopSftpProcess()
{
    if(m_SftpIsReadyForCommands)
    {
        *m_SftpProcessTextStream << "bye" << endl << "bye" << endl; //idfk why, but when i was testing this it needed two byes. since the second one [probably?] won't hurt, fuck it :-P. i think it's because the first bye causes a disconnect, and for some retarded reason sftp hangs there until it receives another command, at which case that command makes stfp error-out/quit (because disconnect)
    }
    else if(m_SftpProcess->state() != QProcess::NotRunning)
    {
        m_SftpProcess->terminate(); //hmmm idk lol
        if(!m_SftpProcess->waitForFinished(5000))
        {
            m_SftpProcess->kill();
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
    m_SftpWasToldToQuit = false;
    startSftpProcessInBatchMode();

    emit sftpUploaderAndRenamerQueueStarted();
}
void SftpUploaderAndRenamerQueue::enqueueFileForUploadAndRename(SftpUploaderAndRenamerQueueTimestampAndFilenameType timestampAndFilenameToEnqueueForUpload)
{
    m_SegmentsQueuedForUpload.enqueue(timestampAndFilenameToEnqueueForUpload);
    tryDequeueAndUploadSingleSegment();
}
void SftpUploaderAndRenamerQueue::tellStatus()
{
    int queueSize = m_SegmentsQueuedForUpload.size();
    QString headOfUploadQueue = (queueSize > 0 ?  m_SegmentsQueuedForUpload.head().second : "");
    QString segmentInformations = "==Ffmpeg Segment Uploader Status==\nSize of upload queue: " + QString::number(queueSize) + "\n'Head' of the upload queue: " + headOfUploadQueue + "\n'Tail' of upload queue: " + (queueSize > 0 ?  m_SegmentsQueuedForUpload.back().second : "") + "\nsftp connection status: " + (m_SftpIsReadyForCommands ? "" : "NOT ") + "ready for commands";
    emit statusGenerated(segmentInformations);
}
void SftpUploaderAndRenamerQueue::stopSftpUploaderAndRenamerQueue()
{
    m_SftpWasToldToQuit = true;
    if(m_SegmentsQueuedForUpload.isEmpty()) //start -> stop no segment entries seen, and also if ffmpeg was stopped a while ago
    {
        stopSftpProcess();
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
    if(m_FiveSecondRetryTimer->isActive())
    {
        m_FiveSecondRetryTimer->stop();
    }
    if(m_SegmentsQueuedForUpload.isEmpty() || m_SftpPutInProgressSoWatchForRenameCommandEcho)
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
        QString renameCommand = "rename \"" + remoteUploadPath + "\" \"" + m_RemoteDestinationToMoveToWithSlashAppended + queuedPair.first + ".ogg\"";
        //queue upload then rename to sftp
        *m_SftpProcessTextStream << putCommand << endl << renameCommand << endl;
        m_SftpPutInProgressSoWatchForRenameCommandEcho = true;
    }
    else
    {
        emit e("segment queue couldn't upload because sftp is not ready for commands, trying again in 5 sec");
        if(!m_FiveSecondRetryTimer->isActive())
        {
            m_FiveSecondRetryTimer->start();
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
        m_SftpWasToldToQuit = true; //give it permission to not retry
        emit e("sftp started and then finished pretty quickly, so auth probably failed or something?");
    }
    else
    {
        emit o("sftp connected");
        m_SftpIsReadyForCommands = true;
        QMetaObject::invokeMethod(this, "tryDequeueAndUploadSingleSegment", Qt::QueuedConnection); //might use this code path for initial connection (race condition), but if this is a reconnect, we might want to resume uploading if queue isn't empty
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
                m_SegmentsQueuedForUpload.dequeue();
                if(m_SftpWasToldToQuit && m_SegmentsQueuedForUpload.isEmpty()) //TODOoptional: the last segment PROBABLY won't be segmentLengthSeconds long, so the timestamp we calculate when it's added to the segment entry list will be early and 'pointing to a time in the previous segment'. i could solve this by keeping track of time elapsed or whatever and then only using that for the very last segment added when quitting, but i'm too lazy for now :-P
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
void SftpUploaderAndRenamerQueue::handleSftpProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_SftpIsReadyForCommands = false;
    m_SftpPutInProgressSoWatchForRenameCommandEcho = false;

    if(!m_SftpWasToldToQuit) //TODOreq: should maybe rely solely on exitCode/exitStatus instead of this bool? it might be zero on a connection drop though idfk
    {
        //sftp -b mode will quit with exitCode == 1 any time an operation does not succeed, and I'd imagine also if/when the connection is 'lost' (?)... though perhaps that isn't noticed until an operation fails? idk lol

        //reconnect by starting it again
        emit e("sftp will try to start again in 5 seconds...");
        QTimer::singleShot(5000, Qt::VeryCoarseTimer, this, SLOT(startSftpProcessInBatchMode()));
    }
    else
    {
        m_SftpWasToldToQuit = false;
        emit o("...SftpUploaderAndRenamerQueue has cleanly stopped");
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

#include "hottee.h"

#include <QTextStream>
#include <QFile>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>

#include <boost/filesystem.hpp> //*shakes fist at Qt*

#define READ_CHUNK_SIZE_BYTES (32*1024) //32kb
#define ONE_HUNDRED_MEGABYTES (1024*1024*100)
#define HOTTEE_ONE_GIGABYTE (1024*1024*1024)
#define EMIT_CHANGE_SOON_AT_PERCENT 80

//TODOoptional: could also put each 100mb chunk through a hash for checksumming etc
//TODOoptional: when "q" is used and the input process has stopped, it should quit? or maybe if WHENEVER the input process stops, it should quit? Right now I'm using "q" followed by a "qq", but idk if the first q makes any difference
Hottee::Hottee(QObject *parent) :
    QObject(parent), m_InputProcess(0), m_OutputProcess(0), m_WriteToOutputProcess(false), m_CurrentOutputFile(0), m_Current100mbChunkWriteOffset(0), m_Dest2(false), m_CurrentlyWritingToEitherDestination(false), m_StopWritingAtEndOfThisChunk(false), m_StartWritingAtBeginningOfNextChunk(false), m_QuitAfterThisChunkFinishes(false), m_100mbChunkOffsetForFilename(0), m_DestinationStoragePercentUsedLastTime(0), m_LastTimestampOfSyncInMS(0), m_RestartOutputProcessOnNextChunkStart(false)
{ }
Hottee::~Hottee()
{
    cleanupHotteeing();
}
bool Hottee::startOutputProcess()
{
    if(m_OutputProcess)
        delete m_OutputProcess;
    m_OutputProcess = new QProcess(this);
    m_OutputProcess->start(m_OutputProcessPathAndArgs, QIODevice::WriteOnly);
    if(!m_OutputProcess->waitForStarted(-1))
    {
        emit e("output process failed to start");
        return false;
    }
    connect(m_OutputProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleOutputProcessFinished(int,QProcess::ExitStatus)));
    return true;
}
bool Hottee::eitherDestinationIsLessThan1gbCapacity()
{
    if(filesystemIsLessThan1gbCapacity(m_Destination1endingWithSlash))
        return true;
    if(filesystemIsLessThan1gbCapacity(m_Destination2endingWithSlash))
        return true;
    return false;
}
bool Hottee::filesystemIsLessThan1gbCapacity(const QString &pathOfFilesystem)
{
    boost::filesystem::path currentPath(pathOfFilesystem.toStdString());
    boost::filesystem::space_info currentPathSpaceInfo = boost::filesystem::space(currentPath);
    double spaceCapacity = static_cast<double>(currentPathSpaceInfo.capacity);
    return (spaceCapacity < HOTTEE_ONE_GIGABYTE);
}
void Hottee::toggleDestinations()
{
    m_DestinationStoragePercentUsedLastTime = 0;
    emit o("NOTICE: Just changed from " + m_CurrentDestinationEndingWithSlash); //listeners of this can know that the last 100mb chunk is flushed/closed by qt (but likely still needs "sync" system call (umount should take care of that))
    if(m_Dest2)
    {
        m_Dest2 = false;
        m_CurrentDestinationEndingWithSlash = m_Destination1endingWithSlash;
        return;
    }
    m_Dest2 = true;
    m_CurrentDestinationEndingWithSlash = m_Destination2endingWithSlash;
}
bool Hottee::createAndOpen100mbFileAtCurrentDestination()
{
    m_LastTimestampOfSyncInMS = QDateTime::currentMSecsSinceEpoch();
    if(!m_CurrentOutputFile)
    {
        m_CurrentOutputFile = new QFile(this);
    }
    QString chunkIndexStringWithZeroPadding = QString("%1").arg(m_100mbChunkOffsetForFilename, 9, 10, QChar('0')); //width of 9 gives me ~253 years of 100mb chunks, the padding is to help with sorting ofc (although probably not necessary fuckit)
    m_CurrentOutputFile->setFileName(m_CurrentDestinationEndingWithSlash + "streamChunk_" + chunkIndexStringWithZeroPadding + ".bin"); //when daisy chaining across network, only chunk offset in filename will allow for 'matching' the filenames across systems. using timestamps in filename would be inaccurate [sometimes]
    if(!m_CurrentOutputFile->open(QIODevice::WriteOnly))
    {
        emit d("opening destination file for writing failed: " + m_CurrentOutputFile->fileName());
        return false;
    }
    return true;
}
bool Hottee::readInputProcessesStdOutAndWriteAccordingly()
{
    qint64 bytesAvailable = m_InputProcess->bytesAvailable();
    while(true)
    {
        if(bytesAvailable < 1)
            return true;

        qint64 bytesLeftIn100mbChunk = (ONE_HUNDRED_MEGABYTES-m_Current100mbChunkWriteOffset);
        qint64 sizeToRead = qMin(qMin(bytesAvailable, static_cast<qint64>(READ_CHUNK_SIZE_BYTES)), bytesLeftIn100mbChunk);
        if(sizeToRead < 1)
        {
            //spot right here indicates a ONE_HUNDRED_MEGABYTES 'SYNC' point
            ++m_100mbChunkOffsetForFilename;
            m_Current100mbChunkWriteOffset = 0;
            if(m_RestartOutputProcessOnNextChunkStart)
            {
                startOutputProcess();
                m_RestartOutputProcessOnNextChunkStart = false;
            }

            if(m_StartWritingAtBeginningOfNextChunk)
            {
                m_CurrentlyWritingToEitherDestination = true;
                m_DestinationStoragePercentUsedLastTime = 0;
            }
            if(m_CurrentlyWritingToEitherDestination)
            {
                //flush/close current, then set up next 100mb file, then continue;

                if(!m_StartWritingAtBeginningOfNextChunk) //special case, nothing to flush/close if we are starting back up again
                {
                    if(!m_CurrentOutputFile->flush())
                    {
                        emit d("error flushing 100mb chunk file: " + m_CurrentOutputFile->fileName());
                        m_CurrentOutputFile->close();
                        return false;
                    }
                    m_CurrentOutputFile->close();
                }
                else
                {
                    m_StartWritingAtBeginningOfNextChunk = false;
                }

                if(m_QuitAfterThisChunkFinishes)
                {
                    emit d("Hottee is cleanly quitting at end of chunk: " + m_CurrentOutputFile->fileName());
                    emit d("Wait...");
                    cleanupHotteeing(); //it isn't strictly necessary to call this here, since it's called by our destructor... but in order to make readyRead never be emitted again, it's best to call it as soon as possible (now)
                    emit quitRequested(); //TO DOnereq: backends should not call "quit", they should just emit "done" or "stopped" and then the listener can decide when to call quit. BECAUSE: if there are multiple backends. I think there is a way to do that "waiting for all backends to finish" thing fancily using QFuture or similar, but can't find it. QFutureSynchronizer sounds right, but has no signals wtf -_-. I _COULD_ solve that uglily by using a separate thread whose job is just to synchronously wait on all my backends to finish, and then he emits a signal. Ugly and expensive, but would work.
                    return true; //true/false doesn't matter here, since readyRead is never emitted again we never get to to where it would matter
                }

                if(m_StopWritingAtEndOfThisChunk)
                {
                    emit d("Attempting to cleanly stop writing at the end of 100mb chunk: " + m_CurrentOutputFile->fileName());
                    m_StopWritingAtEndOfThisChunk = false;
                    m_CurrentlyWritingToEitherDestination = false;
                    continue;
                }

                //see if 100mb space left on this destination. if not, toggle destinations
                boost::filesystem::path currentPath(m_CurrentDestinationEndingWithSlash.toStdString());
                boost::filesystem::space_info currentPathSpaceInfo = boost::filesystem::space(currentPath);
                double spaceAvailable = static_cast<double>(currentPathSpaceInfo.available);
                double spaceCapacity = static_cast<double>(currentPathSpaceInfo.capacity);
                if(spaceAvailable < ONE_HUNDRED_MEGABYTES)
                {
                    //toggle
                    toggleDestinations();
                    boost::filesystem::path currentPath2(m_CurrentDestinationEndingWithSlash.toStdString());
                    boost::filesystem::space_info currentPathSpaceInfo2 = boost::filesystem::space(currentPath2);
                    spaceAvailable = static_cast<double>(currentPathSpaceInfo2.available);
                    spaceCapacity = static_cast<double>(currentPathSpaceInfo2.capacity);
                    if(spaceAvailable < ONE_HUNDRED_MEGABYTES)
                    {
                        emit o("!!!!!!!!ALERT: human intervention did not occur in time. We are no longer writing to either destination drive!!!!!!!!");
                        return false;
                    }
                }

                //see if we are crossing over the 80% full border. if we are, output the "change soon" notification
                double destinationStorageSpaceUsedNow = spaceCapacity-spaceAvailable;
                double destinationStoragePercentUsedNow = ((destinationStorageSpaceUsedNow/spaceCapacity)*100);
#if 0 //bug, if the os is doing anything else, or if two hottee instances were running on same machine, the 80% threshold might not have been seen as crossed. the 'percent after' could have been 79.9999, then the os could have made it 80.0001, then the test would have failed and the signal never would have been emitted
                double percentUsedAfterNext100mb = (((spaceUsedBeforeNext100mb+ONE_HUNDRED_MEGABYTES)/spaceCapacity)*100);
                if(percentUsedBeforeNext100Mb < EMIT_CHANGE_SOON_AT_PERCENT && percentUsedAfterNext100mb >= EMIT_CHANGE_SOON_AT_PERCENT)
#endif
                if(m_DestinationStoragePercentUsedLastTime < EMIT_CHANGE_SOON_AT_PERCENT && destinationStoragePercentUsedNow >= EMIT_CHANGE_SOON_AT_PERCENT)
                {
                    //emit 'urgent change soon' if the destination about to changed to isn't already empty/ready... by (hack) seeing if 100mb is available on it. Ideally they'd have done right when "INFO: destination changed" was seen
                    QString destinationAboutToChangeTo(m_Dest2 ? m_Destination1endingWithSlash : m_Destination2endingWithSlash);
                    boost::filesystem::path pathAboutToChangeTo(destinationAboutToChangeTo.toStdString());
                    boost::filesystem::space_info spaceInfoAboutToChangeTo = boost::filesystem::space(pathAboutToChangeTo);
                    if(spaceInfoAboutToChangeTo.available < ONE_HUNDRED_MEGABYTES) //TODOreq: what if the alternate destination is unmounted when this happens, such as when human intervention is being performed? a very serious "race condition". if it's unmounted, then we'd likely see the "underlying OS" available space and there would likely be more than 100mb available (BUT IF WE ARE MID SWAP OUT, THAT IS OKAY (as long as we finish it dur)). as long as we either do the swapping "right at change" signal (easily beating it) or "right after change soon" (racing to beat it to 100%), we _SHOULD_ never hit that race condition. *COUGH* *COUGH* BLOWJOB *COUGH*
                    {
                        emit o("URGENT: 100mb is not available on: " + destinationAboutToChangeTo);
                    }
                }
                m_DestinationStoragePercentUsedLastTime = destinationStoragePercentUsedNow;

                if(!createAndOpen100mbFileAtCurrentDestination())
                    return false;

                continue; //re-calculate sizeToRead
            }
        }
        //read input process
        QByteArray readChunk = m_InputProcess->read(sizeToRead);
        qint64 actualReadChunkSize = static_cast<qint64>(readChunk.size());
        if(actualReadChunkSize < 1)
        {
            emit d("error reading from input process");
            return false;
        }

        if(m_WriteToOutputProcess) //so ehh yea it's possible that the app is in a state where it's doing nothing but keeping track of 100mb chunk segment positions (can still start writing) and reading from inputProcess's stdout (discarding it).... but whatever....
        {
            //write to output process
            qint64 bytesWrittenToOutputProcess = m_OutputProcess->write(readChunk);
            if(bytesWrittenToOutputProcess < actualReadChunkSize)
            {
                emit d("error writing to output process");
                return false;
            }
        }

        if(m_CurrentlyWritingToEitherDestination)
        {
            //write to one of the two destinations, indicating indicators where indicated
            qint64 bytesWrittenTo100mbFile = m_CurrentOutputFile->write(readChunk);
            if(bytesWrittenTo100mbFile < actualReadChunkSize)
            {
                emit d("error writing to 100mb file: " + m_CurrentOutputFile->fileName());
                return false;
            }
        }
        m_Current100mbChunkWriteOffset += actualReadChunkSize;
        bytesAvailable -= actualReadChunkSize;
    }
}
qint64 Hottee::getPercentOf100mbChunkUsedPerMinute()
{
    //qint64 percentOfCurrentChunkWritten = (m_Current100mbChunkWriteOffset/ONE_HUNDRED_MEGABYTES);

    qint64 bytesPerMinute = ((m_Current100mbChunkWriteOffset/(QDateTime::currentMSecsSinceEpoch()-m_LastTimestampOfSyncInMS))*(1000*60));
    //since 100 is the max percent and also our chunk size, no further calculation/conversion is needed aside from converting to a 0-100% based ranged for returning
    qint64 percentBytesPerMinute = bytesPerMinute*100;
    return percentBytesPerMinute;

#if 0

    //duration of 100mb chunk
    qint64 durationOf100mbChunkInMs = ((lastSyncTimestamp-currentTimestamp)*);


    QList<qint64> previousTimestampsForAveraging;
    qint64 runningTotalOfPreviousTimestamps = 0;
    QListIterator<qint64> previousTimestampsForAveragingIterator(previousTimestampsForAveraging);
    while(previousTimestampsForAveragingIterator.hasNext())
    {
        runningTotalOfPreviousTimestamps += previousTimestampsForAveragingIterator.next();
    }
    qint64 averageOfPreviousTimestamps = 0;
    if(!previousTimestampsForAveraging.isEmpty())
    {
        averageOfPreviousTimestamps = (runningTotalOfPreviousTimestamps / previousTimestampsForAveraging.size());
    }
#endif
}
bool Hottee::folderIsntEmpty(const QString &path)
{
    QDir dir(path);
    QStringList entries = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    return (!entries.empty());
}
//tl;dr: inputProcess's stdout copied to outputProcess, and also either destinationDir 1 or 2 in 100mb chunks, notifying when size is about to, or has, run out (indicating human intervention necessary)
void Hottee::startHotteeing(const QString &inputProcessPathAndArgs, const QString &destinationDir1, const QString &destinationDir2, const QString &outputProcessPathAndArgs, qint64 outputProcessFilenameOffsetJoinPoint)
{    
    m_Destination1endingWithSlash = appendSlashIfNeeded(destinationDir1);
    m_Destination2endingWithSlash = appendSlashIfNeeded(destinationDir2);

    QDir dir1(m_Destination1endingWithSlash);
    if(!dir1.exists())
    {
        dir1.mkpath(m_Destination1endingWithSlash);
    }
    else if(folderIsntEmpty(m_Destination1endingWithSlash))
    {
        emit d("Folder not empty:" + m_Destination1endingWithSlash);
        emit quitRequested();
        return;
    }

    QDir dir2(m_Destination2endingWithSlash);
    if(!dir2.exists())
    {
        dir2.mkpath(m_Destination2endingWithSlash);
    }
    else if(folderIsntEmpty(m_Destination2endingWithSlash))
    {
        emit d("Folder not empty:" + m_Destination2endingWithSlash);
        emit quitRequested();
        return;
    }

    //NOTE: It is required that destinations are large enough so that when 80% boundary is crossed, there is at least another 100mb left... or else the app will segfault. The below >= 1gb check is my way of ensuring that. I calculated 500mb as the theoretical min, but made it 1gb instead because of the inexact nature of "crossing over" the 80% point
    if(eitherDestinationIsLessThan1gbCapacity())
    {
        emit d("Both of your destinations need to be at least 1gb in size"); //TODOoptional: for use with smaller destinations, a smaller chunk size (100mb) could be used/specifable. The 1gb check here should probably react dynamically to that chunk size in that case...
        emit quitRequested();
        return;
    }

    m_CurrentDestinationEndingWithSlash = m_Destination1endingWithSlash;
    m_Dest2 = false;
    m_StopWritingAtEndOfThisChunk = false;
    m_StartWritingAtBeginningOfNextChunk = false;
    m_QuitAfterThisChunkFinishes = false;
    m_RestartOutputProcessOnNextChunkStart = false;

    m_100mbChunkOffsetForFilename = outputProcessFilenameOffsetJoinPoint;
    if(!createAndOpen100mbFileAtCurrentDestination())
    {
        m_CurrentlyWritingToEitherDestination = false;
        emit quitRequested();
        return;
    }

    m_CurrentlyWritingToEitherDestination = true;

    m_WriteToOutputProcess = !outputProcessPathAndArgs.isEmpty();
    m_OutputProcessPathAndArgs = outputProcessPathAndArgs;

    if(m_WriteToOutputProcess)
    {
        //output process
        if(!startOutputProcess())
        {
            return;
        }
    }

    //input process
    if(m_InputProcess)
        delete m_InputProcess;
    m_InputProcess = new QProcess(this);
    m_InputProcess->setReadChannel(QProcess::StandardOutput);
    connect(m_InputProcess, SIGNAL(readyRead()), this, SLOT(handleInputProcessReadyReadStandardOutput()));
    connect(m_InputProcess, SIGNAL(readyReadStandardError()), this, SLOT(handleInputStdErr()));
    m_InputProcess->start(inputProcessPathAndArgs, QIODevice::ReadOnly);
}
void Hottee::queryChunkWriteOffsetAndStorageCapacityStuff()
{
    //TODOreq: might as well say whether or not writing is active as well
    QString ret;

    {
        //dest1 percent full
        boost::filesystem::path dest1Path(m_Destination1endingWithSlash.toStdString());
        boost::filesystem::space_info dest1SpaceInfo = boost::filesystem::space(dest1Path);
        double dest1spaceAvailable = static_cast<double>(dest1SpaceInfo.available);
        double dest1spaceCapacity = static_cast<double>(dest1SpaceInfo.capacity);
        double dest1spaceUsed = dest1spaceCapacity-dest1spaceAvailable;
        double dest1percentUsed = ((dest1spaceUsed/dest1spaceCapacity)*100);

        //dest1 percent full
        boost::filesystem::path dest2Path(m_Destination2endingWithSlash.toStdString());
        boost::filesystem::space_info dest2SpaceInfo = boost::filesystem::space(dest2Path);
        double dest2spaceAvailable = static_cast<double>(dest2SpaceInfo.available);
        double dest2spaceCapacity = static_cast<double>(dest2SpaceInfo.capacity);
        double dest2spaceUsed = dest2spaceCapacity-dest2spaceAvailable;
        double dest2percentUsed = ((dest2spaceUsed/dest2spaceCapacity)*100);

        qint64 percentOf100mbChunkUsedPerMinute = getPercentOf100mbChunkUsedPerMinute();

        QTextStream retStream(&ret, QIODevice::WriteOnly | QIODevice::Text);
        retStream   << "==Information==" << endl
                    << "Currently Writing: " << (m_CurrentlyWritingToEitherDestination ? "YES :-D" : "no") << endl
                    << "Currently pointing at Destination " << (m_Dest2 ? ("2 (" + m_Destination2endingWithSlash + ")") : ("1 (" + m_Destination1endingWithSlash + ")")) << endl
                    << "Current 100mb chunk write offset: "
                    << m_Current100mbChunkWriteOffset << " ("
                    << QString::number(((m_Current100mbChunkWriteOffset/ONE_HUNDRED_MEGABYTES)*100), 'f', 2) << "% @ "
                    << QString::number(percentOf100mbChunkUsedPerMinute, 'f', 6) << " %/minute)" << endl
                    << "Current 100mb chunk filename offset: " << m_100mbChunkOffsetForFilename << endl
                    << "Percent Full, Destination 1 (" + m_Destination1endingWithSlash + "): " << dest1percentUsed << endl
                    << "Percent Full, Destination 2 (" + m_Destination2endingWithSlash + "): " << dest2percentUsed;
    }

    emit d(ret);
}
void Hottee::startWritingAtNextChunkStart()
{
    if(m_CurrentlyWritingToEitherDestination)
    {
        emit d("You are already writing");
        return;
    }
    m_StartWritingAtBeginningOfNextChunk = true;
}
void Hottee::restartOutputProcessOnNextChunkStart()
{
    m_RestartOutputProcessOnNextChunkStart = true;
}
void Hottee::stopWritingAtEndOfThisChunk()
{
    if(!m_CurrentlyWritingToEitherDestination && !m_StartWritingAtBeginningOfNextChunk) //the bit about the !m_StartWritingAtBeginningOfNextChunk is a hack ish to make it so that if they hit start->stop before the next chunk "SYNC" (0) point, that they get what they want: not writing. the body of readInputProcessesStdOutAndWriteAccordingly is carefully crafted to "start" and then "stop" without actually creating/writing-to the the 'next' chunk
    {
        emit d("You aren't currently writing");
        return;
    }
    m_StopWritingAtEndOfThisChunk = true;
}
void Hottee::quitAfterThisChunkFinishes()
{
    m_QuitAfterThisChunkFinishes = true;
}
void Hottee::cleanupHotteeing()
{
    if(m_InputProcess)
    {
        if(m_InputProcess->isOpen())
        {
            m_InputProcess->close();
            m_InputProcess->waitForFinished(-1);
        }
        delete m_InputProcess;
        m_InputProcess = 0;
    }
    if(m_OutputProcess) //m_WriteToOutputProcess implied
    {
        if(m_OutputProcess->isOpen())
        {
            m_OutputProcess->close();
            m_OutputProcess->waitForFinished(-1);
        }
        delete m_OutputProcess;
        m_OutputProcess = 0;
    }
    if(m_CurrentOutputFile)
    {
        if(m_CurrentOutputFile->isOpen())
        {
            m_CurrentOutputFile->close();
            emit d("only wrote " + QString::number(m_Current100mbChunkWriteOffset) + " bytes in last 100mb chunk");
        }
        delete m_CurrentOutputFile;
        m_CurrentOutputFile = 0;
    }
}
void Hottee::handleInputProcessReadyReadStandardOutput()
{
    if(!readInputProcessesStdOutAndWriteAccordingly())
    {
        m_CurrentlyWritingToEitherDestination = false;
        emit d("stopped writing to destinations because of above error");
    }
}
void Hottee::handleInputStdErr()
{
    QByteArray allStdErrBA = m_InputProcess->readAllStandardError();
    QString allStdErrString(allStdErrBA);
    emit d("input process wrote to stderr: " + allStdErrString);
}
void Hottee::handleOutputProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode != 0)
    {
        emit e("Hottee's output process didn't exit with code zero: " + QString::number(exitCode));
    }
    if(exitStatus != QProcess::NormalExit)
    {
        emit e("Hottee's output process didn't exit normally" + QString::number(exitStatus));
    }

    if(!m_WriteToOutputProcess) //implied but whatever
        return;

    emit o("Hottee's output process stopped during 100mb segment # " + QString::number(m_100mbChunkOffsetForFilename));
}

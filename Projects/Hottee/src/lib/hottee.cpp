#include "hottee.h"

#include <QProcess>
#include <QTextStream>
#include <QFile>
#include <QCoreApplication>

#include <boost/filesystem.hpp> //*shakes fist at Qt*

#define READ_CHUNK_SIZE_BYTES (32*1024) //32kb
#define ONE_HUNDRED_MEGABYTES (1024*1024*100)
#define EMIT_CHANGE_SOON_AT_PERCENT 80

//TODOoptional: could also put each 100mb chunk through a hash for checksumming etc
//TODOreq: no idea how i would 'stop' this endless process... since i don't even want to! the whole purpose of the app is to keep a stream going indefinitely... so an exit point doesn't even make any sense...
Hottee::Hottee(QObject *parent) :
    QObject(parent), m_InputProcess(0), m_OutputProcess(0), m_CurrentOutputFile(0), m_Current100mbChunkWriteOffset(0), m_Dest2(false), m_100mbChunkOffsetForFilename(0)
{ }
void Hottee::toggleDestinations()
{
    emit destinationToggled(m_CurrentDestinationEndingWithSlash); //listeners of this can know that the last 100mb chunk is flushed/closed by qt (but likely still needs "sync" system call (umount should take care of that))
    if(m_Dest2)
    {
        m_Dest2 = false;
        m_CurrentDestinationEndingWithSlash = m_Destination1endingWithSlash;
        return;
    }
    m_Dest2 = true;
    m_CurrentDestinationEndingWithSlash = m_Destination2endingWithSlash;
}
void Hottee::createAndOpen100mbFileAtCurrentDestination()
{
    m_Current100mbChunkWriteOffset = 0;
    if(!m_CurrentOutputFile)
    {
        m_CurrentOutputFile = new QFile(this);
    }
    QString chunkIndexStringWithZeroPadding = QString("%1").arg(m_100mbChunkOffsetForFilename, 9, 10, QChar('0')); //width of 9 gives me ~253 years of 100mb chunks, the padding is to help with sorting ofc (although probably not necessary fuckit)
    m_CurrentOutputFile->setFileName(m_CurrentDestinationEndingWithSlash + "100mbStreamChunk_" + chunkIndexStringWithZeroPadding + ".bin"); //when daisy chaining across network, only chunk offset in filename will allow for 'matching' the filenames across systems (otherwise they would be milliseconds off)
    ++m_100mbChunkOffsetForFilename;
    if(!m_CurrentOutputFile->open(QIODevice::WriteOnly))
    {
        emit d("opening destination file for writing failed: " + m_CurrentOutputFile->fileName());
        return;
    }
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
            //flush/close current, then set up next 100mb file, then continue;
            if(!m_CurrentOutputFile->flush())
            {
                emit d("error flushing 100mb chunk file: " + m_CurrentOutputFile->fileName());
                return false;
            }
            m_CurrentOutputFile->close();

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
                    emit d("TOTAL SYSTEM FAILURE (human intervention did not occur in time)");
                    return false; //TODOreq: just disable file saving, but keep on doing input->output copying
                }
            }

            //see if we are crossing over the 80% full border. if we are, output the "change soon" notification
            double spaceUsedBeforeNext100mb = spaceCapacity-spaceAvailable;
            double percentUsedBeforeNext100Mb = ((spaceUsedBeforeNext100mb/spaceCapacity)*100);
            double percentUsedAfterNext100mb = (((spaceUsedBeforeNext100mb+ONE_HUNDRED_MEGABYTES)/spaceCapacity)*100);
            if(percentUsedBeforeNext100Mb < EMIT_CHANGE_SOON_AT_PERCENT && percentUsedAfterNext100mb >= EMIT_CHANGE_SOON_AT_PERCENT)
            {
                //emit change soon, BUT check to see that it wasn't already done (hack)... by seeing if 100mb is available on it
                QString destinationAboutToChangeTo(m_Dest2 ? m_Destination1endingWithSlash : m_Destination2endingWithSlash);
                boost::filesystem::path pathAboutToChangeTo(destinationAboutToChangeTo.toStdString());
                boost::filesystem::space_info spaceInfoAboutToChangeTo = boost::filesystem::space(pathAboutToChangeTo);
                if(spaceInfoAboutToChangeTo.available < ONE_HUNDRED_MEGABYTES) //TODOreq: what if the alternate destination is unmounted when this happens, such as when human intervention is being performed? a very serious "race condition". if it's unmounted, then we'd likely see the "underlying OS" available space and there would likely be more than 100mb available (BUT IF WE ARE MID SWAP OUT, THAT IS OKAY (as long as we finish it dur)). as long as we either do the swapping "right at change" signal (easily beating it) or "right after change soon" (racing to beat it to 100%), we _SHOULD_ never hit that race condition. *COUGH* *COUGH* BLOWJOB *COUGH*
                {
                    emit changeAlternateSoon(destinationAboutToChangeTo);
                }
            }

            createAndOpen100mbFileAtCurrentDestination();
            continue; //re-calculate sizeToRead
        }
        //read input process
        QByteArray readChunk = m_InputProcess->read(sizeToRead);
        qint64 actualReadChunkSize = static_cast<qint64>(readChunk.size());
        if(actualReadChunkSize < sizeToRead)
        {
            emit d("error reading from input process");
            return false;
        }

        //write to output process
        qint64 bytesWrittenToOutputProcess = m_OutputProcess->write(readChunk);
        if(bytesWrittenToOutputProcess < sizeToRead)
        {
            emit d("error writing to output process");
            return false;
        }

        //write to one of the two destinations, indicating indicators where indicated
        qint64 bytesWrittenTo100mbFile = m_CurrentOutputFile->write(readChunk);
        if(bytesWrittenTo100mbFile < sizeToRead)
        {
            emit d("error writing to 100mb file");
            return false;
        }
        m_Current100mbChunkWriteOffset += sizeToRead;
        bytesAvailable -= sizeToRead;
    }
}
//tl;dr: inputProcess's stdout copied to outputProcess, and also either destinationDir 1 or 2 in 100mb chunks, notifying when size is about to, or has, run out (indicating human intervention necessary)
void Hottee::startHotteeing(const QString &inputProcessPathAndArgs, const QString &outputProcessPathAndArgs, const QString &destinationDir1, const QString &destinationDir2)
{
    m_Destination1endingWithSlash = appendSlashIfNeeded(destinationDir1);
    m_Destination2endingWithSlash = appendSlashIfNeeded(destinationDir2);
    m_CurrentDestinationEndingWithSlash = m_Destination1endingWithSlash;
    m_Dest2 = false;

    createAndOpen100mbFileAtCurrentDestination();

    //output process
    if(m_OutputProcess)
        delete m_OutputProcess;
    m_OutputProcess = new QProcess(this);
    m_OutputProcess->start(outputProcessPathAndArgs, QIODevice::WriteOnly); //head start

    //input process
    if(m_InputProcess)
        delete m_InputProcess;
    m_InputProcess = new QProcess(this);
    m_InputProcess->setReadChannel(QProcess::StandardOutput);
    connect(m_InputProcess, SIGNAL(readyRead()), this, SLOT(handleInputProcessReadyReadStandardOutput()));
    connect(m_InputProcess, SIGNAL(readyReadStandardError()), this, SLOT(handleInputStdErr()));
    m_InputProcess->start(inputProcessPathAndArgs, QIODevice::ReadOnly);

    m_OutputProcess->waitForStarted(); //now wait. we don't want to readyRead input until output is ready for writing
}
void Hottee::stopHotteeing()
{
    if(m_InputProcess)
    {
        if(m_InputProcess->isOpen())
        {
            m_InputProcess->close();
            m_InputProcess->waitForFinished();
        }
        delete m_InputProcess;
        m_InputProcess = 0;
    }
    if(m_OutputProcess)
    {
        if(m_OutputProcess->isOpen())
        {
            m_OutputProcess->close();
            m_OutputProcess->waitForFinished();
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
        stopHotteeing();
        QCoreApplication::quit();
    }
}
void Hottee::handleInputStdErr()
{
    emit d("input process wrote to stderr: " + QString(m_InputProcess->readAllStandardError()));
}

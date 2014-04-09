#include "hotteecli.h"

#include <QCoreApplication>
#include <QStringList>

HotteeCli::HotteeCli(QObject *parent) :
    QObject(parent), m_StdInSocketNotifier(0, QSocketNotifier::Read), m_StdIn(stdin, QIODevice::ReadOnly), m_StdOut(stdout, QIODevice::WriteOnly)
{
    connect(&m_StdInSocketNotifier, SIGNAL(activated(int)), this, SLOT(stdInHasLineOfInput()));

    connect(&m_Hottee, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(&m_Hottee, SIGNAL(o(QString)), this, SLOT(handleD(QString)));
    connect(this, SIGNAL(startHotteeingRequested(QString,QString,QString,QString)), &m_Hottee, SLOT(startHotteeing(QString,QString,QString,QString)));
    connect(this, SIGNAL(queryChunkWriteOffsetAndStorageCapacityStuffRequested()), &m_Hottee, SLOT(queryChunkWriteOffsetAndStorageCapacityStuff()));
    connect(this, SIGNAL(startWritingAtNextChunkStartRequested()), &m_Hottee, SLOT(startWritingAtNextChunkStart()));
    //connect(this, SIGNAL(quitNowRequested()), &m_Hottee, SLOT(cleanupHotteeing()), Qt::DirectConnection); //even though direct is already implied, I'm specifying it because it's imperitive that the signal emitter wait for cleanupHotteeing to finish. In this CLI/same-thread usage, DirectConnection can/should/is-already used... BUT for the gui/backend-thread design, the connection must be established with Qt::BlockingQueuedConnection. I erroneously had this cli/same-thread one using BlockingQueuedConnection, but that was a deadlock obviously. "Auto" here would fuck shit up when using the backend thread (gui) design. I almost want to suggest an enum to be added to Qt's framework: DirectIfSameThreadBlockingQueuedIfOtherThread. "kinda" like auto, except not...
    //^^actually, i think just calling cleanupHotteeing() in destructor solves all of this...
    connect(this, SIGNAL(stopWritingAtEndOfThisChunkRequested()), &m_Hottee, SLOT(stopWritingAtEndOfThisChunk())); //TODOreq: does mroe or less same code that all the errors do
    connect(this, SIGNAL(quitAfterThisChunkFinishesRequested()), &m_Hottee, SLOT(quitAfterThisChunkFinishes()));

    QStringList args = QCoreApplication::instance()->arguments();
    if(args.size() < 4 || args.size() > 5)
    {
        cliUsage();
        QMetaObject::invokeMethod(QCoreApplication::instance(), SLOT(quit()));
        return;
    }

    QString outputProcessMb;
    if(args.size() > 4)
    {
        outputProcessMb = args.at(4);
    }
    emit startHotteeingRequested(args.at(1), args.at(2), args.at(3), outputProcessMb);
    handleD("Hottee is starting");
    cliUserInterfaceMenu();
}
void HotteeCli::cliUserInterfaceMenu()
{
    handleD("Available Actions (H to show this again):");
    handleD("\t0\t- Query chunk write offset and storage device capacity/availability");
    handleD("\t1\t- Start writing at next chunk start");
    //doesn't make sense: handleD("\t2 - Start writing at next de")
    handleD("\t2\t- Stop writing after current 100mb chunk ends");
    handleD("\tqq\t- Quit NOW (mid-chunk). Use 'q' instead if you can.");
    handleD("\tq\t- Quit after current 100mb chunk ends");
    //TODOoptional: handleD("\t9 - Quit when current destination fills up");
}
void HotteeCli::cliUsage()
{
    handleD("Usage:");
    handleD("HotteeCli inputProcessAndArgs destination1 destination2 [outputProcessAndArgs]");
    handleD("");
    handleD("Surround input/output process args with spaces in quotes. Destination 1 is intended to be on a different physical drive than destination 2 (both are intended to be external devices, not on the OS drive). When destination 1 becomes 80% full, a message is shown if destination 2 is not seen as empty. When no more data can be written to destination 1, destination 2 is used and the cycle repeats indefinitely. Destination 1 must be replaced with an empty drive by the time destination 2 reaches 100% otherwise the process fails");
    handleD("");
    handleD("Example: HotteeCli \"netcat blahInput\" /mnt/externalDrive1 /mnt/externalDrive2 \"netcat blahOutput\"");
    handleD("^reads inputProcess's stdin and writes it in 100mb file chunks onto /mnt/external1 drive until it is full, then switches over to /mnt/external2. Also sends a copy of the stdin to another netcat, daisy chaining them");
}
void HotteeCli::handleD(const QString &msg)
{
    m_StdOut << msg << endl;
}
void HotteeCli::stdInHasLineOfInput()
{
    QString lineOfInput = m_StdIn.readLine().trimmed().toLower();
    if(lineOfInput == "h")
    {
        cliUserInterfaceMenu();
    }
    else if(lineOfInput == "0")
    {
        emit queryChunkWriteOffsetAndStorageCapacityStuffRequested(); //current chunk write offset, and capacity of BOTH destinations
    }
    else if(lineOfInput == "1")
    {
        emit startWritingAtNextChunkStartRequested();
    }
    else if(lineOfInput == "2")
    {
        emit stopWritingAtEndOfThisChunkRequested();
    }
    else if(lineOfInput == "qq")
    {
        disconnect(&m_StdInSocketNotifier, SIGNAL(activated(int)));
        QMetaObject::invokeMethod(QCoreApplication::instance(), SLOT(quit()));
    }    
    else if(lineOfInput == "q")
    {
        disconnect(&m_StdInSocketNotifier, SIGNAL(activated(int)));
        emit quitAfterThisChunkFinishesRequested();
        handleD("Hottee is going to quit after this chunk finishes...");
    }
    else
    {
        cliUserInterfaceMenu();
        handleD("Invalid selection: '" + lineOfInput + "'");        
    }
}

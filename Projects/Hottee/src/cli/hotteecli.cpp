#include "hotteecli.h"

#include <QCoreApplication>
#include <QStringList>

#include "../lib/hottee.h"

HotteeCli::HotteeCli(QObject *parent)
    : QObject(parent)
    , m_StdInSocketNotifier(0, QSocketNotifier::Read)
    , m_StdIn(stdin, QIODevice::ReadOnly)
    , m_StdOut(stdout, QIODevice::WriteOnly)
    , m_Hottee(new Hottee(this))
{
    connect(&m_StdInSocketNotifier, SIGNAL(activated(int)), this, SLOT(stdInHasLineOfInput()));

    connect(m_Hottee, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(m_Hottee, SIGNAL(o(QString)), this, SLOT(handleD(QString)));
    connect(m_Hottee, SIGNAL(e(QString)), this, SLOT(handleD(QString)));
    connect(m_Hottee, SIGNAL(quitRequested()), this, SLOT(handleQuitRequested()));
    connect(this, SIGNAL(startHotteeingRequested(QString,QString,QString,QString,qint64)), m_Hottee, SLOT(startHotteeing(QString,QString,QString,QString,qint64)));
    connect(this, SIGNAL(queryChunkWriteOffsetAndStorageCapacityStuffRequested()), m_Hottee, SLOT(queryChunkWriteOffsetAndStorageCapacityStuff()));
    connect(this, SIGNAL(startWritingAtNextChunkStartRequested()), m_Hottee, SLOT(startWritingAtNextChunkStart()));
    //connect(this, SIGNAL(quitNowRequested()), &m_Hottee, SLOT(cleanupHotteeing()), Qt::DirectConnection); //even though direct is already implied, I'm specifying it because it's imperitive that the signal emitter wait for cleanupHotteeing to finish. In this CLI/same-thread usage, DirectConnection can/should/is-already used... BUT for the gui/backend-thread design, the connection must be established with Qt::BlockingQueuedConnection. I erroneously had this cli/same-thread one using BlockingQueuedConnection, but that was a deadlock obviously. "Auto" here would fuck shit up when using the backend thread (gui) design. I almost want to suggest an enum to be added to Qt's framework: DirectIfSameThreadBlockingQueuedIfOtherThread. "kinda" like auto, except not...
    //^^actually, i think just calling cleanupHotteeing() in destructor solves all of this...
    connect(this, SIGNAL(restartOutputProcessOnNextChunkStartRequested()), m_Hottee, SLOT(restartOutputProcessOnNextChunkStart()));
    connect(this, SIGNAL(stopWritingAtEndOfThisChunkRequested()), m_Hottee, SLOT(stopWritingAtEndOfThisChunk())); //TODOreq: does mroe or less same code that all the errors do
    connect(this, SIGNAL(quitAfterThisChunkFinishesRequested()), m_Hottee, SLOT(quitAfterThisChunkFinishes()));

    QStringList args = QCoreApplication::instance()->arguments();

    if(args.size() < 4 || args.size() > 6)
    {
        cliUsage();
        handleQuitRequested();
        return;
    }

    QString outputProcessMb;
    if(args.size() > 4)
    {
        outputProcessMb = args.at(4);
    }
    qint64 outputProcessFilenameOffsetJoinPoint = 0;
    if(args.size() > 5)
    {
        bool convertOk = false;
        qint64 temp = args.at(5).toLongLong(&convertOk);
        if(convertOk && temp > 0)
            outputProcessFilenameOffsetJoinPoint = temp;
    }
    emit startHotteeingRequested(args.at(1), args.at(2), args.at(3), outputProcessMb, outputProcessFilenameOffsetJoinPoint);
    handleD("Hottee is starting");
    cliUserInterfaceMenu();
}
void HotteeCli::cliUserInterfaceMenu()
{
    QString ret =   "Available Actions (H to show this again):\n"
                    "\t0\t- Query chunk write offset and storage device capacity/availability\n"
                    "\t1\t- Start writing at next chunk start\n"
                    //doesn't make sense: handleD("\t2 - Start writing at next de")
                    "\t2\t- Stop writing after current 100mb chunk ends\n"
                    "t3\t- Restart Output Process at next chunk start (use 0 to make sure chunk index isn't about to change before calling)" //TODOoptional: make more fail-safe, allow and recover from "missing the index" (destination netcat didn't open in time or some shit)
                    "\tqq\t- Quit NOW (mid-chunk). Use 'q' instead if you can.\n"
                    "\tq\t- Quit after current 100mb chunk ends (or when the input process closes)\n";
    //TODOoptional: handleD("\t9 - Quit when current destination fills up");
    handleD(ret);
}
void HotteeCli::cliUsage()
{
    handleD("Usage:");
    handleD("HotteeCli inputProcessAndArgs destination1 destination2 [outputProcessAndArgs=\"\"] [indexForFilenameOf100mbChunk=0]");
    handleD("");
    handleD("Surround input/output process args with spaces in quotes. Destination 1 is intended to be on a different physical drive than destination 2 (both are intended to be external devices, not on the OS drive). When destination 1 becomes 80% full, a message is shown if destination 2 is not seen as empty. When no more data can be written to destination 1, destination 2 is used and the cycle repeats indefinitely. Destination 1 must be replaced with an empty drive by the time destination 2 reaches 100% otherwise the process fails");
    handleD("");
    handleD("Example: HotteeCli \"netcat blahInput\" /mnt/externalDrive1 /mnt/externalDrive2 \"netcat blahOutput\" 69");
    handleD("^reads inputProcess's stdin and writes it in 100mb file chunks onto /mnt/external1 drive until it is full, then switches over to /mnt/external2. Also sends a copy of the stdin to another netcat, daisy chaining them. The 69 indicates that the filename offset for the 100mb chunk, whose presence tells use we are joining not from the beginning");
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
        return;
    }
    if(lineOfInput == "0")
    {
        emit queryChunkWriteOffsetAndStorageCapacityStuffRequested(); //current chunk write offset, and capacity of BOTH destinations
        return;
    }
    if(lineOfInput == "1")
    {
        emit startWritingAtNextChunkStartRequested();
        return;
    }
    if(lineOfInput == "2")
    {
        emit stopWritingAtEndOfThisChunkRequested();
        return;
    }
    if(lineOfInput == "3")
    {
        emit restartOutputProcessOnNextChunkStartRequested();
        return;
    }
    if(lineOfInput == "qq")
    {
        disconnect(&m_StdInSocketNotifier, SIGNAL(activated(int)));
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
        return;
    }    
    if(lineOfInput == "q")
    {
        disconnect(&m_StdInSocketNotifier, SIGNAL(activated(int)));
        emit quitAfterThisChunkFinishesRequested();
        handleD("Hottee is going to quit after this chunk finishes (or when the input process closes)...");
        return;
    }
    cliUserInterfaceMenu();
    handleD("Invalid selection: '" + lineOfInput + "'");
}
void HotteeCli::handleQuitRequested()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}

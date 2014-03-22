#include "hotteecli.h"

#include <QCoreApplication>
#include <QStringList>

HotteeCli::HotteeCli(QObject *parent) :
    QObject(parent), m_StdIn(stdin), m_StdOut(stdout)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleHotteeReadyForConnections()));
    m_BusinessThread.start();
}
void HotteeCli::usage()
{
    handleD("Usage:");
    handleD("HotteeCli inputProcessAndArgs destination1 destination2 [outputProcessAndArgs]");
    handleD("");
    handleD("Surround input/output process args with spaces in quotes. Destination 1 is intended to be on a different physical drive than destination 2 (both are intended to be external devices, not on the OS drive). When destination 1 becomes 80% full, a message is shown if destination 2 is not seen as empty. When no more data can be written to destination 1, destination 2 is used and the cycle repeats indefinitely. Destination 1 must be replaced with an empty drive by the time destination 2 reaches 100% otherwise the process fails");
    handleD("");
    handleD("Example: HotteeCli \"netcat blahInput\" /mnt/externalDrive1 /mnt/externalDrive2 \"netcat blahOutput\"");
    handleD("^reads inputProcess's stdin and writes it in 100mb file chunks onto /mnt/external1 drive until it is full, then switches over to /mnt/external2. Also sends a copy of the stdin to another netcat, daisy chaining them");
}
void HotteeCli::cliUserInterfaceLoop()
{
    handleD("Available Actions (H to show this again):");
    handleD("\t0 - Query chunk write offset and storage device capacity/availability");
    handleD("\t1 - Start writing at next chunk start");
    //doesn't make sense: handleD("\t2 - Start writing at next de")
    //TODOoptional(ugly): quit NOW (mid-chunk)
    handleD("\t7 - Stop writing after current 100mb chunk ends");
    handleD("\t8 - Quit after current 100mb chunk ends");
    //TODOoptional: handleD("\t9 - Quit when current destination fills up");

    handleD("");

    //oh boy cin, it's been a while <3
    QString x;
    x = m_StdIn.readLine().trimmed();

    if(x == "h" || x == "H")
    {
        //do nothing, we are already in a loop (this is here because tons of output from backend thread could have pushed the available actions text up and off the screen)
    }
    else if(x == "0")
    {
        emit queryChunkWriteOffsetAndStorageCapacityStuffRequested(); //current chunk write offset, and capacity of BOTH destinations
    }
    else if(x == "1")
    {
        emit startWritingAtNextChunkStartRequested();
    }
    else if(x == "7")
    {
        emit stopWritingAtEndOfThisChunkRequested();
    }
    else if(x == "8")
    {
        emit quitAfterThisChunkFinishesRequested();
        handleD("Hottee is stopping...");
        return;
    }
    else
    {
        handleD("Invalid selection");
    }
    cliUserInterfaceLoop();
}
void HotteeCli::handleD(const QString &msg)
{
    m_StdOut << msg << endl;
}
void HotteeCli::handleHotteeReadyForConnections()
{
    Hottee *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(business, SIGNAL(o(QString)), this, SLOT(handleD(QString)));
    connect(this, SIGNAL(startHotteeingRequested(QString,QString,QString,QString)), business, SLOT(startHotteeing(QString,QString,QString,QString)));
    connect(this, SIGNAL(queryChunkWriteOffsetAndStorageCapacityStuffRequested()), business, SLOT(queryChunkWriteOffsetAndStorageCapacityStuff()));
    connect(this, SIGNAL(startWritingAtNextChunkStartRequested()), business, SLOT(startWritingAtNextChunkStart()));
    connect(this, SIGNAL(stopWritingAtEndOfThisChunkRequested()), business, SLOT(stopWritingAtEndOfThisChunk())); //TODOreq: does mroe or less same code that all the errors do
    connect(this, SIGNAL(quitAfterThisChunkFinishesRequested()), business, SLOT(quitAfterThisChunkFinishes()));

    QStringList args = QCoreApplication::instance()->arguments();
    if(args.size() < 4 || args.size() > 5)
    {
        usage();
        QCoreApplication::quit();
        return;
    }

    QString outputProcessMb;
    if(args.size() > 4)
    {
        outputProcessMb = args.at(4);
    }
    emit startHotteeingRequested(args.at(1), args.at(2), args.at(3), outputProcessMb);
    handleD("Hottee is starting");
    cliUserInterfaceLoop();
}
void HotteeCli::handleAboutToQuit() //bleh what's the use if ctrl+c isn't caught...
{
    m_BusinessThread.quit(); //this [cli] design beats all the ones i've made previously, but incurs the extra thread cost i don't think cli apps should pay. still, i'll take elegant and cleanly shutting down over performant and leaky (until i can figure something better out)
    m_BusinessThread.wait();
    handleD("Hottee has stopped");
}

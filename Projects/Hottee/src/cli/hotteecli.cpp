#include "hotteecli.h"

#include <QCoreApplication>
#include <QStringList>

#include "../lib/hottee.h"

HotteeCli::HotteeCli(QObject *parent) :
    QObject(parent), m_StdOut(stdout), m_Hottee(0)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
}
void HotteeCli::startHotteeAndWaitUntilDone()
{
    QStringList args = QCoreApplication::instance()->arguments();
    if(args.size() != 5)
    {
        usage();
        QCoreApplication::quit();
        return;
    }
    if(m_Hottee)
        delete m_Hottee;
    m_Hottee = new Hottee(this);
    connect(m_Hottee, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(m_Hottee, SIGNAL(changeAlternateSoon(QString)), this, SLOT(handleChangeAlternateSoon(QString)));
    connect(m_Hottee, SIGNAL(destinationToggled(QString)), this, SLOT(handleDestinationToggled(QString)));
    m_Hottee->startHotteeing(args.at(1), args.at(2), args.at(3), args.at(4));
}
void HotteeCli::usage()
{
    handleD("Usage:");
    handleD("HotteeCli inputProcess outputProcess destination1 destination2");
    handleD("");
    handleD("No arguments are optional. Surround input/output process args with spaces in quotes. Destination 1 is intended to be on a different physical drive than destination 2 (both are intended to be external devices, not on the OS drive). When destination 1 becomes 80% full, a message is shown if destination 2 is not seen as empty. When no more data can be written to destination 1, destination 2 is used and the cycle repeats indefinitely. Destination 1 must be replaced with an empty drive by the time destination 2 reaches 100% otherwise the process fails");
    handleD("");
    handleD("Example: HotteeCli \"netcat blahInput\" \"netcat blahOutput\" /mnt/externalDrive1 /mnt/externalDrive2");
}
void HotteeCli::handleD(const QString &msg)
{
    m_StdOut << msg << endl;
}
void HotteeCli::handleChangeAlternateSoon(const QString &destinationAboutToChangeTo)
{
    handleD("URGENT: 100mb is not available on " + destinationAboutToChangeTo);
}
void HotteeCli::handleDestinationToggled(const QString &destinationChangedFrom)
{
    handleD("Info: Just changed from " + destinationChangedFrom);
}
void HotteeCli::handleAboutToQuit() //bleh what's the use if ctrl+c isn't caught...
{
    if(m_Hottee)
    {
        m_Hottee->stopHotteeing();
    }
}

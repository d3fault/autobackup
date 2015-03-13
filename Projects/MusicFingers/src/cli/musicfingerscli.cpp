#include "musicfingerscli.h"

#include <QCoreApplication>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

#include "standardinputnotifier.h"
#include "musicfingersserialportintegration.h"
#include "musicfingers.h"

MusicFingersCli::MusicFingersCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    QList<QSerialPortInfo> availPorts = QSerialPortInfo::availablePorts();
    QSerialPortInfo chosenSerialPortInfo;
    if(availPorts.isEmpty())
    {
        qWarning() << "No serial ports available";
        return;
    }
    else if(availPorts.size() > 1)
    {
        handleO("Choose your serial port (empty line or 'q' exits):");
        handleO("");
        int currentPortIndex = 0;
        Q_FOREACH(const QSerialPortInfo &currentPort, availPorts)
        {
            handleO("\t" + QString::number(currentPortIndex) + " - " + currentPort.portName() + " (" + currentPort.description() + " (" + currentPort.systemLocation() + "))");
            ++currentPortIndex;
        }
        handleO("");

        QTextStream stdIn(stdin);
        QString line;
        do
        {
            line = stdIn.readLine();

        }while(!serialPortSelectionIsValid(line, availPorts.size()));

        if(line.isNull() || line.isEmpty() || line.toLower().startsWith("q"))
        {
            quit();
            return;
        }
        int chosenSerialPortIndex = line.toInt(); //already sanitized, will convert
        chosenSerialPortInfo = availPorts.at(chosenSerialPortIndex);
    }
    else //exactly 1 serial port
    {
        chosenSerialPortInfo = availPorts.at(0);
    }

    MusicFingersSerialPortIntegration *musicFingersSerialPortIntegration = new MusicFingersSerialPortIntegration(chosenSerialPortInfo, this);
    connect(musicFingersSerialPortIntegration, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    if(!musicFingersSerialPortIntegration->open(QIODevice::ReadOnly))
    {
        handleE("failed to open serial port: " + chosenSerialPortInfo.portName() + " (" + chosenSerialPortInfo.description() + " (" + chosenSerialPortInfo.systemLocation() + "))");
        quit();
        return;
    }

    StandardInputNotifier *sin = new StandardInputNotifier(this);
    connect(sin, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputLineReceived(QString)));

    MusicFingers *musicFingers = new MusicFingers(this);
    connect(musicFingersSerialPortIntegration, SIGNAL(fingerMoved(Finger::FingerEnum,int)), musicFingers, SLOT(moveFinger(Finger::FingerEnum,int)));
    connect(musicFingers, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(musicFingers, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(musicFingers, SIGNAL(quitRequested()), this, SLOT(quit()));
    connect(this, SIGNAL(queryMusicFingerPositionRequested(int)), musicFingers, SLOT(queryMusicFingerPosition(int)));
}
void MusicFingersCli::cliUsage()
{
    handleE("invalid user input: valid values are 0-9");
}
bool MusicFingersCli::serialPortSelectionIsValid(const QString &serialPortSelection, int numSerialPorts)
{
    if(serialPortSelection.isNull() || serialPortSelection.isEmpty() || serialPortSelection.toLower().startsWith("q"))
    {
        return true;
    }
    int enteredNum = -1;
    bool convertOk = false;
    enteredNum = serialPortSelection.toInt(&convertOk);
    if(!convertOk)
    {
        handleE("invalid input. enter a number");
        return false;
    }
    if(enteredNum < 0 || enteredNum > (numSerialPorts-1))
    {
        handleE("invalid input. no serial port at that index");
        return false;
    }
    return true;
}
void MusicFingersCli::handleStandardInputLineReceived(const QString &line)
{
    if(line == "q")
    {
        quit();
        return;
    }
    bool convertOk = false;
    int fingerIndexToQueryValueOf = line.toInt(&convertOk);
    if((!convertOk) || (!MusicFingers::isValidFingerId(fingerIndexToQueryValueOf)))
    {
        cliUsage();
        return;
    }
    emit queryMusicFingerPositionRequested(fingerIndexToQueryValueOf);
}
void MusicFingersCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void MusicFingersCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void MusicFingersCli::quit()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}

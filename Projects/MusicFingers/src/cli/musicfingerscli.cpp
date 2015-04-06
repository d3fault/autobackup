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
    CHOOSE_MUSIC_FINGERS_SERIAL_PORT_INFO_OR_ELSEANDRETURN(chosenSerialPortInfo, quit())

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
void MusicFingersCli::handleStandardInputLineReceived(const QString &line)
{
    if(line == "q")
    {
        quit();
        return;
    }
    bool convertOk = false;
    int fingerIndexToQueryValueOf = line.toInt(&convertOk);
    if((!convertOk) || (!Finger::isValidFingerId(fingerIndexToQueryValueOf)))
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

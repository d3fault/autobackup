#include "musicfingerssynthesistoolkit.h"

#include <QCoreApplication>

#include "musicfingersserialportintegration.h"

//decided that since this is most likely going to be a hacked together piece of shit, it should be a prototype (but in all fairness, life itself is a hacked together piece of shit [and there's still no `proper`)
MusicFingersSynthesisToolkit::MusicFingersSynthesisToolkit(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{ }
void MusicFingersSynthesisToolkit::startSynthesizingToStkToolkitStdinFromMusicFingersSerialPort()
{
    connect(this, &MusicFingersSynthesisToolkit::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

    CHOOSE_MUSIC_FINGERS_SERIAL_PORT_INFO_OR_ELSEANDRETURN(chosenSerialPortInfo, emit exitRequested(1))

    MusicFingersSerialPortIntegration *musicFingersSerialPortIntegration = new MusicFingersSerialPortIntegration(chosenSerialPortInfo, this);
    connect(musicFingersSerialPortIntegration, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    if(!musicFingersSerialPortIntegration->open(QIODevice::ReadOnly))
    {
        handleE("failed to open serial port: " + chosenSerialPortInfo.portName() + " (" + chosenSerialPortInfo.description() + " (" + chosenSerialPortInfo.systemLocation() + "))");
        emit exitRequested(1);
        return;
    }
    connect(musicFingersSerialPortIntegration, SIGNAL(fingerMoved(Finger::FingerEnum,int)), this, SLOT(handleFingerMoved(Finger::FingerEnum,int)));
}
void MusicFingersSynthesisToolkit::handleO(const QString &msg)
{
    //m_StdOut << msg << endl;
    m_StdErr << msg << endl; //we pipe our output, so...
}
void MusicFingersSynthesisToolkit::handleE(const QString &msg)
{
    m_StdErr << msg;
}
void MusicFingersSynthesisToolkit::handleFingerMoved(Finger::FingerEnum finger, int position)
{
    //hmm, do i want to control stk-demo or do I want to just output skini.... I guess I should choose the latter since it's inherently more modular
    m_StdOut << "PitchChange 0 " << QString::number(finger) /*TODOoptional: fingerToFingerIndex*/ << " " << position;
    m_StdOut.flush(); //send that bitch
}

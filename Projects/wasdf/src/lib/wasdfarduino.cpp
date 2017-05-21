#include "wasdfarduino.h"

#include <QtSerialPort/QSerialPort>
#include <QStringList>

#include "wasdf.h"

//so now for the protocol. at least [so far (and I intend to keep it that way!)] the protocol is relatively simple. I need to also keep in mind that when the Arduino is in mouse/keyboard mode that it doesn't use the Serial.write shit (it could, but then I'd need a priveledged keyboard-press/mouse-movement-n-click daemon-thing on the PC side, and those daemon-things are OS specific!)

//Modes
//1) Calibrating
//2) Report finger movements verbosely over serial
//3) Keyboard/Mouse

//are (2) and (3) mutually exclusive? idfk tbh so I'll try to make it so they can both be used simultaneously (I seem to recall a fantasy where I'm coding and making music at the same time? in fact that is part of my 'perfect moment' (goal) in life so yea they are not/shouldn't be mutually exclusive (unless I'm just toggling modes ultra fast in that fantasy. since the fantasy hasn't happened yet I'm free to define it however I please)

//hmm json, binary, csv(s/comma/colon/newline/etc), hmm. time to research what kind of serialization arduino comes with, since it's libraries are way more limited than the PC sides of things
//I want to use json but I don't see any built-in libs and I'm sure as shit not about to write a parser. there are very likely lightweight self-contained pure c/c++ libraries I could just #include, but I'm pretty sure this arduino micro has.... a micro amount of space available. mb just 8kb compiled? *checks*. oh nice 28kb, that should be enough for anyone (ziiiiiing)
//meh after researching a decent sounding MIT-licensed ArduionJson library... and thinking about it more in the process... I don't think my protocol will be so complex to warrant json. I'll keep json in mind in case things start to get complex, but meh just a plaintext+colon+newlines protocol (like in MusicFingers, though obviously more robust since calibration mode must exist) sounds sufficient. I'll think about it more while I eat dinner (clam chowda)
//another way I could do things is to have 1 sketch for calibration and 1 sketch for normal operation, that would both give me more room to code the sketches and it would get dead (calibration) code paths out of normal operation
//TODOreq: the arduino could persist calibration/configuration to it's EEPROM, which would allow users to take their arduino around to various computers and use them without needing to recalibrate. The protocol would need to be more robust (we first query the arduino to see if it has a stored conf), but it wouldn't be too difficult to implement
WasdfArduino::WasdfArduino(QObject *parent)
    : QObject(parent)
    , m_SerialPort(new QSerialPort(this))
    , m_SerialPortTextStream(m_SerialPort)
{
    m_SerialPort->setBaudRate(QSerialPort::Baud9600); //TODOoptional: QSerialPortInfo supplies a list of "standard baud rates" that the serial port supports
}
void WasdfArduino::openSerialPortIfNotOpen()
{
    if(m_SerialPort->isOpen())
    {
        if(!m_SerialPort->open(QIODevice::ReadWrite /*QIODevice::Text <-- apparently not supported, but I guess it's text by default?*/))
        {
            emit e("failed to open serial port \"" + m_SerialPort->portName() + "\" for reading/writing"); //TODOreq: connect all output/error signals (not just this class)
            //TODOreq: error out, return false or emit finished(false) etc
        }
    }
}
void WasdfArduino::startInCalibrationMode()
{
    //in calibration mode, arduino sends _ALL_ (not just 10) analog pin readings over serial

    disconnect(m_SerialPort, SIGNAL(readyRead())); //TODOreq: for some reason this fails with qt5-style pmf syntax. maybe a newer version of Qt doesn't (otherwise file a bugreport)
    connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadCalibrationMode);

    openSerialPortIfNotOpen();

    m_SerialPortTextStream << "calibrate" << endl; //TODOmb: these strings should be in a shared header, shared between the arduino sketch and this sauce
    m_SerialPortTextStream.flush(); //TODOmb: encapsulate writes so I can't forget to flush (and psbly an endl)
}
void WasdfArduino::start(const WasdfCalibrationConfiguration &calibrationConfig)
{
    //TODOreq: this is where the 10 fingers get mapped to the 10 pins

    disconnect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadCalibrationMode);
    connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadNormalFingerMovementMode);

    openSerialPortIfNotOpen();

    m_SerialPortTextStream << "start[BLAH:MAP:0:5:A7:ETC:TODOreq]" << endl;
    m_SerialPortTextStream.flush();
}
void WasdfArduino::handleSerialPortReadyReadCalibrationMode()
{
    while(m_SerialPort->canReadLine())
    {
        QString line = m_SerialPortTextStream.readLine();
        //line is simply: "[pinId]:[sensorValue]"
        QStringList lineSplitAtColon = line.split(":");
        Q_ASSERT(lineSplitAtColon.size() == 2); //TODOmb: instead of these asserts, we should just silently ignore errors? still this is useful for debugging
        bool stringToIntConvertedOk;
        int analogPinId = lineSplitAtColon.first().toInt(&stringToIntConvertedOk);
        Q_ASSERT(stringToIntConvertedOk);
        int sensorValue = lineSplitAtColon.at(1).toInt(&stringToIntConvertedOk);
        Q_ASSERT(stringToIntConvertedOk);
        emit analogPinReadingChangedDuringCalibration(analogPinId, sensorValue);
    }
}
void WasdfArduino::handleSerialPortReadyReadNormalFingerMovementMode()
{
    //TODOreq:
}

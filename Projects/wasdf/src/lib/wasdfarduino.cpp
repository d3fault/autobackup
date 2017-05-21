#include "wasdfarduino.h"

#include <QtSerialPort/QSerialPort>

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
WasdfArduino::WasdfArduino(QObject *parent)
    : QObject(parent)
    , m_SerialPort(new QSerialPort(this))
{
    connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyRead);
}
void WasdfArduino::startInCalibrationMode()
{
    //TODOreq: arduino sends _ALL_ (not just 10) analog pin readings over serial
}
void WasdfArduino::start(const WasdfCalibrationConfiguration &calibrationConfig)
{
    //TODOreq: this is where the 10 fingers get mapped to the 10 pins
}
void WasdfArduino::handleSerialPortReadyRead()
{
    //TODOreq:
}

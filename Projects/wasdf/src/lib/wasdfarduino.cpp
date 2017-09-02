#include "wasdfarduino.h"

#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QHashIterator>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

#include "qtiodevicechecksummedmessagereader.h"

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
//TODOreq: I have a choice of simply telling the arduino "hey we want values for these 10 pins" and then keeping the pin-to-fingers map local to the PC, or I can say "hey we want values for these 10 pins which correspond to these fingers". It makes very little difference AT FIRST (esp since both are C++ environments I recently found out (I thought arduino was C only xD)), but considering the t0d0r3q directly above this one (persisting calibration to eeprom), I should then go with the second approach of sending more of the information to the arduino. so once started, I should receive messages like this across serial: "finger0 moved to position 69" (compared to "analog pin 18 moved to position 420"). I do/will have a copy of that pin-to-fingers map on the PC side of course, but I no longer need it once we're up and running (and I suppose if we read calibration config from eeprom, the PC never even needs to know it TODOreq: make sure to never use it then if that's the case (might be safer just to read it just in case we accidentally use it)!!!) <-- disregard that actually we do want to use the calibration on the PC side, we want to constrain (map, actually (maybe both)) the reported sensor values to the determined calibrated min/max values
WasdfArduino::WasdfArduino(QObject *parent)
    : QObject(parent)
    , m_SerialPort(new QSerialPort("/dev/ttyACM0", this)) //TODOreq: be able to choose different serial ports at runtime, MusicFingers has some of this implemented (using ugly defines kek) and some decent ideas such as "use the _one_ containing the string 'arduino'" etc
    , m_SerialPortTextStream(m_SerialPort)
{
    m_SerialPort->setBaudRate(QSerialPort::Baud38400);

    connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::dumpRawSerialStreamByPeeking);
    m_ChecksummedMessageReader = new QtIoDeviceChecksummedMessageReader(m_SerialPort, this); //this line has to go after the above connect() because otherwise the QtIoDeviceChecksummedMessageReader::handleReadyRead will be called before dumpRawSerialStreamByPeeking, and in that case the peeking won't work!
}
void WasdfArduino::openSerialPortIfNotOpen()
{
    if(!m_SerialPort->isOpen())
    {
        if(!m_SerialPort->open(QIODevice::ReadWrite /*QIODevice::Text <-- apparently not supported, but I guess it's text by default? <- nope it's binary but eh ascii isn't so hard to work with anyways. my protocol will be pure ascii*/))
        {
            emit e("failed to open serial port \"" + m_SerialPort->portName() + "\" for reading/writing");
            //TODOreq: error out, return false or emit finished(false) etc
        }
    }
}
QString WasdfArduino::jsonObjectToQString(const QJsonObject &jsonObject)
{
    QJsonDocument jsonDocument(jsonObject);
    QString ret(jsonDocument.toJson(QJsonDocument::Compact));
    return ret;
}
void WasdfArduino::sendRawCommandToArduino(const QString &commandToSendToArduino)
{
    qDebug() << "commandToSendToArduino:" << commandToSendToArduino; //TODOreq: this is only here for testing

    //send SYNC, then checksumOfSizeOfData, then sizeOfData, then checksumOfData, then commandToSendToArduino, which is a json document in string form
    static const QString delim(WASDF_CHECKSUMMED_MESSAGE_HEADER_COMMADELIM); //TODOreq: in shared headers

    //send SYNC
    m_SerialPortTextStream << "SYNC";
    m_SerialPortTextStream << delim;

    //send checksumOfSizeOfData
    QString sizeofDataAsString = QString::number(commandToSendToArduino.size());
    QByteArray sizeOfCommandAsByteArray(sizeofDataAsString.toLatin1());
    QByteArray checksumOfSizeOfCommand = QtIoDeviceChecksummedMessageReader::checksum(sizeOfCommandAsByteArray);
    QString checksumOfSizeOfCommandAsString = QString(checksumOfSizeOfCommand);
    m_SerialPortTextStream << checksumOfSizeOfCommandAsString; //TODOreq: s/Command/Data/Message (idc which, but keep it uniform mother fucker (eh "command" makes sense from wasdf business logic, but "message" and "data" make sense from "message header reader" perspective))
    m_SerialPortTextStream << delim;

    //send sizeOfData
    m_SerialPortTextStream << sizeofDataAsString;
    m_SerialPortTextStream << delim;

    //send checksumOfData
    QByteArray commandToSendToArduinoAsLatin1(commandToSendToArduino.toLatin1());
    QByteArray checksumOfCommand = QtIoDeviceChecksummedMessageReader::checksum(commandToSendToArduinoAsLatin1);
    QString checksumOfCommandAsString = QString(checksumOfCommand);
    m_SerialPortTextStream << checksumOfCommandAsString;
    m_SerialPortTextStream << delim;

    //send commandToSendToArduino
    m_SerialPortTextStream << commandToSendToArduino;

    //TODOreq: merge this assert with above
    //Q_ASSERT(commandToSendToArduino.length() < (1024)); //TODOreq: 1024 should be in a shared header file between sketch and this. the arduino calls String.reserve(1024) in setup()
    //m_SerialPortTextStream << commandToSendToArduino << "\n"; //don't use endl, that uses \r\n sometimes I think but might be wrong since the QIODevice isn't using QIODevice::Text? fuggit -- we don't need a newline delim anymore because our protocol specifies the "message size". we read "message size" bytes, then immediately start looking for the next SYNC... so I'm 99% sure newline (or a delim in between messages, of any kind) isn't needed anymore
    m_SerialPortTextStream.flush();

#if 0 //turns out I do need to send things over as ASCII. arduino uses 2-byte int size wtf??? I can't be fucked. I'm going to checksum (hash) the STRING representation of shit instead of the raw bytes
    QByteArray sync("SYNC"); //TODOoptimization: make this a member and it's initialization in construction phasemmandToSendToArduino.toLatin1());
    QByteArray commandToSendToArduinoAsLatin1(commandToSendToArduino.toLatin1());
    qint32 sizeOfCommand = commandToSendToArduino.size();
    QByteArray sizeOfCommandAsByteArray = QByteArray::fromRawData((const char*)(&sizeOfCommand), sizeof(sizeOfCommand));
    QByteArray checksumOfSizeOfCommand = QtIoDeviceChecksummedMessageReader::checksum(sizeOfCommandAsByteArray);
    qDebug() << checksumOfSizeOfCommand.size(); //TODOreq: this is for testing only. I think it should be 32 hex digits times 4 bytes per digit = 128-bits, or 16 bytes
    QByteArray checksumOfCommand = QtIoDeviceChecksummedMessageReader::checksum(commandToSendToArduinoAsLatin1);

    m_SerialPort->write(sync); //fixed size
    m_SerialPort->write(checksumOfSizeOfCommand); //fixed size
    m_SerialPort->write(sizeOfCommandAsByteArray); //fixed size
    m_SerialPort->write(checksumOfCommand); //fixed size
    m_SerialPort->write(commandToSendToArduinoAsLatin1); //variable size
#endif
}
bool WasdfArduino::detectAndHandleDebugMessage(const QByteArray &messageJson)
{
    QJsonParseError jsonParseError;
    QJsonDocument message = QJsonDocument::fromJson(messageJson, &jsonParseError);
    if(jsonParseError.error != QJsonParseError::NoError)
    {
        emit e("Error parsing json: " + jsonParseError.errorString());
        return false;
    }
    const QJsonObject &rootObject = message.object();
    QJsonObject::const_iterator it = rootObject.constFind(WASDF_JSON_KEY_DEBUGMESSAGE);
    if(it == rootObject.constEnd())
        return false;
    QString debugMessage = it.value().toString();
    emit e("debug message received from arduino: " + QString(debugMessage)); //TODOmb: emit debugMessageReceivedFromArduino(debugMessage) and let the business logic (class "Wasdf") handle it however it wants
    return true;
}
void WasdfArduino::startInCalibrationMode()
{
    //in calibration mode, arduino sends _ALL_ (not just 10) analog pin readings over serial (TODOmb: user can pass an --exclude-analog-pins flag in case they're using the other analog pins for something else (but then they'd need to modify the sketch anyways, so maybe this isn't necessary?)

    disconnect(m_ChecksummedMessageReader, SIGNAL(checksummedMessageRead(QByteArray))); //TODOreq: for some reason this fails with qt5-style pmf syntax. maybe a newer version of Qt doesn't (otherwise file a bugreport)
    connect(m_ChecksummedMessageReader, &QtIoDeviceChecksummedMessageReader::checksummedMessageRead, this, &WasdfArduino::handleCalibrationModeMessageReceived);
    //disconnect(m_SerialPort, SIGNAL(readyRead()));
    //connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadCalibrationMode);

    openSerialPortIfNotOpen();

    QJsonObject calibrateCommandJsonObject;
    QJsonValue calibrateCommandJsonValue(QString(WASDF_JSON_KEY_CALIBRATECOMMAND));
    calibrateCommandJsonObject.insert(WASDF_JSON_KEY_COMMAND, calibrateCommandJsonValue);

    QString calibrateCommandJson = jsonObjectToQString(calibrateCommandJsonObject);
    sendRawCommandToArduino(calibrateCommandJson); //TODOreq: these strings should be in a shared header, shared between the arduino sketch and this sauce
}
void WasdfArduino::start(const WasdfCalibrationConfiguration &calibrationConfig)
{
    m_CalibrationConfig = calibrationConfig;

    disconnect(m_ChecksummedMessageReader, &QtIoDeviceChecksummedMessageReader::checksummedMessageRead, this, &WasdfArduino::handleCalibrationModeMessageReceived);
    connect(m_ChecksummedMessageReader, &QtIoDeviceChecksummedMessageReader::checksummedMessageRead, this, &WasdfArduino::handleRegularModeMessageReceived);
    //connect(m_SerialPort, &QSerialPort::readyRead, this, &WasdfArduino::handleSerialPortReadyReadNormalFingerMovementMode);

    openSerialPortIfNotOpen();

    QJsonObject startCommandJsonObject;

    QJsonValue startCommandJsonValue(QString(WASDF_JSON_KEY_STARTCOMMAND));
    startCommandJsonObject.insert(WASDF_JSON_KEY_COMMAND, startCommandJsonValue);

    //insert "at rest ranges" as args to the "start" command
    QJsonObject fingersAtRestRanges;
    QHashIterator<Finger, WasdfCalibrationFingerConfiguration> it(calibrationConfig);
    while(it.hasNext())
    {
        it.next();
        //Finger fing = it.key();
        WasdfCalibrationFingerConfiguration fingConf = it.value();
        int atRestMin, atRestMax;
        fingConf.calculateAtRestRange(&atRestMin, &atRestMax);
        QJsonObject fingerJsonObject;
        fingerJsonObject.insert(WASDF_JSON_KEY_ATRESTMIN, atRestMin);
        fingerJsonObject.insert(WASDF_JSON_KEY_ATRESTMAX, atRestMax);
        fingersAtRestRanges.insert(QString::number(fingConf.AnalogPinIdOnArduino), fingerJsonObject); //TODOreq: what happens when same key is inserted twice? during testing especially this is going to happen, but even until I solve the "floating values" problem I still might see "same key" occurances. Ideally I'd never use the same key twice, because my calibrator would know not to emit that
    }
    startCommandJsonObject.insert(WASDF_JSON_KEY_ATRESTRANGES, fingersAtRestRanges);

    QString startCommandJson = jsonObjectToQString(startCommandJsonObject);
    sendRawCommandToArduino(startCommandJson);
}
void WasdfArduino::handleCalibrationModeMessageReceived(const QByteArray &messageJson)
{
    qDebug() << messageJson;
    if(detectAndHandleDebugMessage(messageJson))
        return;
    QJsonParseError jsonParseError;
    QJsonDocument message = QJsonDocument::fromJson(messageJson, &jsonParseError);
    if(jsonParseError.error != QJsonParseError::NoError)
    {
        emit e("Error parsing json: " + jsonParseError.errorString());
        return; //TODOreq: handle errors better
    }
    const QJsonObject &rootObject = message.object();
    for(QJsonObject::const_iterator it = rootObject.constBegin(); it != rootObject.constEnd(); ++it)
    {
        //TODOreq: sanitize better
        QString analogPinIdString = it.key();
        QByteArray analogPinIdBA = analogPinIdString.toLatin1();
        int analogPinId = analogPinIdBA.toInt();
        int sensorValue = it.value().toInt();
        if(sensorValue < MinAnalogPinValue || sensorValue > MaxAnalogPinValue)
        {
            qDebug() << "a sensor value was out of bounds:" << messageJson;
            continue;
        }
        emit analogPinReadingChangedDuringCalibration(analogPinId, sensorValue);
    }
#if 0 //TODOreq: merge below with above (nameley the sanitization)
    while(m_SerialPort->canReadLine())
    {
        QString line = m_SerialPortTextStream.readLine();
        emit e("raw line read from serial: " + line); //TODOreq: this is only here for testing
        //line is simply: "[pinId]:[sensorValue]"
        QStringList lineSplitAtColon = line.split(":");
        if(lineSplitAtColon.size() != 2)
        {
            qDebug() << "line didn't split evenly into 2 parts:" << line; //TODOmb: use emit v() for verbose or some such instead? in general I don't like to use qDebug/etc in my proper apps
            continue;
        }
        bool convertOk;
        int analogPinId = lineSplitAtColon.first().toInt(&convertOk);
        if(!convertOk)
        {
            qDebug() << "lhs of colon didn't convert to int:" << line;
            continue;
        }
        //TODOmb: some error checking of the analog pin id. eg can't be less than 0 (or maybe it can? I have no idea what arduino's rules are on this), but [during calibration (TODOreq: after calibration we know EXACTLY what 'finger ids' we want, so we can filter out everything else)] I'm not sure what a good upper max would be (unless I read them out of arduino headers)
        int sensorValue = lineSplitAtColon.at(1).toInt(&convertOk);
        if(!convertOk)
        {
            qDebug() << "rhs of colon didn't convert to int:" << line;
            continue;
        }

        //should I map the raw sensor values to the calibrated range here? should wasdf do it when it receives the signal emitted below? I think it kind of makes sense to do it right here, because Wasdf called m_Arduino.start(m_Calibration) ... so it makes sense that m_Arduino (this class) reports mapped/calibrated values
        //TODOreq: map the sensor value to 0-1023. wait no map it to their min/max range, wait no it's a 2 step process, map it to their min/max range and then map that to 0-1023? ehh need to think a little harder on this xD. there's also "constrain" to consider. also the above error checking needs to be modified accordingly

        emit analogPinReadingChangedDuringCalibration(analogPinId, sensorValue);
    }
#endif
}
void WasdfArduino::handleRegularModeMessageReceived(const QByteArray &messageJson)
{
    //TODOreq: similar to handleCalibrationModeMessageReceived (could probably share json parsing code paths), but we map the analog pin to Finger before emitting
    qDebug() << messageJson;
    if(detectAndHandleDebugMessage(messageJson))
        return;

    //at this point, we know m_CalibrationConfig is populated

    //this is modified copypasta from the similar calibration-related method above, handleCalibrationModeMessageReceived
    QJsonParseError jsonParseError;
    QJsonDocument message = QJsonDocument::fromJson(messageJson, &jsonParseError);
    if(jsonParseError.error != QJsonParseError::NoError)
    {
        emit e("Error parsing json: " + jsonParseError.errorString());
        return; //TODOreq: handle errors better
    }
    const QJsonObject &rootObject = message.object();
    for(QJsonObject::const_iterator it = rootObject.constBegin(); it != rootObject.constEnd(); ++it)
    {
        //TODOreq: sanitize better
        QString analogPinIdString = it.key();
        QByteArray analogPinIdBA = analogPinIdString.toLatin1();
        int analogPinId = analogPinIdBA.toInt();
        if(!m_CalibrationConfig.hasFingerWithAnalogPinId(analogPinId))
        {
            qWarning() << "no finger in m_CalibrationConfig with this analog pin id: " << analogPinId;
            continue;
        }
        int sensorValue = it.value().toInt();
        Finger fing = m_CalibrationConfig.getFingerByAnalogPinId(analogPinId);
        if(sensorValue < MinAnalogPinValue || sensorValue > MaxAnalogPinValue)
        {
            qDebug() << "a sensor value was out of bounds for your " << fingerEnumToHumanReadableString(fing) << "finger. the sensor value is " << sensorValue << ". here is the json too:" << messageJson;
            continue;
        }
        emit fingerMoved(fing, sensorValue);
    }
}
void WasdfArduino::dumpRawSerialStreamByPeeking()
{
    QByteArray peekedBytes = m_SerialPort->peek(m_SerialPort->bytesAvailable());
    QString peekedBytesAsString(peekedBytes);
    qDebug() << endl << "[BEGIN PEEKED BYTES]" << endl << peekedBytesAsString << endl << "[END PEEKED BYTES]" << endl << endl;
}

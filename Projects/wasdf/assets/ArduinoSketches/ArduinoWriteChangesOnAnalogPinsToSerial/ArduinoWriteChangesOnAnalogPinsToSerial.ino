//rewriting the first version of this instead of just removing calibration because I also want to try to not to use pp defines/macros. pure C++ is cleaner and easier to read/modify. aside from that it's just a functionally equivalent rewrite (with the calibration stripped out since that's going to be done on the PC side)

#ifndef WASDF_PC
    #include <Arduino.h> //for NUM_ANALOG_INPUTS
    #include <MD5.h> //for checksum
    #include <ArduinoJson.h> //for sanity
#endif // ndef WASDF_PC

//-------BEGIN PC-ARDUINO SHARED DEFINES
static const int NUM_FINGERS = 10;
static const char *SYNC = "SYNC";
static const int SizeOfaChecksum = 32; //MD5 uses 32x 4-bit hex digits (128-bits total). since we're passing the hex md5 as a "string", we need 32 bytes to hold it

static const int MinAnalogPinValue = 0;
static const int MaxAnalogPinValue = 1023;

static const int HALF_THRESHOLD_FOR_NOTIFYING_OF_CHANGE = 2; //TODOoptional: this could be determined during 'calibration' (so it'd be received from PC over serial). example instructions to uesr for determining it: "move your right index finger as little as possible until you see on screen that it's registered" or some such (haven't thought this through very much, maybe it's dumb idea)

#define WASDF_CHECKSUMMED_MESSAGE_HEADER_COMMADELIM ","
#define WASDF_JSON_KEY_DEBUGMESSAGE "debugMessage"
#define WASDF_JSON_KEY_COMMAND "command"
#define WASDF_JSON_KEY_CALIBRATECOMMAND "calibrate"
#define WASDF_JSON_KEY_STARTCOMMAND "start"
#define WASDF_JSON_KEY_ATRESTMIN "atRestMin"
#define WASDF_JSON_KEY_ATRESTMAX "atRestMax"
#define WASDF_JSON_KEY_ATRESTRANGES "fingersAtRestRanges"
//-------END PC-ARDUINO SHARED DEFINES

#ifndef WASDF_PC
static const int MinAnalogPinId = A0;
static const int MaxAnalogPinId = (A0 + (NUM_ANALOG_INPUTS-1));
//typedef StaticJsonBuffer<200> MyJsonBuffer;
typedef DynamicJsonBuffer MyJsonBuffer;
static const int MAX_MESSAGE_SIZE = 1024;
#if (NUM_ANALOG_INPUTS < NUM_FINGERS)
#error "Your board does not support at least 10 analog inputs" //TODOmb: fail gracefully. but don't simply uncomment this, otherwise we will probably have a memory access violation in this code. namely when we try to pull out the "old" sensor value of finger9, but we only allocated (on the stack) an array of size NUM_ANALOG_INPUTS to hold our old sensor values
#endif
#if (MESSAGE_HEADER_SIZE > 64)
#error "Message header size (which is fixed) is bigger than 64 bytes (Serial port's internal buffer size)!"
#endif

//TODOmb: when in calibrating mode maybe I should blink pin 13 rapidly, and when normal mode starts I should do 3 long pulses and then disable it (because leaving it on is dumb)
//TODOreq: during testing I noticed that the analog pin values with no sensor connected were "floating". not floating point or whatever, but their values were not zero and they also weren't consistent. they seemed to change WITH the analog pin that had a sensor connected, as it moved (I only tested with 1, but it probably applies with more than 1), but their values didn't match. I need to verify that my "pin detection" stuff is ok/safe to use (it seems to be fine despite that random floating/changing, but maybe I'm only getting lucky). If it isn't, I need to "require" 10 pins always connected, and only do FINGER DETECTION only by using the same code that I do right now for "pin detection to finger mapping", but altered slightly to only do it on  10 pins.  The default analog pins should be 0-9, but there should be a way for the user to pass in pins ("range" parsing is perhaps (but perhaps not after more thinking about it) a GUI thing, the business object wants those pin numbers in a QList<int>). What I need to verify is that the "total accrued distance moved" will always be greater on anlog pins with sensors connected; greater than pins with no sensors connected. but for now I'll just say fuck it (but maybe once 10 are connected it won't work ofc)
//TODOreq: any time a message header doesn't check out (checksum failure) or the data doesn't check out (checksum failure or just unknown command (same thing as message), we should just blink pin 13 to indicate error. this would at the very least tell us we need to reset the arduino. since input commands are so rare (FOR NOW. for my 0.1 target), so too will be the errors where I need to restart the arduino. ultimately though it would be nice to be fault tolerant, and there are TODOreq comments littered in this code indicating places where faults are detected
String checksumResult;
void checksum(const String &dataToChecksum)
{
    int dataLength = dataToChecksum.length();
    int dataLengthIncludingNullTerminator = dataLength + 1;
    char dataAsCharArray[dataLengthIncludingNullTerminator]; //I thought this was illegal to use a non-const array size, but uhhhh it's compiling/working just fine... maybe arduino doesn't use real C++ after all?? or maybe I'm just retarded? seriously wtf I thought non-const array sizes needed to use new/delete (heap vs stack)
    dataToChecksum.toCharArray(dataAsCharArray, dataLengthIncludingNullTerminator);
    unsigned char *rawChecksum = MD5::make_hash(dataAsCharArray, dataLength);
    char *hexChecksum_or_HexcumWoopsFreudianSlip = MD5::make_digest(rawChecksum, 16); //I have no idea why this 2nd arg is 16 instead of 32. //TODOoptimization: in theory no need to conver to hex because it's not going to be read by a human (could also remove make_digest from the md5lib in order to save space)
    checksumResult = hexChecksum_or_HexcumWoopsFreudianSlip;

    //TODOoptimization: both ptrs that are free'd here could probably be setup once in setup() and then re-used every time checksum() is called (and never freed because there's no shutdown() function xD)
    free(hexChecksum_or_HexcumWoopsFreudianSlip);
    free(rawChecksum);
}
void sendDebugMessage(const String &debugMessage)
{
    MyJsonBuffer jsonBuffer;
    JsonObject &myObject = jsonBuffer.createObject();
    myObject[WASDF_JSON_KEY_DEBUGMESSAGE] = debugMessage;
    String debugMessageJson;
    myObject.printTo(debugMessageJson);
    sendMessageToPc(debugMessageJson);
}
void fatalErrorBlinkPin13(int blinkIntervalSec)
{
    sendDebugMessage("fatalErrorBlinkPin13() called with interval: " + String(blinkIntervalSec));
    //hmm I thought about keeping track of the time using millis etc, but might as well just use a while(true) loop to simplify my life. this is a FATAL error after all :-D
    int blinkIntervalMSec = blinkIntervalSec*1000; //TODOkek: blink in morse code spelling out the function that called fatalErrorBlinkPin13() or the line number (if this wasn't a single-file dev enviornment (which is dumb, fkn arduino meh) then I'd say out put the filename in addition to the line number). of course another way of solving it is to attach an LCD (or dump stack trace over Serial?), but that is not as kek
    pinMode(13, OUTPUT);
    while(true)
    {
        while(Serial.available())
            Serial.read();
        digitalWrite(13, HIGH);
        delay(blinkIntervalMSec);
        digitalWrite(13, LOW);
        delay(blinkIntervalMSec);
    }
}
bool newSensorValueHasChangedEnoughThatWeWantToReportIt(int oldSensorValue, int newSensorValue)
{
    /*if(NewSensorValue == OldSensorValue)
        return false;
    return true;*/

    if(newSensorValue < (oldSensorValue-HALF_THRESHOLD_FOR_NOTIFYING_OF_CHANGE))
        return true;

    if(newSensorValue > (oldSensorValue+HALF_THRESHOLD_FOR_NOTIFYING_OF_CHANGE))
        return true;

    return false;
}
bool sanitizingAnalogRead(int analogPinId, int *out_analogSensorValue)
{
    if(analogPinId == -1)
        return false;
    int rawSensorValue = analogRead(analogPinId);
    if(rawSensorValue < MinAnalogPinValue || rawSensorValue > MaxAnalogPinValue)
        return false;
    *out_analogSensorValue = rawSensorValue;
    return true;
}
class Finger_aka_AnalogPin //TODOmb: shared declaration between PC and Arduino?
{
public:
    explicit Finger_aka_AnalogPin()
        : CurrentSensorValue(0)
        , OldSensorValue(0)
    { }
    void initialize(int analogPinId, int atRestMin, int atRestMax)
    {
        IntPinId = analogPinId;
        AtRestMin = atRestMin;
        AtRestMax = atRestMax;
        //TODOmb: sanitize at rest min/max here, fatal13 otherwise? idk I mean we've already verified the checksum of the data.. so.... yea idk tbh because it's still "foreign" data...? I guess this applies to pinId as well so fk idk lol what to assume... but FOR NOW at least I'll assume checksum valid == data valid (sane, within range, etc)
    }
    bool fingerMoved()
    {
         if(!sanitizingAnalogRead(IntPinId, &CurrentSensorValue))
             return false;
        if(CurrentSensorValue <= AtRestMax && CurrentSensorValue >= AtRestMin)
            return false; //finger is within "at rest" range, so return false
        if(newSensorValueHasChangedEnoughThatWeWantToReportIt(OldSensorValue, CurrentSensorValue))
        {
            /*
            Serial.print(StringPinId);
            Serial.print(":");
            Serial.println(NewSensorValue);
            */
            //*out_NewFingerPosition = NewSensorValue;
            OldSensorValue = CurrentSensorValue;
            return true;
        }
        return false;
    }
    ~Finger_aka_AnalogPin() { }

    int IntPinId;
    int CurrentSensorValue;
    int OldSensorValue;
    int AtRestMin;
    int AtRestMax;
private:
    explicit Finger_aka_AnalogPin(const Finger_aka_AnalogPin &other) { } //delete
};

void sendMessageToPc(const String &jsonString)
{
    //send over Serial: SYNC,checksumOfSize,Size,checksumOfData,Data
    //where jsonString is Data

    Serial.print(SYNC);
    Serial.print(WASDF_CHECKSUMMED_MESSAGE_HEADER_COMMADELIM);
    String jsonSizeString = String(jsonString.length());
    checksum(jsonSizeString);
    Serial.print(checksumResult /*checksumOfSize*/);
    Serial.print(WASDF_CHECKSUMMED_MESSAGE_HEADER_COMMADELIM);
    Serial.print(jsonSizeString);
    Serial.print(WASDF_CHECKSUMMED_MESSAGE_HEADER_COMMADELIM);
    checksum(jsonString);
    Serial.print(checksumResult /*checksumOfData*/);
    Serial.print(WASDF_CHECKSUMMED_MESSAGE_HEADER_COMMADELIM);
    Serial.print(jsonString);
}

class Hands
{
public:
    void reportFingerMovementOverSerialPort()
    {
        int fingerIndexesThatMoved[NUM_FINGERS];
        int currentIndexIntoFingerIndexesThatMoved = 0;

        bool atLeastOneFingerMoved = false;
        for(int i = 0; i < NUM_FINGERS; ++i)
        {
            if(Fingers[i].fingerMoved())
            {
                atLeastOneFingerMoved = true;
                fingerIndexesThatMoved[currentIndexIntoFingerIndexesThatMoved] = i;
                ++currentIndexIntoFingerIndexesThatMoved;
            }
        }
        if(atLeastOneFingerMoved)
        {
            //even though the arduino2pc protocol is extremely simple atm and never changes from "analogPinId:newValue", I still want to convert it to json so I can 'bundle' all 10 finger movements into a single json object and (this is the key part) I send that jsonString over using the "SYNC,checksumOfSize,Size,checksumOfData,Data" protocol -- the same protocol used for pc2arduino comm (would be nice if they could share code). Yes I don't need to convert to json, BUT checksumming each "pinId:newValue" _line_ is inefficient as fuck, and "batching" them into 10 is a million times easier using json. thus, json is warranted

            MyJsonBuffer jsonBuffer;
            JsonObject &myObject = jsonBuffer.createObject();
            for(int i = 0; i < currentIndexIntoFingerIndexesThatMoved; ++i)
            {
                const Finger_aka_AnalogPin &theFinger = Fingers[fingerIndexesThatMoved[i]];
                String key_aka_PinId(theFinger.IntPinId);
                myObject[key_aka_PinId] = theFinger.CurrentSensorValue;
            }
            String jsonString; //TODOoptimization: re-use. maybe need to clear() in between each printTo call?
            myObject.printTo(jsonString);
            sendMessageToPc(jsonString); //TODOreq: calibration mode should also call this sendMessageToPc function and send json over
        }
    }
    void initializeFinger(int fingerArrayIndex, int analogPinId, int atRestMin, int atRestMax)
    {
        Fingers[fingerArrayIndex].initialize(analogPinId, atRestMin, atRestMax);
    }
private:
    Finger_aka_AnalogPin Fingers[NUM_FINGERS];
};

struct Mode
{
    enum ModeEnum //TODOreq: I forget how to properly set up 'flags' that can be combined, this might be wrong (also calibrating can never be used with any other modes, but eh)
    {
          DoNothingMode = 0x000
        , Calibrating = 0x001
        , Sending10FingerMovementsMode = 0x010
        , KeyboardAndMouseMode = 0x100
    };
};
class MessageHeaderReader
{
private:
    enum MessageHeaderReaderState
    {
          LookingForSync = 0
        , ReadingChecksumOfSizeOfData = 1
        , ReadingSizeOfData = 2
        , ReadingChecksumOfData = 3
    };
    MessageHeaderReaderState m_State;

    bool tryReadUntilComma()
    {
        //TO DOnereq: the json inputCommand must not contain the word "SYNC". it can and will contain assloads of commas, which yes if we do get out of sync will trigger this shiz yada yada, but it's fine as long as SYNC isn't seen. actually fuck it, come to think of it even that doesn't matter... because the fkn checksums gotta check out ;-P. my protocol is finally robust
        while(Serial.available())
        {
            char inChar = (char)Serial.read();
            if(inChar == ',') //TODOreq: shared delim def in header
                return true;
            m_MessageHeaderBuffer += inChar;

            if(m_MessageHeaderBuffer.length() > SizeOfLargestHeaderComponent)
            {
                //while reading here and filling up m_MessageHeaderBuffer, we need to also occassionally shrink it. if we got out of sync then it might grow quite large before we see a comma again. the max size it needs to be is the biggest size of the 3 different message headers, so I'm guessing the hex encoding of [either of] the checksum[s] is going to be the biggest and therefore the max size we should allow m_MessageHeaderBuffer to become. we should drop characters starting from the very left, since new ones are appended on the right
                m_MessageHeaderBuffer = m_MessageHeaderBuffer.substring(1);
            }
        }
        return false;
    }
public:
    MessageHeaderReader()
        : m_State(LookingForSync)
        , SizeOfLargestHeaderComponent(SizeOfaChecksum /*32 hex digits for MD5 Checksums*/)
    { }
    bool tryReadMessageHeader(int *out_MessageSize)
    {
        switch(m_State)
        {
            case LookingForSync:
            {
                if(!tryReadUntilComma())
                {
                    return false;
                    break;
                }
                if(m_MessageHeaderBuffer.endsWith(SYNC))
                {
                    m_State = ReadingChecksumOfSizeOfData;
                }
                m_MessageHeaderBuffer = "";
            }
            //INTENTIONALLY NOT BREAKING HERE
            case ReadingChecksumOfSizeOfData:
            {
                if(!tryReadUntilComma())
                {
                    return false;
                    break;
                }
                m_ParsedChecksumOfSizeOfData = m_MessageHeaderBuffer;
                m_MessageHeaderBuffer = "";
                m_State = ReadingSizeOfData;
            }
            //INTENTIONALLY NOT BREAKING HERE
            case ReadingSizeOfData:
            {
                if(!tryReadUntilComma())
                {
                    return false;
                    break;
                }
                m_ParsedSizeOfData = m_MessageHeaderBuffer.toInt();
                m_ParsedSizeOfDataAsString = m_MessageHeaderBuffer;
                m_MessageHeaderBuffer = "";
                checksum(m_ParsedSizeOfDataAsString);
                if(checksumResult != m_ParsedChecksumOfSizeOfData)
                {
                    //fatalErrorBlinkPin13(2); //TODOreq: "request message is re-sent"
                    m_State = LookingForSync;
                    return false;
                    break;
                }
                m_State = ReadingChecksumOfData;
            }
            //INTENTIONALLY NOT BREAKING HERE
            case ReadingChecksumOfData:
            {
                if(!tryReadUntilComma())
                {
                    return false;
                    break;
                }
                m_ParsedMessageChecksum = m_MessageHeaderBuffer; //we have to wait until we read in the data before using this checksum
                m_MessageHeaderBuffer = "";
                *out_MessageSize = m_ParsedSizeOfData;
                m_State = LookingForSync;
                return true;
            }
            break;
        }
        return false; //should (will) never get here
    }
    const String &parsedMessageChecksum() const
    {
        return m_ParsedMessageChecksum;
    }
private:
    const int SizeOfLargestHeaderComponent;

    String m_MessageHeaderBuffer;

    String m_ParsedChecksumOfSizeOfData;
    int m_ParsedSizeOfData;
    String m_ParsedSizeOfDataAsString;

    String m_ParsedMessageChecksum;
};
class MessageReader
{
private:
    bool messageChecksumIsValid()
    {
        checksum(m_MessageBuffer);
        if(checksumResult == m_MessageHeaderReader.parsedMessageChecksum())
        {
            return true;
        }
        else
        {
            //fatalErrorBlinkPin13(3); //TODOreq: "request message is re-sent"
            m_HaveReadMessageHeader = false;
            return false;
        }
        return false;
    }
    bool tryReadMessageActual(String *out_Message)
    {
        int numBytesToRead = min(m_NumBytesOfMessageLeft, Serial.available());
        if(numBytesToRead < 1)
            return false;
        for(int i = 0; i < numBytesToRead; ++i)
        {
            m_MessageBuffer += (char)Serial.read(); //TODOoptimization: read chunks. idfk how on arduino tho kek
        }
        m_NumBytesOfMessageLeft -= numBytesToRead;
        if(m_NumBytesOfMessageLeft < 1)
        {
            //woot read in all of a message
            //now verify it's checksum
            m_HaveReadMessageHeader = false; //so next time we call tryReadMessage we'll be back to looking for the header
            if(messageChecksumIsValid())
            {
                *out_Message = m_MessageBuffer; //TODOoptimization: if this is deep copy, which it probably is, avoid it
                return true;
            }
            return false;
        }
        return false;
    }
public:
    MessageReader()
        : m_HaveReadMessageHeader(false)
    { }
    bool tryReadMessage(String *out_Message)
    {
        if(!m_HaveReadMessageHeader)
        {
            if(!m_MessageHeaderReader.tryReadMessageHeader(&m_MessageSize))
            {
                return false;
            }
            else
            {
                if(m_MessageSize > MAX_MESSAGE_SIZE)
                {
                    fatalErrorBlinkPin13(4);
                    return false;
                }
                m_HaveReadMessageHeader = true;
                m_NumBytesOfMessageLeft = m_MessageSize;
                m_MessageBuffer = "";
            }
        }

        //if we get here we've read the message header. either just now or on a previous call to tryReadMessage. it also means m_MessageSize is valid
        return tryReadMessageActual(out_Message);
    }
private:
    String m_MessageBuffer;
    MessageHeaderReader m_MessageHeaderReader;
    int m_MessageSize;
    bool m_HaveReadMessageHeader;
    int m_NumBytesOfMessageLeft;
};

//Globals
int CalibrationDataOldSensorValues[NUM_ANALOG_INPUTS];
Mode::ModeEnum CurrentMode;
Hands hands;
String inputCommandBuffer;
MessageReader *messageReader;

void setup()
{
    messageReader = 0;
    Serial.begin(38400 /*, SERIAL_8O2 TODOreq: I want to try out "Odd" (that's an oh not a zero in between 8 and 2) parity and 2 stop bits to see if it improves reliability, but I keep getting an error that begin() only takes one argument wtf? maybe I need to upgrade? but it doesn't even look like debian stretch bumped the version at all (except a small patch). the define SERIAL_8O2 does in fact exist, so wtf?*/);
    for(int i = 0; i < NUM_ANALOG_INPUTS; ++i)
        CalibrationDataOldSensorValues[i] = 0;
    CurrentMode = Mode::DoNothingMode;
    //commented this out because it's more memory conservative to allocate as needed: inputCommandBuffer.reserve(MAX_MESSAGE_SIZE); //TODOreq: calculate the max string length and reserve that much (or maybe a little more to be on the safe side). it can be determined because the values only take up so many characters. only thing is maybe the arduino mega 2560x1000 might have 999999999999 analog pins xDDDD, ya know what I mean??
    //inputCommandBuffer = new char(MAX_MESSAGE_SIZE); //TODOoptimization: never call new after takeoff. the md5 lib I'm gonna be using looks suspect af
    while(!Serial) ; //wait for serial port to connect. Needed for Leonardo/Micro only. I intentionally put the String::reserve command (and other cmds) in between this and Serial.begin() in case the connect happens asynchronously (idfk tbh, but it might), in which case I may have saved an entire millisecond!!!
}
bool analogPinSensorValueChangedDuringCalibration(int pinId, int indexIntoCalibrationDataOldSensorValues)
{
    int oldSensorValue = CalibrationDataOldSensorValues[indexIntoCalibrationDataOldSensorValues];
    int newSensorValue;
    if(!sanitizingAnalogRead(pinId, &newSensorValue))
        return false;
    if(newSensorValueHasChangedEnoughThatWeWantToReportIt(oldSensorValue, newSensorValue))
    {
        /*
        Serial.print(pinId);
        Serial.print(":");
        Serial.println(newSensorValue);
        */
        CalibrationDataOldSensorValues[indexIntoCalibrationDataOldSensorValues] = newSensorValue;
        return true;
    }
    return false;
}
void calibrationLoop()
{
    int CalibrationDataOldSensorValues_IndexesThatMoved[NUM_ANALOG_INPUTS];
    int currentIndex_CalibrationDataOldSensorValues_IndexesThatMoved = 0;

    bool atLeastOneAnalogPinSensorValueChanged = false;
    for(int j = 0, i = A0; j < NUM_ANALOG_INPUTS; ++j, ++i)
    {
        if(analogPinSensorValueChangedDuringCalibration(i, j))
        {
            atLeastOneAnalogPinSensorValueChanged = true;
            CalibrationDataOldSensorValues_IndexesThatMoved[currentIndex_CalibrationDataOldSensorValues_IndexesThatMoved] = i;
            ++currentIndex_CalibrationDataOldSensorValues_IndexesThatMoved;
        }
    }
    if(atLeastOneAnalogPinSensorValueChanged)
    {
        MyJsonBuffer jsonBuffer;
        JsonObject &myObject = jsonBuffer.createObject();
        for(int i = 0; i < currentIndex_CalibrationDataOldSensorValues_IndexesThatMoved; ++i)
        {
            int indexIntoCalibrationDataOfAnalogPinThatMoved = CalibrationDataOldSensorValues_IndexesThatMoved[i];
            int newSensorValue = CalibrationDataOldSensorValues[indexIntoCalibrationDataOfAnalogPinThatMoved];
            myObject[String(A0 + indexIntoCalibrationDataOfAnalogPinThatMoved)] = newSensorValue;
        }
        String jsonString; //TODOoptimization: re-use. maybe need to clear() in between each printTo call?
        myObject.printTo(jsonString);
        sendMessageToPc(jsonString);
    }
}
int parseAndSanitizeAnalogPinIdFromForeignString(const String &foreignString)
{
    int ret = foreignString.toInt();
    //if(ret < 0 || ret > 9)
    if(ret < MinAnalogPinId || ret > MaxAnalogPinId)
    {
        fatalErrorBlinkPin13(7);
        return 69; //this never gets called because fatalErrorBlinkPin13() never returns
    }
    return ret;
}
void processInputCommandString(const String &inputCommandString)
{
    MyJsonBuffer jsonBuffer;
    const JsonObject &rootJsonObject = jsonBuffer.parseObject(inputCommandString);
    if(!rootJsonObject.success())
    {
        fatalErrorBlinkPin13(5);
        return;
    }

    String command = rootJsonObject[WASDF_JSON_KEY_COMMAND];

    String debugMessage("received command from pc: \"");
    debugMessage += command;
    debugMessage += "\". here is the corresponding json: ";
    debugMessage += inputCommandString;
    sendDebugMessage(debugMessage);

    if(command == WASDF_JSON_KEY_CALIBRATECOMMAND)
    {
        CurrentMode = Mode::Calibrating;
    }
    else if(command == WASDF_JSON_KEY_STARTCOMMAND)
    {
        CurrentMode = Mode::Sending10FingerMovementsMode;
        //the "used analog pin IDs" (connected to the 10 fingers) is sent as args (TODOreq: make sure PC isn't sending "0-9" but is instead sending the analog pin IDs corresponding to fingers 0-9
        //retrive atRestMin/Max from json, corresponding to the analog pin IDs being used
        const JsonObject &atRestPositionsJsonObject = rootJsonObject[WASDF_JSON_KEY_ATRESTRANGES];
        JsonObject::const_iterator it = atRestPositionsJsonObject.begin();
        int i = 0; //we don't care whether or not this finger2analogPinId mapping is correct or not, it's merely used as array index. we only care about "which analog pin ids to send changes for" and their respective "atRestMin"/"atRestMax" values, not necessarily which fingers those analog pin IDs correspond to (this might change in the future, eg eeprom impl, but for now is true)
        while(it != atRestPositionsJsonObject.end())
        {
            const String &analogPinIdAsString = it->key;
            int analogPinId = parseAndSanitizeAnalogPinIdFromForeignString(analogPinIdAsString);
            const JsonObject &atRestRange = it->value;
            int atRestMin = atRestRange[WASDF_JSON_KEY_ATRESTMIN];
            int atRestMax = atRestRange[WASDF_JSON_KEY_ATRESTMAX];
            if(i >= NUM_FINGERS)
            {
                fatalErrorBlinkPin13(8);
                return;
            }
            hands.initializeFinger(i, analogPinId, atRestMin, atRestMax);
            ++it, ++i;
        }
        if(i < NUM_FINGERS)
        {
            fatalErrorBlinkPin13(6);
            return;
        }
        debugMessage = "started";
        sendDebugMessage(debugMessage);
    }
    else
    {
        debugMessage = "unknown command: \"";
        debugMessage += command;
        debugMessage += "\". here is the corresponding json: ";
        debugMessage += inputCommandString;
        sendDebugMessage(debugMessage);
        //TODOreq: blink pin 13 rapidly to indicate an error. longer term should request the command is re-sent. note: even if the checksum succeeds we still might get an invalid command (checksums aren't perfect), we we'd STILL want to request the command is re-sent. if however 50 invalid commands are received IN A ROW, then we would want to go into blink-13-error-mode as that indicates a bug
        fatalErrorBlinkPin13(1);
    }
}
void sendOutputIfAny()
{
    switch(CurrentMode) //TODOreq: this switch case doesn't support simultaneous 10-fingers and keyboardMouse modes
    {
        case Mode::Sending10FingerMovementsMode:
            hands.reportFingerMovementOverSerialPort(); //tell, don't ask ;-)
        break;
        case Mode::KeyboardAndMouseMode:
            //TODOreq. I'm starting to lean towards synthing kb/mouse on pc side instead of using the micro/leonardo built-in approach, for various reasons (serialization/correct-replaying of the input command stream greatly simplified being the biggest)
        break;
        case Mode::Calibrating:
            calibrationLoop();
        break;
        case Mode::DoNothingMode:
            //I used to hav a delay(50) here, until I learned that Serial has internal buffer size of only 64 bytes
        break;
    }
}
void receiveInputIfAny()
{
    if(messageReader == 0)
    {
        if(Serial.available())
            messageReader = new MessageReader();
        else
            return;
    }
    if(messageReader->tryReadMessage(&inputCommandBuffer))
    {
        delete messageReader; //OOM optimization
        messageReader = 0;
        processInputCommandString(inputCommandBuffer);
    }
}
void loop()
{
    sendOutputIfAny();
    receiveInputIfAny();
    delay(1);
}
#endif // ndef WASDF_PC

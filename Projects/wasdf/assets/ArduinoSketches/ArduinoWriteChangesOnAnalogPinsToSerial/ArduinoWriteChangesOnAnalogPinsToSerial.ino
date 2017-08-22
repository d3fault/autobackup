//rewriting the first version of this instead of just removing calibration because I also want to try to not to use pp defines/macros. pure C++ is cleaner and easier to read/modify. aside from that it's just a functionally equivalent rewrite (with the calibration stripped out since that's going to be done on the PC side)

#include <Arduino.h> //for NUM_ANALOG_INPUTS
#include <MD5.h> //for checksum
//#include <ArduinoJson.h> //for sanity

static const String Sync("SYNC"); //TODOreq: use definition in header shared between PC and Arduino
static const int SizeOfaChecksum = 32; //MD5 uses 32x 4-bit hex digits (128-bits total). since we're passing the hex md5 as a "string", we need 32 bytes to hold it

#if (NUM_ANALOG_INPUTS < 10)
#error "Your board does not support at least 10 analog inputs" //TODOmb: fail gracefully. but don't simply uncomment this, otherwise we will probably have a memory access violation in this code. namely when we try to pull out the "old" sensor value of finger9, but we only allocated (on the stack) an array of size NUM_ANALOG_INPUTS to hold our old sensor values
#endif
#if (MESSAGE_HEADER_SIZE > 64)
#error "Message header size (which is fixed) is bigger than 64 bytes (Serial port's internal buffer size)!"
#endif

static const int NUM_HANDS = 2;
static const int NUM_FINGERS_PER_HAND = 5;

static const int HALF_THRESHOLD_FOR_NOTIFYING_OF_CHANGE = 2; //TODOoptional: this could be determined during 'calibration' (so it'd be received from PC over serial). example instructions to uesr for determining it: "move your right index finger as little as possible until you see on screen that it's registered" or some such (haven't thought this through very much, maybe it's dumb idea)

static const int MAX_MESSAGE_SIZE = 1024;

//TODOmb: when in calibrating mode maybe I should blink pin 13 rapidly, and when normal mode starts I should do 3 long pulses and then disable it (because leaving it on is dumb)
//TODOreq: during testing I noticed that the analog pin values with no sensor connected were "floating". not floating point or whatever, but their values were not zero and they also weren't consistent. they seemed to change WITH the analog pin that had a sensor connected, as it moved (I only tested with 1, but it probably applies with more than 1), but their values didn't match. I need to verify that my "pin detection" stuff is ok/safe to use (it seems to be fine despite that random floating/changing, but maybe I'm only getting lucky). If it isn't, I need to "require" 10 pins always connected, and only do FINGER DETECTION only by using the same code that I do right now for "pin detection to finger mapping", but altered slightly to only do it on  10 pins.  The default analog pins should be 0-9, but there should be a way for the user to pass in pins ("range" parsing is perhaps (but perhaps not after more thinking about it) a GUI thing, the business object wants those pin numbers in a QList<int>). What I need to verify is that the "total accrued distance moved" will always be greater on anlog pins with sensors connected; greater than pins with no sensors connected. but for now I'll just say fuck it (but maybe once 10 are connected it won't work ofc)
//TODOreq: any time a message header doesn't check out (checksum failure) or the data doesn't check out (checksum failure or just unknown command (same thing as message), we should just blink pin 13 to indicate error. this would at the very least tell us we need to reset the arduino. since input commands are so rare (FOR NOW. for my 0.1 target), so too will be the errors where I need to restart the arduino. ultimately though it would be nice to be fault tolerant, and there are TODOreq comments littered in this code indicating places where faults are detected
void fatalErrorBlinkPin13(int blinkIntervalSec)
{
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
class Finger_aka_AnalogPin
{
public:
    explicit Finger_aka_AnalogPin()
        : NewSensorValue(0)
        , OldSensorValue(0)
    { }
    void setStringAndIntPinIds(const char *stringPinId, int intPinId)
    {
        StringPinId = stringPinId;
        IntPinId = intPinId;
    }
    void reportMovementOverSerialPort()
    {
        NewSensorValue = analogRead(IntPinId);
        if(newSensorValueHasChangedEnoughThatWeWantToReportIt(OldSensorValue, NewSensorValue))
        {
            Serial.print(StringPinId);
            Serial.print(":");
            Serial.println(NewSensorValue);
            OldSensorValue = NewSensorValue;
        }
    }
    ~Finger_aka_AnalogPin() { }

    const char *StringPinId;
    int IntPinId;
    int NewSensorValue;
    int OldSensorValue;

private:
    explicit Finger_aka_AnalogPin(const Finger_aka_AnalogPin &other) { } //delete
};

struct Hand
{
    void setupHand(bool leftTrue_rightFalse)
    {
        //TODOreq: the arduino micro doesn't use analog pins 0-9 (note that finger 9 below maps to pin A11), so the following mappings are arduino micro specific. there should obviously be some way to use this code with other boards of course (even a mapping 'file' is better than hardcoding it here). the BEST solution is to, before/during calibration, 'probe' every analog port to see if it's wired, and then to have the user tell us which finger that pin corresponds to.. then to yea save it in some ~/.config 'profile' thingo. for now I'm fine with hardcoding the pin mappings since my target is to get range-of-fingers realtime-with-feedback calibration working [first]. I wonder what happens when you read an analog pin that has nothing attached... it returns 0. in any case and whatever value it is, it shouldn't "move" (I hope)
        //TODOreq:^I also need to change the MusicFingers serial READ code (runs on PC) to use 0-9 and not the whacky micro-specific mapping like in the previous version of this code

        //TODOreq: this obviously needs to be refactored to use the calibration data received from the PC
        if(leftTrue_rightFalse)
        {
            //left hand
            Fingers[0].setStringAndIntPinIds("0", A0);
            Fingers[1].setStringAndIntPinIds("1", A1);
            Fingers[2].setStringAndIntPinIds("2", A2);
            Fingers[3].setStringAndIntPinIds("3", A3);
            Fingers[4].setStringAndIntPinIds("4", A4);
        }
        else
        {
            //right hand
            Fingers[0].setStringAndIntPinIds("5", A5);
            Fingers[1].setStringAndIntPinIds("6", A8);
            Fingers[2].setStringAndIntPinIds("7", A9);
            Fingers[3].setStringAndIntPinIds("8", A10);
            Fingers[4].setStringAndIntPinIds("9", A11);
        }
    }
    void reportFingerMovementOverSerialPort()
    {
        for(int i = 0; i < NUM_FINGERS_PER_HAND; ++i)
        {
            Fingers[i].reportMovementOverSerialPort();
        }
    }

    Finger_aka_AnalogPin Fingers[NUM_FINGERS_PER_HAND];
};

struct Hands //eh hands class should have just contained 10 fingers (instead of 2 hands each containing 5 fingers), but not about to un-code it xD
{
    explicit Hands()
    {
        hands[0].setupHand(true);
        hands[1].setupHand(false);
    }

    void reportFingerMovementOverSerialPort()
    {
        for(int i = 0; i < NUM_HANDS; ++i)
        {
            hands[i].reportFingerMovementOverSerialPort();
        }
    }

    Hand hands[NUM_HANDS];
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
void checksum(const String &dataToChecksum, String *out_HexChecksum)
{
    int dataLength = dataToChecksum.length();
    int dataLengthIncludingNullTerminator = dataLength + 1;
    char dataAsCharArray[dataLengthIncludingNullTerminator]; //I thought this was illegal to use a non-const array size, but uhhhh it's compiling/working just fine... maybe arduino doesn't use real C++ after all?? or maybe I'm just retarded? seriously wtf I thought non-const array sizes needed to use new/delete (heap vs stack)
    dataToChecksum.toCharArray(dataAsCharArray, dataLengthIncludingNullTerminator);
    unsigned char *rawChecksum = MD5::make_hash(dataAsCharArray, dataLength);
    char *hexChecksum_or_HexcumWoopsFreudianSlip = MD5::make_digest(rawChecksum, 16); //I have no idea why this 2nd arg is 16 instead of 32. //TODOoptimization: in theory no need to conver to hex because it's not going to be read by a human (could also remove make_digest from the md5lib in order to save space)
    *out_HexChecksum = hexChecksum_or_HexcumWoopsFreudianSlip;

    //TODOoptimization: both ptrs that are free'd here could probably be setup once in setup() and then re-used every time checksum() is called (and never freed because there's no shutdown() function xD)
    free(hexChecksum_or_HexcumWoopsFreudianSlip);
    free(rawChecksum);
}
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
                if(m_MessageHeaderBuffer.endsWith(Sync))
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
                checksum(m_ParsedSizeOfDataAsString, &m_CalculatedChecksumOfSizeOfData);
                if(m_CalculatedChecksumOfSizeOfData != m_ParsedChecksumOfSizeOfData)
                {
                    fatalErrorBlinkPin13(2); //TODOreq: remove this call to fatal when "requesting message is re-sent" is implemented
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
    String m_CalculatedChecksumOfSizeOfData;

    String m_ParsedMessageChecksum;
};
class MessageReader
{
private:
    bool messageChecksumIsValid()
    {
        checksum(m_MessageBuffer, &m_CalculatedMessageChecksum);
        if(m_CalculatedMessageChecksum == m_MessageHeaderReader.parsedMessageChecksum())
        {
            return true;
        }
        else
        {
            fatalErrorBlinkPin13(3); //TODOreq: remove this call to fatal when "requesting message is re-sent" is implemented
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
    String m_CalculatedMessageChecksum;
};


//Globals
int CalibrationDataOldSensorValues[NUM_ANALOG_INPUTS];
Mode::ModeEnum CurrentMode;
Hands hands;
String inputCommandBuffer;
MessageReader messageReader;

void setup()
{
    Serial.begin(38400 /*, SERIAL_8O2 TODOreq: I want to try out "Odd" (that's an oh not a zero in between 8 and 2) parity and 2 stop bits to see if it improves reliability, but I keep getting an error that begin() only takes one argument wtf? maybe I need to upgrade? but it doesn't even look like debian stretch bumped the version at all (except a small patch). the define SERIAL_8O2 does in fact exist, so wtf?*/);
    for(int i = 0; i < NUM_ANALOG_INPUTS; ++i)
        CalibrationDataOldSensorValues[i] = 0;
    CurrentMode = Mode::DoNothingMode;
    inputCommandBuffer.reserve(MAX_MESSAGE_SIZE); //TODOreq: calculate the max string length and reserve that much (or maybe a little more to be on the safe side). it can be determined because the values only take up so many characters. only thing is maybe the arduino mega 2560x1000 might have 999999999999 analog pins xDDDD, ya know what I mean??
    //inputCommandBuffer = new char(MAX_MESSAGE_SIZE); //TODOoptimization: never call new after takeoff. the md5 lib I'm gonna be using looks suspect af
    while(!Serial) ; //wait for serial port to connect. Needed for Leonardo/Micro only. I intentionally put the String::reserve command (and other cmds) in between this and Serial.begin() in case the connect happens asynchronously (idfk tbh, but it might), in which case I may have saved an entire millisecond!!!
}
void readAndReportChangesToAnalogPinOverSerial(int pinId, int indexIntoCalibrationDataOldSensorValues)
{
    int oldSensorValue = CalibrationDataOldSensorValues[indexIntoCalibrationDataOldSensorValues];
    int newSensorValue = analogRead(pinId);
    if(newSensorValueHasChangedEnoughThatWeWantToReportIt(oldSensorValue, newSensorValue))
    {
        Serial.print(pinId);
        Serial.print(":");
        Serial.println(newSensorValue);
        CalibrationDataOldSensorValues[indexIntoCalibrationDataOldSensorValues] = newSensorValue;
    }
}
void calibrationLoop()
{
    for(int j = 0, i = A0; j < NUM_ANALOG_INPUTS; ++j, ++i)
    {
        readAndReportChangesToAnalogPinOverSerial(i, j);
    }
}
void processInputCommandString(const String &inputCommandString)
{
    if(inputCommandString == "calibrate")
    {
        CurrentMode = Mode::Calibrating;
    }
#if 0 //TODOreq
    else if(inputCommandString.startsWith("startReportingThesePinsExcludingTheirAtRestRanges:"))
    {
        //TODOreq:
        //where the fuck is my String.split!?!? aww come pls don't make me use indexOf :(
        //*cough* json *cough*

    }
#endif
    else
    {
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
    if(messageReader.tryReadMessage(&inputCommandBuffer))
        processInputCommandString(inputCommandBuffer);
}
void loop()
{
    sendOutputIfAny();
    receiveInputIfAny();
}

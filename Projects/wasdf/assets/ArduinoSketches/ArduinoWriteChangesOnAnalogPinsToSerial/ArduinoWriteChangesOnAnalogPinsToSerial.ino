//rewriting the first version of this instead of just removing calibration because I also want to try to not to use pp defines/macros. pure C++ is cleaner and easier to read/modify. aside from that it's just a functionally equivalent rewrite (with the calibration stripped out since that's going to be done on the PC side)

#include <Arduino.h> //for NUM_ANALOG_INPUTS

#if (NUM_ANALOG_INPUTS < 10)
#error "Your board does not support at least 10 analog inputs" //TODOmb: fail gracefully. but don't simply uncomment this, otherwise we will probably have a memory access violation in this code. namely when we try to pull out the "old" sensor value of finger9, but we only allocated (on the stack) an array of size NUM_ANALOG_INPUTS to hold our old sensor values
#endif

static const int NUM_HANDS = 2;
static const int NUM_FINGERS_PER_HAND = 5;

static const int HALF_THRESHOLD_FOR_NOTIFYING_OF_CHANGE = 2; //TODOoptional: this could be determined during 'calibration' (so it'd be received from PC over serial). example instructions to uesr for determining it: "move your right index finger as little as possible until you see on screen that it's registered" or some such (haven't thought this through very much, maybe it's dumb idea)

//TODOmb: when in calibrating mode maybe I should blink pin 13 rapidly, and when normal mode starts I should do 3 long pulses and then disable it (because leaving it on is dumb)
//TODOreq: during testing I noticed that the analog pin values with no sensor connected were "floating". not floating point or whatever, but their values were not zero and they also weren't consistent. they seemed to change WITH the analog pin that had a sensor connected, as it moved (I only tested with 1, but it probably applies with more than 1), but their values didn't match. I need to verify that my "pin detection" stuff is ok/safe to use (it seems to be fine despite that random floating/changing, but maybe I'm only getting lucky). If it isn't, I need to "require" 10 pins always connected, and only do FINGER DETECTION only by using the same code that I do right now for "pin detection to finger mapping", but altered slightly to only do it on  10 pins.  The default analog pins should be 0-9, but there should be a way for the user to pass in pins ("range" parsing is perhaps (but perhaps not after more thinking about it) a GUI thing, the business object wants those pin numbers in a QList<int>). What I need to verify is that the "total accrued distance moved" will always be greater on anlog pins with sensors connected; greater than pins with no sensors connected. but for now I'll just say fuck it (but maybe once 10 are connected it won't work ofc)
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

//Globals
int CalibrationDataOldSensorValues[NUM_ANALOG_INPUTS];
String inputCommandString;
Mode::ModeEnum CurrentMode;
Hands hands;

void setup()
{
    Serial.begin(38400 /*, SERIAL_8O2 TODOreq: I want to try out "Odd" (that's an oh not a zero in between 8 and 2) parity and 2 stop bits to see if it improves reliability, but I keep getting an error that begin() only takes one argument wtf? maybe I need to upgrade? but it doesn't even look like debian stretch bumped the version at all (except a small patch). the define SERIAL_8O2 does in fact exist, so wtf?*/);
    for(int i = 0; i < NUM_ANALOG_INPUTS; ++i)
        CalibrationDataOldSensorValues[i] = 0;
    CurrentMode = Mode::DoNothingMode;
    inputCommandString.reserve(1024); //TODOreq: calculate the max string length and reserve that much (or maybe a little more to be on the safe side). it can be determined because the values only take up so many characters. only thing is maybe the arduino mega 2560x1000 might have 999999999999 analog pins xDDDD, ya know what I mean??
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
void processInputCommandString()
{
    if(inputCommandString == "calibrate")
    {
        CurrentMode = Mode::Calibrating;
    }
    else if(inputCommandString.startsWith("startReportingThesePinsExcludingTheirAtRestRanges:"))
    {
        //TODOreq:
        //where the fuck is my String.split!?!? aww come pls don't make me use indexOf :(
        //*cough* json *cough*

    }
    else
    {
        //TODOreq: blink pin 13 rapidly to indicate an error. longer term should request the command is re-sent. note: even if the checksum succeeds we still might get an invalid command (checksums aren't perfect), we we'd STILL want to request the command is re-sent. if however 50 invalid commands are received IN A ROW, then we would want to go into blink-13-error-mode as that indicates a bug
    }
}
void loop()
{
    while(Serial.available())
    {
        char inChar = (char)Serial.read();
        if(inChar == '\n')
        {
            processInputCommandString();
            inputCommandString = "";
        }
        else
            inputCommandString += inChar;
    }
    switch(CurrentMode) //TODOreq: this switch case doesn't support simultaneous 10-fingers and keyboardMouse modes
    {
        case Mode::Sending10FingerMovementsMode:
            hands.reportFingerMovementOverSerialPort(); //tell, don't ask ;-)
        break;
        case Mode::KeyboardAndMouseMode:
            //TODOreq
        break;
        case Mode::Calibrating:
            calibrationLoop();
        break;
        case Mode::DoNothingMode:
            delay(50); //mb this saves battery? w/e lol
        break;
    }

    delay(2); //TODOreq: this might fuck up MOUSE MOVEMENT, everything else should be fine with it tho
}

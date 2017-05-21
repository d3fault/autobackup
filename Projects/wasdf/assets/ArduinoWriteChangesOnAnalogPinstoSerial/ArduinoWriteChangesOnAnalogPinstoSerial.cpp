//rewriting the first version of this instead of just removing calibration because I also want to try to not to use pp defines/macros. pure C++ is cleaner and easier to read/modify. aside from that it's just a functionally equivalent rewrite (with the calibration stripped out since that's going to be done on the PC side)

static const int NUM_ANALOG_PINS_TOTAL = 15; //TODOreq: find a good maximum. maybe use whatever my mega 2560 uses (and cross fingers that boards with fewer analog pins don't crash (could always patch arduino sauce so it returns 0 when requested pin DNE))

static const int NUM_HANDS = 2;
static const int NUM_FINGERS_PER_HAND = 5;

static const int HALF_THRESHOLD_FOR_NOTIFYING_OF_CHANGE = 2; //TODOoptional: this could be determined during 'calibration' (so it'd be received from PC over serial). example instructions to uesr for determining it: "move your right index finger as little as possible until you see on screen that it's registered" or some such (haven't thought this through very much, maybe it's dumb idea)

//TODOmb: when in calibrating mode maybe I should blink pin 13 rapidly, and when normal mode starts I should do 3 long pulses and then disable it (because leaving it on is dumb)
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

struct Hands
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
    enum ModeEnum
    {
          DoNothingMode = 0x000
        , Calibrating = 0x001
        , Sending10FingerMovementsMode = 0x010
        , KeyboardAndMouseMode = 0x100
    };
};

//Globals
int CalibrationDataOldSensorValues[NUM_ANALOG_PINS_TOTAL];
String inputCommandString;
Mode::ModeEnum CurrentMode;
Hands hands;

void setup()
{
    Serial.begin(9600);
    for(int i = 0; i < NUM_ANALOG_PINS_TOTAL; ++i)
        CalibrationDataOldSensorValues[i] = 0;
    CurrentMode = Mode::DoNothingMode;
    inputCommandString.reserve(512); //TODOreq: ensure command strings never grow beyond 512 bytes
    while(!Serial) ; //wait for serial port to connect. Needed for Leonardo/Micro only. I intentionally put the String::reserve command (and other cmds) in between this and Serial.begin() in case the connect happens asynchronously (idfk tbh, but it might), in which case I may have saved an entire millisecond!!!
}
void calibrationLoop()
{
    for(int i = A0, j = 0; i < NUM_ANALOG_PINS_TOTAL; ++i, ++j)
    {
        int oldSensorValue = CalibrationDataOldSensorValues[j];
        int newSensorValue = analogRead(i);
        if(newSensorValueHasChangedEnoughThatWeWantToReportIt(oldSensorValue, newSensorValue))
        {
            Serial.print(i);
            Serial.print(":");
            Serial.println(newSensorValue);
            CalibrationDataOldSensorValues[j] = newSensorValue;
        }
    }
}
void loop()
{
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
void processInputCommandString()
{
    if(inputCommandString == "calibrate")
    {
        CurrentMode = Mode::Calibrating;
        return;
    }
}
void serialEvent()
{
    while(Serial.available())
    {
        char inChar = (char)Serial.read();
        inputCommandString += inChar;
        if(inChar == '\n')
        {
            processInputCommandString();
            inputCommandString = "";
        }
    }
}

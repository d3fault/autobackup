int sensorValue = 0;
#define MUSIC_FINGERS_CALIBRATION_MAX 127 //TODOreq: this should be 1023. The value should be mapped to 0-127 only JUST before being sent to stk-demo's stdin (MAYBE a tiny bit earlier, so we 'know' what stk-demo currently has as a value... but definitely not here on the arduino). I think, though, that calibration has to be moved off of the Arduino in order for that to happen (which requires a more robust serial protocol (so fuck that for now. KISS))

#define MUSIC_FINGRERS_ANALOG_PIN_VAR(pin) analogInPin_##pin
#define MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) pin##_newSensorValue
#define MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin) pin##_oldSensorValue
#define MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin) pin##_minSensorValue
#define MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin) pin##_maxSensorValue

#define MUSIC_FINGER_GLOBALS(pin) \
const int MUSIC_FINGRERS_ANALOG_PIN_VAR(pin) = pin; \
int MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) = 0; \
int MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin) = 0; \
int MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin) = 1023; \
int MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin) = 0;


/* EX:
int analogPin_A0 = 0;
int A0_newSensorValue = 0;
int A0_oldSensorValue = 0;
int A0_minSensorValue = 1023;
int A0_maxSensorValue = 0;
*/
MUSIC_FINGER_GLOBALS(A0)
MUSIC_FINGER_GLOBALS(A1)
MUSIC_FINGER_GLOBALS(A2)
MUSIC_FINGER_GLOBALS(A3)
MUSIC_FINGER_GLOBALS(A4)
MUSIC_FINGER_GLOBALS(A5)
MUSIC_FINGER_GLOBALS(A8)
MUSIC_FINGER_GLOBALS(A9)
MUSIC_FINGER_GLOBALS(A10)
MUSIC_FINGER_GLOBALS(A11)


#define MUSIC_FINGERS_CALIBRATE(pin) \
sensorValue = analogRead(pin); \
if(sensorValue > MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin)) \
{ \
  MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin) = sensorValue; \
} \
if(sensorValue < MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin)) \
{ \
  MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin) = sensorValue; \
}


#define MUSIC_FINGERS_MAIN_LOOP(pinQuoted, pin) \
MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) = analogRead(MUSIC_FINGRERS_ANALOG_PIN_VAR(pin)); \
if(MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) < (MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin)-halfThresholdForNotifyingOfChange) || MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) > (MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin)+halfThresholdForNotifyingOfChange)) \
{ \
  Serial.print(pinQuoted); \
  Serial.print(":"); \
  int sensorValueMappedAkaCalibrated = map(MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin), MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin), MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin), 0, MUSIC_FINGERS_CALIBRATION_MAX); /* 0-127 corresponds to the typical synthesis toolkit min/max ranges it accepts for the frequency on stdin....*/ \
  sensorValueMappedAkaCalibrated = constrain(sensorValueMappedAkaCalibrated, 0, MUSIC_FINGERS_CALIBRATION_MAX); \
  Serial.println(sensorValueMappedAkaCalibrated); /*TODOoptimization: pretty sure i don't need newlines, but I can't be fucked atm. edit: newlines cost just as much as any other delimiter, so what gives? i think now that i have a colon-separated-protocol, this optimization can no longer happen*/ \
  MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin) = MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin); \
}


int halfThresholdForNotifyingOfChange = 2; //plus OR minus, hence 'half'

void setup()
{
  Serial.begin(9600);

  //BEGIN Calibrate
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  while(millis() < 5000)
  {
    MUSIC_FINGERS_CALIBRATE(A0)
    MUSIC_FINGERS_CALIBRATE(A1)
    MUSIC_FINGERS_CALIBRATE(A2)
    MUSIC_FINGERS_CALIBRATE(A3)
    MUSIC_FINGERS_CALIBRATE(A4)
    MUSIC_FINGERS_CALIBRATE(A5)
    MUSIC_FINGERS_CALIBRATE(A8)
    MUSIC_FINGERS_CALIBRATE(A9)
    MUSIC_FINGERS_CALIBRATE(A10)
    MUSIC_FINGERS_CALIBRATE(A11)
  }
  digitalWrite(13, LOW);
  //END Calibrate
}
void loop()
{
  MUSIC_FINGERS_MAIN_LOOP("0", A0)
  MUSIC_FINGERS_MAIN_LOOP("1", A1)
  MUSIC_FINGERS_MAIN_LOOP("2", A2)
  MUSIC_FINGERS_MAIN_LOOP("3", A3)
  MUSIC_FINGERS_MAIN_LOOP("4", A4)
  MUSIC_FINGERS_MAIN_LOOP("5", A5)
  MUSIC_FINGERS_MAIN_LOOP("8", A8)
  MUSIC_FINGERS_MAIN_LOOP("9", A9)
  MUSIC_FINGERS_MAIN_LOOP("10", A10)
  MUSIC_FINGERS_MAIN_LOOP("11", A11)
  
  delay(2);
}


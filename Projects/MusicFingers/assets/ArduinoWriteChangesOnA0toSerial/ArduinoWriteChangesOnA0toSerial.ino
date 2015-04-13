int sensorValue = 0;
#define MUSIC_FINGERS_CALIBRATION_MAX 127

#define MUSIC_FINGRERS_ANALOG_PIN_VAR(pin) analogInPin_##pin
#define MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) pin##_newSensorValue
#define MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin) pin##_oldSensorValue
#define MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin) pin##_minSensorValue
#define MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin) pin##_maxSensorValue

#define MUSIC_FINGER_GLOBALS(pin) \
const int MUSIC_FINGRERS_ANALOG_PIN_VAR(pin) = pin; \
int MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) = 0; \
int MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin) = 0; \
int MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin) = 1024; \
int MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin) = 0;


MUSIC_FINGER_GLOBALS(A0)
//MUSIC_FINGER_GLOBALS(A1)

#define MUSIC_FINGERS_CALIBRATE(pin) \
sensorValue = analogRead(A##pin); \
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
  int sensorValueMappedAkaCalibrated = map(USIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin), MUSIC_FINGERS_MIN_SENSOR_VALUE_VAR(pin), MUSIC_FINGERS_MAX_SENSOR_VALUE_VAR(pin), 0, MUSIC_FINGERS_CALIBRATION_MAX); /* 0-127 corresponds to the typical synthesis toolkit min/max ranges it accepts for the frequency on stdin....*/ \
  sensorValueMappedAkaCalibrated = constrain(sensorValueMappedAkaCalibrated, 0, MUSIC_FINGERS_CALIBRATION_MAX); \
  Serial.println(sensorValueMappedAkaCalibrated); /*TODOoptimization: pretty sure i don't need newlines, but I can't be fucked atm*/ \
  MUSIC_FINGERS_OLD_SENSOR_VALUE_VAR(pin) = MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin); \
}


int halfThresholdForNotifyingOfChange = 2;

void setup()
{
  Serial.begin(9600);

  //BEGIN Calibrate
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  while(millis() < 5000)
  {
    MUSIC_FINGERS_CALIBRATE(A0)
    //MUSIC_FINGERS_CALIBRATE(A1)
  }
  digitalWrite(13, LOW);
  //END Calibrate
}
void loop()
{
  MUSIC_FINGERS_MAIN_LOOP("0", A0)
  //MUSIC_FINGERS_MAIN_LOOP("1", A1)
  
  delay(2);
}

